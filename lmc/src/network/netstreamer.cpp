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


#include <QDir>
#include <QFileInfo>
#include <QDataStream>
#include <QNetworkProxy>
#include "core/trace.h"
#include "netstreamer.h"

namespace {
// Per transfer, Qt's userspace network queue is capped at 512 KiB. The
// filesystem chunk is 64 KiB, independent of the advertised file size.
constexpr qint64 kQueueLimit = 512 * 1024;
constexpr qint64 kReadBudget = 1024 * 1024;
constexpr qsizetype kControlLimit = 128;
constexpr quint32 kMessageLimit = 1024 * 1024;
constexpr int kHandshakeTimeoutMs = 5000;
const QByteArray kStart = QByteArrayLiteral("START2\n");
const QByteArray kDonePrefix = QByteArrayLiteral("DONE2 ");
}

/****************************************************************************
** Class: FileSender
** Description: Bounded streaming sender, with receiver-confirmed completion.
****************************************************************************/
FileSender::FileSender(void) = default;

FileSender::FileSender(QString szId, QString szLocalId, QString szPeerId, QString szFilePath,
                       QString szFileName, qint64 nFileSize, QString szAddress, int nPort,
                       FileType nType, int timeoutMs) {
  id = szId;
  localId = szLocalId;
  peerId = szPeerId;
  filePath = szFilePath;
  fileName = szFileName;
  fileSize = nFileSize;
  address = szAddress;
  port = nPort;
  type = nType;
  idleTimeoutMs = qMax(1, timeoutMs);
}

FileSender::~FileSender(void) {
  stop();
}

bool FileSender::terminal(void) const {
  return state == State::Complete || state == State::Failed || state == State::Stopped;
}

void FileSender::init(void) {
  if(state != State::Idle)
    return;
  state = State::WaitingForStart;
  socket = new QTcpSocket(this);
  socket->setProxy(QNetworkProxy::NoProxy);
  socket->setReadBufferSize(kControlLimit);
  connect(socket, &QAbstractSocket::connected, this, &FileSender::connected);
  connect(socket, &QAbstractSocket::disconnected, this, &FileSender::disconnected);
  connect(socket, &QIODevice::readyRead, this, &FileSender::readyRead);
  connect(socket, &QIODevice::bytesWritten, this, &FileSender::bytesWritten);
  connect(socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
    // A receipt can be buffered when the remote peer closes. Consume it
    // before treating the close as an error.
    if(error == QAbstractSocket::RemoteHostClosedError)
      readyRead();
    if(!terminal())
      fail(socket ? socket->errorString() : QStringLiteral("Connection lost"));
  });
  connect(&timer, &QTimer::timeout, this, &FileSender::timer_timeout);
  idleTimer.start();
  timer.start(qMin(PROGRESS_TIMEOUT, qMax(1, idleTimeoutMs / 4)));
  socket->connectToHost(QHostAddress(address), port);
}

void FileSender::stop(void) {
  if(!terminal())
    state = State::Stopped;
  timer.stop();
  if(file.isOpen())
    file.close();
  if(socket)
    socket->abort();
}

void FileSender::fail(const QString& reason) {
  if(terminal())
    return;
  state = State::Failed;
  stop();
  lmcTrace::write(QStringLiteral("File send failed: ") + reason);
  QString data = reason;
  emit progressUpdated(FM_Send, FO_Error, type, &id, &peerId, &data);
}

void FileSender::complete(void) {
  if(terminal())
    return;
  state = State::Complete;
  timer.stop();
  file.close();
  if(socket)
    socket->disconnectFromHost();
  emit progressUpdated(FM_Send, FO_Complete, type, &id, &peerId, &filePath);
}

void FileSender::connected(void) {
  QByteArray data = id.toLocal8Bit();
  data.append(localId.toUtf8());
  data.insert(0, "FILE");
  // This legacy routing header is consumed by lmcTcpNetwork before it
  // hands the socket to FileReceiver. File payloads use START2/DONE2.
  if(socket->write(data) != data.size())
    fail(QStringLiteral("Unable to write file routing header"));
}

