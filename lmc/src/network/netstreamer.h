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


#ifndef NETSTREAMER_H
#define NETSTREAMER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QFile>
#include <QSaveFile>
#include <QPointer>
#include <QElapsedTimer>
#include <QCryptographicHash>
#include <array>
#include <memory>
#include "core/shared.h"

/****************************************************************************
** Class: FileSender
** Description: Handles sending files.
****************************************************************************/
class FileSender : public QObject {
  Q_OBJECT

public:
  FileSender(void);
  FileSender(QString szId, QString szLocalId, QString szPeerId, QString szFilePath, QString szFileName, qint64 nFileSize,
             QString szAddress, int nPort, FileType nType, int idleTimeoutMs = 30000);
  ~FileSender(void);

  void init(void);
  void stop(void);

  QString id;
  QString peerId;
  FileType type = FT_None;

signals:
  void progressUpdated(FileMode mode, FileOp fileOp, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData);

private slots:
  void connected(void);
  void disconnected(void);
  void readyRead(void);
  void bytesWritten(qint64 bytes);
  void timer_timeout(void);

private:
  void sendFile(void);
  void pump(void);
  void fail(const QString& reason);
  void complete(void);
  bool terminal(void) const;
  enum class State { Idle, WaitingForStart, Sending, WaitingForReceipt, Complete, Failed, Stopped };

  QString localId;
  QString filePath;
  QString fileName;
  qint64 fileSize = 0;
  QString address;
  int port = 0;
  QPointer<QTcpSocket> socket;
  QFile file;
  std::array<char, 65536> buffer{};
  qint64 bufferLength = 0;
  qint64 bufferOffset = 0;
  qint64 queuedFileBytes = 0;
  QByteArray control;
  QCryptographicHash hash{QCryptographicHash::Sha256};
  State state = State::Idle;
  QTimer timer;
  QElapsedTimer idleTimer;
  int idleTimeoutMs = 30000;

};

/****************************************************************************
** Class: FileReceiver
** Description: Handles receiving files.
****************************************************************************/
class FileReceiver : public QObject {
  Q_OBJECT

public:
  FileReceiver(void);
  FileReceiver(QString szId, QString szPeerId, QString szFilePath, QString szFileName, qint64 nFileSize,
               QString szAddress, int nPort, FileType nType, int idleTimeoutMs = 30000);
  ~FileReceiver(void);

  void init(QTcpSocket* socket);
  void stop(void);

  QString id;
  QString peerId;
  FileType type = FT_None;

signals:
  void progressUpdated(FileMode mode, FileOp fileOp, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData);

private slots:
  void disconnected(void);
  void readyRead(void);
  void timer_timeout(void);

private:
  bool receiveFile(void);
  void finish(void);
  void fail(const QString& reason);
  bool terminal(void) const;
  enum class State { Idle, Receiving, Complete, Failed, Stopped };

  QString filePath;
  QString fileName;
  qint64 fileSize = 0;
  QString address;
  int port = 0;
  QPointer<QTcpSocket> socket;
  std::unique_ptr<QSaveFile> file;
  std::array<char, 65536> buffer{};
  qint64 receivedBytes = 0;
  QCryptographicHash hash{QCryptographicHash::Sha256};
  State state = State::Idle;
  QTimer timer;
  QElapsedTimer idleTimer;
  int idleTimeoutMs = 30000;
  bool readScheduled = false;

};

/****************************************************************************
** Class: MsgStream
** Description: Handles transmission and reception of TCP streaming messages.
****************************************************************************/
class MsgStream : public QObject {
  Q_OBJECT

public:
  MsgStream(void);
  MsgStream(QString szLocalId, QString szPeerId, QString szPeerAddress, int nPort);
  ~MsgStream(void);

  void init(void);
  void init(QTcpSocket* socket);
  void stop(void);
  void sendMessage(QByteArray& data);

signals:
  void connectionLost(QString* lpszUserId);
  void messageReceived(QString* lpszUserId, QString* lpszAddress, QByteArray& data);

private slots:
  void connected(void);
  void disconnected(void);
  void readyRead(void);
  void bytesWritten(qint64 bytes);

private:
  QPointer<QTcpSocket> socket;
  int port;
  QString localId;
  QString peerId;
  QString peerAddress;
  QByteArray outData;
  QByteArray inData;
  qint64 outDataLen = 0;
  quint32 inDataLen = 0;
  bool reading = false;
};

#endif // NETSTREAMER_H
