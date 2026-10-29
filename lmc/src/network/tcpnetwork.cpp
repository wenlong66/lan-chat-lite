/****************************************************************************
**
** This file is part of LAN Messenger.
** 
** Copyright (c) 2010 - 2012 Qualia Digital Solutions.
** 
** Contact:  qualiatech@gmail.com
** 
** LAN Messenger is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** LAN Messenger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with LAN Messenger.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#include <QVariant>
#include <QNetworkProxy>
#include "core/trace.h"
#include "tcpnetwork.h"
#include "datagram.h"
#include "core/settings.h"

lmcTcpNetwork::lmcTcpNetwork(void) {
  sendList.clear();
  receiveList.clear();
  messageMap.clear();
  locMsgStream = NULL;
  crypto = NULL;
  isRunning = false;
  ipAddress = QHostAddress::Null;
  server = new QTcpServer(this);
  server->setProxy(QNetworkProxy::NoProxy);
  connect(server, &QTcpServer::newConnection, this, &lmcTcpNetwork::server_newConnection);
}

void lmcTcpNetwork::init(int nPort) {
  tcpPort = nPort;
}

void lmcTcpNetwork::start(void) {
  lmcTrace::write("Starting TCP server");
  isRunning = server->listen(QHostAddress::Any, tcpPort > 0 ? tcpPort : IDS_TCPPORT_LEGACY_VAL);
  if(!isRunning && tcpPort == 0 && server->serverError() == QAbstractSocket::AddressInUseError) {
    isRunning = server->listen(QHostAddress::Any, 0);
  }
  if(isRunning) {
    int actualPort = server->serverPort();
    lmcTrace::write("TCP server listening on port " + QString::number(actualPort));
  } else {
    lmcTrace::write("TCP server listening failed " + server->errorString());
  }
}

void lmcTcpNetwork::stop(void) {
  server->close();
  const auto senders = sendList;
  sendList.clear();
  for(FileSender* transfer : senders) {
    transfer->stop();
    transfer->deleteLater();
  }
  const auto receivers = receiveList;
  receiveList.clear();
  for(FileReceiver* transfer : receivers) {
    transfer->stop();
    transfer->deleteLater();
  }
  // Detach the maps before closing sockets: disconnected() may otherwise
  // modify them synchronously while shutdown is iterating. Include older
  // live streams superseded by a simultaneous incoming connection.
  const auto streams = findChildren<MsgStream*>(QString(), Qt::FindDirectChildrenOnly);
  messageMap.clear();
  locMsgStream = nullptr;
  for(MsgStream* stream : streams) {
    disconnect(stream, nullptr, this, nullptr);
    stream->stop();
    stream->deleteLater();
  }
  for(QTcpSocket* socket : server->findChildren<QTcpSocket*>(QString(), Qt::FindDirectChildrenOnly)) {
    socket->abort();
    socket->deleteLater();
  }
  isRunning = false;
}

void lmcTcpNetwork::setLocalId(QString* lpszLocalId) {
  localId = *lpszLocalId;
}

void lmcTcpNetwork::setCrypto(lmcCrypto* pCrypto) {
  crypto = pCrypto;
}

void lmcTcpNetwork::addConnection(QString* lpszUserId, QString* lpszAddress, int nPort) {
  if(!isRunning) {
    lmcTrace::write("Warning: TCP server not running. Unable to connect");
    return;
  }

  int peerPort = nPort > 0 ? nPort : IDS_TCPPORT_LEGACY_VAL;
  lmcTrace::write("Connecting to user " + *lpszUserId + " at " + *lpszAddress + ":" + QString::number(peerPort));

  MsgStream* msgStream = new MsgStream(localId, *lpszUserId, *lpszAddress, peerPort);
  msgStream->setParent(this);
  connect(msgStream, &MsgStream::connectionLost, this, &lmcTcpNetwork::msgStream_connectionLost);
  connect(msgStream, &MsgStream::messageReceived, this, &lmcTcpNetwork::receiveMessage);

  // Preserve the upstream simultaneous incoming/outgoing session behavior.
  if(lpszUserId->compare(localId) == 0)
    locMsgStream = msgStream;
  else
    messageMap.insert(*lpszUserId, msgStream);
  msgStream->init();
}

void lmcTcpNetwork::sendMessage(QString* lpszReceiverId, QString* lpszData) {
  if(!isRunning) {
    lmcTrace::write("Warning: TCP server not running. Message not sent");
    return;
  }

  MsgStream* msgStream;

  if(lpszReceiverId->compare(localId) == 0)
    msgStream = locMsgStream;
  else
    msgStream = messageMap.value(*lpszReceiverId, NULL);

  if(msgStream) {
    lmcTrace::write("Sending TCP data stream to user " + *lpszReceiverId);
    QByteArray clearData = lpszData->toUtf8();
    QByteArray cipherData = crypto->encrypt(lpszReceiverId, clearData);
    if(cipherData.isEmpty()) {
      lmcTrace::write("Warning: Message could not be sent");
      return;
    }
    //	cipherData should now contain encrypted content
    Datagram::addHeader(DT_Message, cipherData);
    msgStream->sendMessage(cipherData);
    return;
  }

  lmcTrace::write("Warning: Socket not found. Message sending failed");
}

//	Point-in-time: a dead connection flips this back via removeUser.
bool lmcTcpNetwork::canSendTo(QString* lpszUserId) {
  if(!isRunning)
    return false;
  if(lpszUserId->compare(localId) == 0)
    return locMsgStream != NULL && crypto && crypto->hasSession(lpszUserId);
  if(!messageMap.contains(*lpszUserId))
    return false;
  return crypto && crypto->hasSession(lpszUserId);
}

void lmcTcpNetwork::initSendFile(QString* lpszReceiverId, QString* lpszAddress, QString* lpszData, int nPort) {
  XmlMessage xmlMessage(*lpszData);
  int type = Helper::indexOf(FileTypeNames, FT_Max, xmlMessage.data(XN_FILETYPE));

  int peerPort = nPort > 0 ? nPort : IDS_TCPPORT_LEGACY_VAL;
  lmcTrace::write("Sending file " + xmlMessage.data(XN_FILEID) + " to user " + *lpszReceiverId + " at " +
                  *lpszAddress + ":" + QString::number(peerPort));

  FileSender* sender = new FileSender(xmlMessage.data(XN_FILEID), localId, *lpszReceiverId, xmlMessage.data(XN_FILEPATH),
                                      xmlMessage.data(XN_FILENAME), xmlMessage.data(XN_FILESIZE).toLongLong(), *lpszAddress, peerPort, (FileType)type);
  sender->setParent(this);
  connect(sender, &FileSender::progressUpdated, this, &lmcTcpNetwork::update);
  sendList.prepend(sender);
  sender->init();
}

void lmcTcpNetwork::initReceiveFile(QString* lpszSenderId, QString* lpszAddress, QString* lpszData) {
  XmlMessage xmlMessage(*lpszData);
  int type = Helper::indexOf(FileTypeNames, FT_Max, xmlMessage.data(XN_FILETYPE));

  FileReceiver* receiver = new FileReceiver(xmlMessage.data(XN_FILEID), *lpszSenderId, xmlMessage.data(XN_FILEPATH),
                                            xmlMessage.data(XN_FILENAME), xmlMessage.data(XN_FILESIZE).toLongLong(), *lpszAddress, tcpPort, (FileType)type);
  receiver->setParent(this);
  connect(receiver, &FileReceiver::progressUpdated, this, &lmcTcpNetwork::update);
  receiveList.prepend(receiver);
}

void lmcTcpNetwork::fileOperation(FileMode mode, QString* lpszUserId, QString* lpszData) {
  Q_UNUSED(lpszUserId);

  XmlMessage xmlMessage(*lpszData);

  int fileOp = Helper::indexOf(FileOpNames, FO_Max, xmlMessage.data(XN_FILEOP));
  QString id = xmlMessage.data(XN_FILEID);

  if(mode == FM_Send) {
    FileSender* sender = getSender(id, *lpszUserId);
    if(!sender)
      return;

    switch(fileOp) {
    case FO_Cancel:
    case FO_Abort:
      sender->stop();
      removeSender(sender);
      break;
    }
  } else {
    FileReceiver* receiver = getReceiver(id, *lpszUserId);
    if(!receiver)
      return;

    switch(fileOp) {
    case FO_Cancel:
    case FO_Abort:
      receiver->stop();
      removeReceiver(receiver);
      break;
    }
  }
}

void lmcTcpNetwork::settingsChanged(void) {
}

void lmcTcpNetwork::setIPAddress(const QString& szAddress) {
  ipAddress = QHostAddress(szAddress);
}

int lmcTcpNetwork::serverPort(void) const {
  if(!isRunning)
    return 0;
  int actualPort = server->serverPort();
  return actualPort > 0 ? actualPort : tcpPort;
}

void lmcTcpNetwork::server_newConnection(void) {
  while(server->hasPendingConnections()) {
    QTcpSocket* socket = server->nextPendingConnection();
    // Bound unclassified connections, including peers sending no header.
    socket->setReadBufferSize(1024);
    connect(socket, &QIODevice::readyRead, this, &lmcTcpNetwork::socket_readyRead);
    connect(socket, &QAbstractSocket::disconnected, socket, [socket] {
      if(!socket->property("lclClaimed").toBool())
        socket->deleteLater();
    });
    QTimer::singleShot(30000, socket, [socket] {
      if(!socket->property("lclClaimed").toBool()) {
        socket->abort();
        socket->deleteLater();
      }
    });
  }
}

void lmcTcpNetwork::socket_readyRead(void) {
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  if(!socket || socket->bytesAvailable() < 3)
    return;

  const QByteArray pending = socket->peek(1024);
  if(pending.startsWith("MSG")) {
    // Preserve the upstream text-session handshake. File headers below
    // have an independently known peer ID and can be framed exactly.
    disconnect(socket, &QIODevice::readyRead, this, &lmcTcpNetwork::socket_readyRead);
    QByteArray header = socket->read(64);
    socket->setProperty("lclClaimed", true);
    socket->setReadBufferSize(0);
    QString userId = QString::fromLocal8Bit(header.mid(3));
    addMsgSocket(&userId, socket);
    return;
  }
  if(pending.size() < 4)
    return;
  if(pending.startsWith("FILE")) {
    if(pending.size() < 36)
      return;
    const QString id = QString::fromLatin1(pending.mid(4, 32));
    bool partialMatch = false;
    for(FileReceiver* receiver : receiveList) {
      if(receiver->id != id)
        continue;
      const QByteArray expected = QByteArrayLiteral("FILE") + id.toLatin1() + receiver->peerId.toUtf8();
      if(expected.size() > 1024)
        continue;
      if(pending.size() >= expected.size() && pending.startsWith(expected)) {
        socket->read(expected.size());
        disconnect(socket, &QIODevice::readyRead, this, &lmcTcpNetwork::socket_readyRead);
        socket->setProperty("lclClaimed", true);
        receiver->init(socket);
        return;
      }
      if(expected.startsWith(pending))
        partialMatch = true;
    }
    if(partialMatch)
      return;
  }
  // No matching accepted transfer: do not keep orphan sockets alive.
  socket->abort();
  socket->deleteLater();
}

void lmcTcpNetwork::msgStream_connectionLost(QString* lpszUserId) {
  MsgStream* stream = qobject_cast<MsgStream*>(sender());
  const QString userId = *lpszUserId;
  bool current = false;
  if(locMsgStream == stream) {
    locMsgStream = nullptr;
    current = true;
  }
  if(messageMap.value(userId, nullptr) == stream) {
    messageMap.remove(userId);
    current = true;
  }
  if(stream)
    stream->deleteLater();
  if(current) {
    QString copy = userId;
    emit connectionLost(&copy);
  }
}

void lmcTcpNetwork::update(FileMode mode, FileOp op, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData) {
  XmlMessage xmlMessage;
  xmlMessage.addHeader(XN_FROM, *lpszUserId);
  xmlMessage.addHeader(XN_TO, localId);
  xmlMessage.addData(XN_MODE, FileModeNames[mode]);
  xmlMessage.addData(XN_FILETYPE, FileTypeNames[type]);
  xmlMessage.addData(XN_FILEOP, FileOpNames[op]);
  xmlMessage.addData(XN_FILEID, *lpszId);

  switch(op) {
  case FO_Complete:
  case FO_Error:
    xmlMessage.addData(XN_FILEPATH, *lpszData);
    if(mode == FM_Send)
      removeSender(static_cast<FileSender*>(sender()));
    else
      removeReceiver(static_cast<FileReceiver*>(sender()));
    break;
  case FO_Progress:
    xmlMessage.addData(XN_FILESIZE, *lpszData);
    break;
  default:
    break;
  }

  QString szMessage = xmlMessage.toString();
  emit progressReceived(lpszUserId, &szMessage);
}

void lmcTcpNetwork::receiveMessage(QString* lpszUserId, QString* lpszAddress, QByteArray& datagram) {
  DatagramHeader* pHeader = NULL;
  if(!Datagram::getHeader(datagram, &pHeader))
    return;

  pHeader->userId = *lpszUserId;
  pHeader->address = *lpszAddress;
  QByteArray cipherData = Datagram::getData(datagram);
  QByteArray clearData;
  QString szMessage;

  lmcTrace::write("TCP stream type " + QString::number(pHeader->type) +
                  " received from user " + *lpszUserId + " at " + *lpszAddress);

  switch(pHeader->type) {
  case DT_PublicKey:
    //	send a session key back
    sendSessionKey(lpszUserId, cipherData);
    break;
  case DT_Handshake:
    // decrypt aes key and iv with private key
    crypto->retreiveAES(&pHeader->userId, cipherData);
    emit newConnection(&pHeader->userId, &pHeader->address);
    break;
  case DT_Message:
    // decrypt message with aes
    clearData = crypto->decrypt(&pHeader->userId, cipherData);
    if(clearData.isEmpty()) {
      lmcTrace::write("Warning: Message could not be retrieved");
      break;
    }
    szMessage = QString::fromUtf8(clearData.data(), clearData.length());
    emit messageReceived(pHeader, &szMessage);
    break;
  default:
    break;
  }
}

void lmcTcpNetwork::addFileSocket(QString* lpszId, QString* lpszUserId, QTcpSocket* pSocket) {
  FileReceiver* receiver = getReceiver(*lpszId, *lpszUserId);
  if(receiver)
    receiver->init(pSocket);
  else {
    pSocket->abort();
    pSocket->deleteLater();
  }
}

void lmcTcpNetwork::addMsgSocket(QString* lpszUserId, QTcpSocket* pSocket) {
  lmcTrace::write("Accepted connection from user " + *lpszUserId);
  QString address = pSocket->peerAddress().toString();
  MsgStream* msgStream = new MsgStream(localId, *lpszUserId, address, tcpPort);
  msgStream->setParent(this);
  connect(msgStream, &MsgStream::connectionLost, this, &lmcTcpNetwork::msgStream_connectionLost);
  connect(msgStream, &MsgStream::messageReceived, this, &lmcTcpNetwork::receiveMessage);
  messageMap.insert(*lpszUserId, msgStream);
  msgStream->init(pSocket);

  sendPublicKey(lpszUserId);
}

//	Once a new incoming connection is established, the server sends a public key to client
void lmcTcpNetwork::sendPublicKey(QString* lpszUserId) {
  lmcTrace::write("Sending public key to user " + *lpszUserId);
  MsgStream* msgStream = messageMap.value(*lpszUserId);
  if(msgStream) {
    QByteArray publicKey = crypto->publicKey;
    QString sh = DatagramTypeNames[DT_PublicKey];
    Datagram::addHeader(DT_PublicKey, publicKey);
    msgStream->sendMessage(publicKey);
  }
}

//	Once the public key from server is received, the client sends a session key which is
//	encrypted with the public key of the server
void lmcTcpNetwork::sendSessionKey(QString* lpszUserId, QByteArray& publicKey) {
  MsgStream* msgStream;

  if(lpszUserId->compare(localId) == 0)
    msgStream = locMsgStream;
  else
    msgStream = messageMap.value(*lpszUserId);

  if(msgStream) {
    lmcTrace::write("Sending session key to user " + *lpszUserId);
    QByteArray sessionKey = crypto->generateAES(lpszUserId, publicKey);
    Datagram::addHeader(DT_Handshake, sessionKey);
    msgStream->sendMessage(sessionKey);
  }
}

FileSender* lmcTcpNetwork::getSender(QString id, QString userId) {
  for(int index = 0; index < sendList.count(); index++)
    if(sendList[index]->id.compare(id) == 0 && sendList[index]->peerId.compare(userId) == 0)
      return sendList[index];

  return NULL;
}

FileReceiver* lmcTcpNetwork::getReceiver(QString id, QString userId) {
  for(int index = 0; index < receiveList.count(); index++)
    if(receiveList[index]->id.compare(id) == 0 && receiveList[index]->peerId.compare(userId) == 0)
      return receiveList[index];

  return NULL;
}

void lmcTcpNetwork::removeSender(FileSender* pSender) {
  int index = sendList.indexOf(pSender);
  if(index < 0)
    return;
  FileSender* sender = sendList.takeAt(index);
  sender->deleteLater(); // deleting later is generally safer
}

void lmcTcpNetwork::removeReceiver(FileReceiver* pReceiver) {
  int index = receiveList.indexOf(pReceiver);
  if(index < 0)
    return;
  FileReceiver* receiver = receiveList.takeAt(index);
  receiver->deleteLater(); // deleting later is generally safer
}