void FileSender::disconnected(void) {
  readyRead();
  if(!terminal())
    fail(QStringLiteral("Peer disconnected without confirming the saved file"));
}

void FileSender::readyRead(void) {
  if(!socket || terminal())
    return;
  const qint64 room = kControlLimit - control.size();
  if(room <= 0) {
    fail(QStringLiteral("Oversized transfer control message"));
    return;
  }
  control.append(socket->read(room));
  while(!terminal()) {
    const qsizetype newline = control.indexOf('\n');
    if(newline < 0)
      break;
    const QByteArray line = control.left(newline + 1);
    control.remove(0, newline + 1);
    if(state == State::WaitingForStart) {
      if(line != kStart) {
        fail(QStringLiteral("File transfer requires LanChat Lite START2 support on both devices"));
        return;
      }
      idleTimer.restart();
      sendFile();
    } else if(state == State::WaitingForReceipt) {
      const QByteArray expected = kDonePrefix + hash.result().toHex() + '\n';
      if(line != expected) {
        fail(QStringLiteral("Receiver receipt is invalid or file SHA-256 does not match"));
        return;
      }
      complete();
    } else {
      fail(QStringLiteral("Unexpected transfer control message"));
      return;
    }
  }
  if(!terminal() && control.size() >= kControlLimit)
    fail(QStringLiteral("Oversized transfer control message"));
}

void FileSender::timer_timeout(void) {
  if(terminal())
    return;
  const int deadline = state == State::WaitingForStart ? qMin(idleTimeoutMs, kHandshakeTimeoutMs) : idleTimeoutMs;
  if(idleTimer.hasExpired(deadline)) {
    fail(state == State::WaitingForStart
           ? QStringLiteral("No START2 response; both devices must run LanChat Lite")
           : QStringLiteral("File transfer timed out without confirmed progress"));
    return;
  }
  if(state == State::Sending || state == State::WaitingForReceipt) {
    QString transferred = QString::number(qMax(qint64(0), queuedFileBytes - socket->bytesToWrite()));
    emit progressUpdated(FM_Send, FO_Progress, type, &id, &peerId, &transferred);
  }
}

void FileSender::bytesWritten(qint64 bytes) {
  if(terminal())
    return;
  if(bytes > 0)
    idleTimer.restart();
  if(state == State::Sending)
    pump();
}

void FileSender::pump(void) {
  if(state != State::Sending || !socket)
    return;
  while(state == State::Sending && socket->bytesToWrite() < kQueueLimit) {
    if(bufferOffset == bufferLength) {
      if(file.pos() == fileSize) {
        if(file.size() != fileSize) {
          fail(QStringLiteral("Source file size changed during transfer"));
          return;
        }
        file.close();
        state = State::WaitingForReceipt;
        // Includes zero-length files: never wait for a bytesWritten event
        // that a zero-byte write cannot generate.
        return;
      }
      const qint64 wanted = qMin(qint64(buffer.size()), fileSize - file.pos());
      bufferLength = file.read(buffer.data(), wanted);
      bufferOffset = 0;
      if(bufferLength <= 0) {
        fail(QStringLiteral("Source file read failed or ended before its advertised size"));
        return;
      }
      hash.addData(QByteArrayView(buffer.data(), bufferLength));
    }
    const qint64 amount = qMin(bufferLength - bufferOffset, kQueueLimit - socket->bytesToWrite());
    const qint64 written = socket->write(buffer.data() + bufferOffset, amount);
    if(written < 0) {
      fail(QStringLiteral("Unable to queue file data"));
      return;
    }
    if(written == 0)
      return;
    bufferOffset += written;
    queuedFileBytes += written;
  }
}

void FileSender::sendFile(void) {
  if(state != State::WaitingForStart)
    return;
  file.setFileName(filePath);
  if(fileSize < 0 || !file.open(QIODevice::ReadOnly) || file.size() != fileSize) {
    fail(QStringLiteral("Source file cannot be opened or no longer matches the offered size"));
    return;
  }
  state = State::Sending;
  hash.reset();
  pump();
}

/****************************************************************************
** Class: FileReceiver
** Description: Bounded streaming into an atomic temporary file.
****************************************************************************/
FileReceiver::FileReceiver(void) = default;

FileReceiver::FileReceiver(QString szId, QString szPeerId, QString szFilePath, QString szFileName,
                           qint64 nFileSize, QString szAddress, int nPort, FileType nType,
                           int timeoutMs) {
  id = szId;
  peerId = szPeerId;
  filePath = szFilePath;
  fileName = szFileName;
  fileSize = nFileSize;
  address = szAddress;
  port = nPort;
  type = nType;
  idleTimeoutMs = qMax(1, timeoutMs);
}

FileReceiver::~FileReceiver(void) {
  stop();
}

bool FileReceiver::terminal(void) const {
  return state == State::Complete || state == State::Failed || state == State::Stopped;
}

void FileReceiver::init(QTcpSocket* incoming) {
  if(state != State::Idle || !incoming) {
    if(incoming && incoming != socket) {
      incoming->abort();
      incoming->deleteLater();
    }
    return;
  }
  socket = incoming;
  socket->setParent(this);
  socket->setReadBufferSize(kQueueLimit);
  connect(socket, &QAbstractSocket::disconnected, this, &FileReceiver::disconnected);
  connect(socket, &QIODevice::readyRead, this, &FileReceiver::readyRead);
  connect(socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
    // The final body bytes can already be buffered at a peer close.
    readyRead();
    if(!terminal())
      fail(socket ? socket->errorString() : QStringLiteral("Connection lost"));
  });
  if(!receiveFile())
    return;
  if(socket->write(kStart) != kStart.size()) {
    fail(QStringLiteral("Unable to start file transfer"));
    return;
  }
  if(fileSize == 0)
    finish();
  else if(socket->bytesAvailable())
    readyRead();
}

void FileReceiver::stop(void) {
  if(!terminal())
    state = State::Stopped;
  timer.stop();
  // QSaveFile destruction cancels and removes an incomplete temporary file;
  // a pre-existing destination is preserved on cancellation or failure.
  file.reset();
  if(socket)
    socket->abort();
}

void FileReceiver::fail(const QString& reason) {
  if(terminal())
    return;
  state = State::Failed;
  stop();
  lmcTrace::write(QStringLiteral("File receive failed: ") + reason);
  QString data = reason;
  emit progressUpdated(FM_Receive, FO_Error, type, &id, &peerId, &data);
}

void FileReceiver::finish(void) {
  if(state != State::Receiving || receivedBytes != fileSize)
    return;
  // commit() checks pending writes, closes the file and atomically renames
  // it; never acknowledge a failed/partial write or overwrite on failure.
  if(!file->commit()) {
    fail(QStringLiteral("Destination could not be committed to disk"));
    return;
  }
  file.reset();
  const QByteArray receipt = kDonePrefix + hash.result().toHex() + '\n';
  state = State::Complete;
  timer.stop();
  // Local receive is complete even if the reply is lost. Only the sender
  // that actually reads this digest is allowed to report send completion.
  if(socket) {
    // The owner removes FileReceiver as soon as FO_Complete is emitted.
    // Let the small receipt drain independently; deleting this receiver
    // must not delete/abort the socket before the sender reads it.
    QTcpSocket* receiptSocket = socket;
    socket.clear();
    receiptSocket->setParent(nullptr);
    connect(receiptSocket, &QAbstractSocket::disconnected, receiptSocket, &QObject::deleteLater);
    QTimer::singleShot(5000, receiptSocket, [receiptSocket] {
      receiptSocket->abort();
      receiptSocket->deleteLater();
    });
    if(receiptSocket->write(receipt) == receipt.size())
      receiptSocket->disconnectFromHost();
    else {
      receiptSocket->abort();
      receiptSocket->deleteLater();
    }
  }
  emit progressUpdated(FM_Receive, FO_Complete, type, &id, &peerId, &filePath);
}

void FileReceiver::disconnected(void) {
  readyRead();
  if(!terminal())
    fail(QStringLiteral("Peer disconnected before the advertised file was received"));
}

void FileReceiver::readyRead(void) {
  if(state != State::Receiving || !socket)
    return;
  qint64 budget = kReadBudget;
  while(state == State::Receiving && socket->bytesAvailable() > 0 && budget > 0) {
    const qint64 wanted = qMin(qint64(buffer.size()), fileSize - receivedBytes);
    if(wanted <= 0) {
      finish();
      return;
    }
    const qint64 received = socket->read(buffer.data(), wanted);
    if(received < 0) {
      fail(QStringLiteral("File socket read failed"));
      return;
    }
    if(received == 0)
      break;
    qint64 written = 0;
    while(written < received) {
      const qint64 count = file->write(buffer.data() + written, received - written);
      if(count <= 0) {
        fail(QStringLiteral("Destination file write failed"));
        return;
      }
      written += count;
    }
    hash.addData(QByteArrayView(buffer.data(), received));
    receivedBytes += received;
    budget -= received;
    idleTimer.restart();
    if(receivedBytes == fileSize) {
      // Never write beyond the explicitly accepted size.
      if(socket->bytesAvailable() > 0) {
        fail(QStringLiteral("Peer sent more data than the advertised file size"));
        return;
      }
      finish();
      return;
    }
  }
  // Yield between batches so a fast transfer cannot monopolize the GUI
  // event loop. Resume even when Qt has no new readyRead signal to emit.
  if(state == State::Receiving && socket->bytesAvailable() > 0 && !readScheduled) {
    readScheduled = true;
    QTimer::singleShot(0, this, [this] {
      readScheduled = false;
      readyRead();
    });
  }
}

void FileReceiver::timer_timeout(void) {
  if(state != State::Receiving)
    return;
  if(idleTimer.hasExpired(idleTimeoutMs)) {
    fail(QStringLiteral("File receive timed out without progress"));
    return;
  }
  QString transferred = QString::number(receivedBytes);
  emit progressUpdated(FM_Receive, FO_Progress, type, &id, &peerId, &transferred);
}

bool FileReceiver::receiveFile(void) {
  if(fileSize < 0) {
    fail(QStringLiteral("Invalid negative file size"));
    return false;
  }
  QDir dir = QFileInfo(filePath).dir();
  if(!dir.exists() && !dir.mkpath(dir.absolutePath())) {
    fail(QStringLiteral("Destination directory cannot be created"));
    return false;
  }
  file = std::make_unique<QSaveFile>(filePath);
  file->setDirectWriteFallback(false);
  if(!file->open(QIODevice::WriteOnly)) {
    fail(QStringLiteral("Destination file cannot be opened"));
    return false;
  }
  state = State::Receiving;
  hash.reset();
  connect(&timer, &QTimer::timeout, this, &FileReceiver::timer_timeout);
  idleTimer.start();
  timer.start(qMin(PROGRESS_TIMEOUT, qMax(1, idleTimeoutMs / 4)));
  return true;
}


/****************************************************************************
** Class: MsgStream
** Description: Handles transmission and reception of TCP streaming messages.
****************************************************************************/
MsgStream::MsgStream(void) {
  socket = NULL;
  reading = false;
}

MsgStream::MsgStream(QString szLocalId, QString szPeerId, QString szPeerAddress, int nPort) {
  localId = szLocalId;
  peerId = szPeerId;
  peerAddress = szPeerAddress;
  port = nPort;
  socket = NULL;
  reading = false;
  outDataLen = 0;
  inDataLen = 0;
}

MsgStream::~MsgStream(void) {
  if(socket) {
    disconnect(socket, nullptr, this, nullptr);
    socket->abort();
  }
}

void MsgStream::init(void) {
  socket = new QTcpSocket(this);
  socket->setProxy(QNetworkProxy::NoProxy);
  socket->setReadBufferSize(kMessageLimit + 4);
  connect(socket, &QAbstractSocket::connected, this, &MsgStream::connected);
  connect(socket, &QAbstractSocket::disconnected, this, &MsgStream::disconnected);
  connect(socket, &QIODevice::readyRead, this, &MsgStream::readyRead);
  connect(socket, &QIODevice::bytesWritten, this, &MsgStream::bytesWritten);

  QHostAddress hostAddress(peerAddress);
  socket->connectToHost(hostAddress, port);
}

void MsgStream::init(QTcpSocket* socket) {
  this->socket = socket;
  socket->setParent(this);
  socket->setReadBufferSize(kMessageLimit + 4);
  connect(socket, &QAbstractSocket::disconnected, this, &MsgStream::disconnected);
  connect(this->socket, &QIODevice::readyRead, this, &MsgStream::readyRead);
  connect(socket, &QIODevice::bytesWritten, this, &MsgStream::bytesWritten);
}

void MsgStream::stop(void) {
  if(socket && socket->isOpen())
    socket->close();
}

void MsgStream::sendMessage(QByteArray& data) {
  if(!socket)
    return;
  if(data.size() > kMessageLimit || socket->bytesToWrite() + qint64(sizeof(quint32)) + data.size() > 2 * kMessageLimit) {
    lmcTrace::write("Error: Message size or outgoing queue exceeds its memory budget");
    socket->abort();
    return;
  }
  qint32 dataLen = sizeof(quint32) + data.length();
  outDataLen += dataLen;
  outData.resize(dataLen);

  QDataStream stream(&outData, QIODevice::WriteOnly);
  stream.setVersion(QDataStream::Qt_5_15);
  stream << (quint32)data.length();
  stream.writeRawData(data.data(), data.length());

  qint64 numBytesWritten = socket->write(outData);
  if(numBytesWritten < 0)
    lmcTrace::write("Error: Socket write failed");
}

void MsgStream::connected(void) {
  outData = localId.toLocal8Bit();
  outData.insert(0, "MSG"); // insert indicator that this socket handles messages
  outDataLen = outData.length();

  //	send an id message and then wait for public key message
  //	from receiver, which will trigger readyRead signal
  qint64 numBytesWritten = socket->write(outData);
  if(numBytesWritten < 0)
    lmcTrace::write("Error: Socket write failed");
}

void MsgStream::disconnected(void) {
  emit connectionLost(&peerId);
}

void MsgStream::readyRead(void) {
  // TCP may split a four-byte length prefix or combine many complete
  // frames in one readyRead. Do not consume a partial prefix.
  while(socket && socket->bytesAvailable() > 0) {
    if(!reading) {
      if(socket->bytesAvailable() < qint64(sizeof(quint32)))
        return;
      const QByteArray prefix = socket->read(sizeof(quint32));
      QDataStream stream(prefix);
      stream.setVersion(QDataStream::Qt_5_15);
      stream >> inDataLen;
      if(stream.status() != QDataStream::Ok || inDataLen > kMessageLimit) {
        lmcTrace::write("Error: Invalid or oversized TCP message frame");
        inData.clear();
        socket->abort();
        return;
      }
      reading = true;
      inData.clear();
      inData.reserve(int(inDataLen));
    }
    if(inDataLen > 0) {
      const qint64 amount = qMin(qint64(inDataLen), socket->bytesAvailable());
      if(amount <= 0)
        return;
      const QByteArray part = socket->read(amount);
      if(part.isEmpty())
        return;
      inData.append(part);
      inDataLen -= quint32(part.size());
    }
    if(inDataLen == 0) {
      reading = false;
      emit messageReceived(&peerId, &peerAddress, inData);
    }
  }
}

void MsgStream::bytesWritten(qint64 bytes) {
  outDataLen -= bytes;
  if(outDataLen == 0)
    return;

  if(outDataLen > 0)
    lmcTrace::write("Warning: Socket write operation not completed");
  if(outDataLen < 0)
    lmcTrace::write("Warning: Socket write overrun");

  //	TODO: handle situation when entire message is not written to stream in one write operation
  //	The following code is not functional currently, hence commented out.
  /*outData = outData.mid(outDataLen);
	socket->write(outData);*/
}
