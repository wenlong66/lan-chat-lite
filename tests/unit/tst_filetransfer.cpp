// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 LanChat Lite contributors.
// Actual loopback TCP transfers, not a simulation of the production pump.
#include <QtTest>
#include <QCryptographicHash>
#include <QPointer>
#include <QTemporaryDir>
#include <QTcpServer>
#include <QTcpSocket>
#include <functional>

#include "network/netstreamer.h"
#include "network/tcpnetwork.h"
#include "messaging/xmlmessage.h"

namespace {
const QString kId(32, QLatin1Char('a'));
const QString kPeer = QStringLiteral("192.0.2.1-source");
const QByteArray kHeader = QByteArrayLiteral("FILE") + kId.toLatin1() + kPeer.toUtf8();

bool writeFile(const QString& path, const QByteArray& bytes) {
  QFile file(path);
  return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size();
}
QByteArray readFile(const QString& path) {
  QFile file(path);
  return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}
QByteArray digest(const QByteArray& body) {
  return QCryptographicHash::hash(body, QCryptographicHash::Sha256).toHex();
}
QByteArray bodyOfSize(int size) {
  QByteArray body(size, Qt::Uninitialized);
  for(int i = 0; i < size; ++i)
    body[i] = char((i * 31 + (i >> 8)) & 0xff);
  return body;
}

class TransferPair : public QObject {
public:
  QTemporaryDir directory;
  QTcpServer server;
  QPointer<FileSender> source;
  QPointer<FileReceiver> destination;
  QPointer<QTcpSocket> receivedSocket;
  int sent = 0, sendErrors = 0, received = 0, receiveErrors = 0;
  bool deleteOnComplete = false;
  QString outputOverride;

  QString input() const { return directory.filePath(QStringLiteral("source.bin")); }
  QString output() const { return outputOverride.isEmpty() ? directory.filePath(QStringLiteral("收到的文件.bin")) : outputOverride; }

  bool start(const QByteArray& body, qint64 offeredSize = -1) {
    if(!directory.isValid() || !writeFile(input(), body) || !server.listen(QHostAddress::LocalHost, 0))
      return false;
    const qint64 size = offeredSize < 0 ? body.size() : offeredSize;
    connect(&server, &QTcpServer::newConnection, this, [this, size] {
      auto* socket = server.nextPendingConnection();
      receivedSocket = socket;
      connect(socket, &QIODevice::readyRead, this, [this, socket, size] {
        if(destination || socket->bytesAvailable() < kHeader.size())
          return;
        if(socket->read(kHeader.size()) != kHeader) {
          socket->abort();
          return;
        }
        destination = new FileReceiver(kId, kPeer, output(), QStringLiteral("file.bin"), size,
                                       QStringLiteral("127.0.0.1"), 0, FT_Normal, 1000);
        destination->setParent(this);
        connect(destination, &FileReceiver::progressUpdated, this,
                [this](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
          if(op == FO_Complete) {
            ++received;
            if(deleteOnComplete)
              destination->deleteLater();
          }
          if(op == FO_Error)
            ++receiveErrors;
        });
        destination->init(socket);
      });
    });
    source = new FileSender(kId, kPeer, QStringLiteral("receiver"), input(), QStringLiteral("file.bin"),
                            size, QStringLiteral("127.0.0.1"), server.serverPort(), FT_Normal, 1500);
    source->setParent(this);
    connect(source, &FileSender::progressUpdated, this,
            [this](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
      if(op == FO_Complete)
        ++sent;
      if(op == FO_Error)
        ++sendErrors;
    });
    source->init();
    return true;
  }
};

// A deliberately independent mock receiver, for framing/receipt failures.
class RawPeer : public QObject {
public:
  QTcpServer server;
  QPointer<QTcpSocket> socket;
  bool routed = false;
  QByteArray received;
  std::function<void()> onRouted;
  std::function<void()> onData;
  bool listen() {
    connect(&server, &QTcpServer::newConnection, this, [this] {
      socket = server.nextPendingConnection();
      connect(socket, &QIODevice::readyRead, this, [this] {
        if(!routed) {
          if(socket->bytesAvailable() < kHeader.size())
            return;
          if(socket->read(kHeader.size()) != kHeader) {
            socket->abort();
            return;
          }
          routed = true;
          if(onRouted)
            onRouted();
        }
        if(socket && onData)
          onData();
      });
    });
    return server.listen(QHostAddress::LocalHost, 0);
  }
};
}

class TstFileTransfer : public QObject {
  Q_OBJECT
private slots:
  void roundTrip_data();
  void roundTrip();
  void fragmentedControlAndReceipt();
  void invalidReceiptNeverCompletes();
  void disconnectBeforeReceiptNeverCompletes();
  void missingStartTimesOut();
  void senderBackpressureIsBounded();
  void cancelledReceivePreservesExistingFile();
  void failedDestinationNeverCompletes();
  void sourceSizeChangeFails();
  void repeatedTransfersReleaseSockets();
  void fragmentedRoutingHeader();
  void excessBodyDoesNotReplaceDestination();
  void messageFramesHandleFragmentsAndCoalescing();
  void oversizedMessageRejected();
};

void TstFileTransfer::roundTrip_data() {
  QTest::addColumn<int>("size");
  QTest::newRow("empty") << 0;
  QTest::newRow("one-byte") << 1;
  QTest::newRow("chunk-minus-one") << 65535;
  QTest::newRow("one-chunk") << 65536;
  QTest::newRow("chunk-plus-one") << 65537;
  QTest::newRow("queue-minus-one") << (512 * 1024 - 1);
  QTest::newRow("one-queue") << (512 * 1024);
  QTest::newRow("queue-plus-one") << (512 * 1024 + 1);
  QTest::newRow("many-chunks") << (3 * 512 * 1024 + 17);
}
void TstFileTransfer::roundTrip() {
  QFETCH(int, size);
  const QByteArray body = bodyOfSize(size);
  TransferPair pair;
  // Production removes the receiver on FO_Complete. This catches receipt
  // loss caused by destroying the socket before its write buffer drains.
  pair.deleteOnComplete = true;
  QVERIFY(pair.start(body));
  QTRY_COMPARE_WITH_TIMEOUT(pair.sent, 1, 5000);
  QCOMPARE(pair.received, 1);
  QCOMPARE(pair.sendErrors, 0);
  QCOMPARE(pair.receiveErrors, 0);
  QVERIFY(QFile::exists(pair.output()));
  QCOMPARE(readFile(pair.output()), body);
  QTRY_VERIFY(pair.destination.isNull());
}

void TstFileTransfer::fragmentedControlAndReceipt() {
  QTemporaryDir dir;
  const QByteArray body = bodyOfSize(256 * 1024 + 3);
  QVERIFY(writeFile(dir.filePath("input"), body));
  RawPeer peer;
  QVERIFY(peer.listen());
  peer.onRouted = [&] {
    peer.socket->write("START"); // ambiguous prefix of a valid fragmented START2
    QTimer::singleShot(5, &peer, [&] { peer.socket->write("2"); });
    QTimer::singleShot(10, &peer, [&] { peer.socket->write("\n"); });
  };
  bool replied = false;
  peer.onData = [&] {
    peer.received += peer.socket->readAll();
    if(peer.received.size() == body.size() && !replied) {
      replied = true;
      peer.socket->write("DONE2 ");
      QTimer::singleShot(5, &peer, [&] { peer.socket->write(digest(peer.received).left(30)); });
      QTimer::singleShot(10, &peer, [&] {
        peer.socket->write(digest(peer.received).mid(30) + '\n');
        peer.socket->disconnectFromHost();
      });
    }
  };
  FileSender source(kId, kPeer, "receiver", dir.filePath("input"), "input", body.size(),
                    "127.0.0.1", peer.server.serverPort(), FT_Normal, 1000);
  int complete = 0, errors = 0;
  connect(&source, &FileSender::progressUpdated, this,
          [&](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
    complete += op == FO_Complete;
    errors += op == FO_Error;
  });
  source.init();
  QTRY_COMPARE_WITH_TIMEOUT(complete, 1, 3000);
  QCOMPARE(errors, 0);
  QCOMPARE(peer.received, body);
}

void TstFileTransfer::invalidReceiptNeverCompletes() {
  QTemporaryDir dir;
  QVERIFY(writeFile(dir.filePath("input"), {}));
  RawPeer peer;
  QVERIFY(peer.listen());
  peer.onRouted = [&] { peer.socket->write("START2\nDONE2 wrong-checksum\n"); };
  FileSender source(kId, kPeer, "receiver", dir.filePath("input"), "input", 0,
                    "127.0.0.1", peer.server.serverPort(), FT_Normal, 1000);
  int complete = 0, errors = 0;
  connect(&source, &FileSender::progressUpdated, this,
          [&](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
    complete += op == FO_Complete;
    errors += op == FO_Error;
  });
  source.init();
  QTRY_COMPARE(errors, 1);
  QCOMPARE(complete, 0);
}

void TstFileTransfer::disconnectBeforeReceiptNeverCompletes() {
  QTemporaryDir dir;
  const QByteArray body = bodyOfSize(128 * 1024);
  QVERIFY(writeFile(dir.filePath("input"), body));
  RawPeer peer;
  QVERIFY(peer.listen());
  peer.onRouted = [&] { peer.socket->write("START2\n"); };
  peer.onData = [&] {
    peer.received += peer.socket->readAll();
    if(peer.received.size() == body.size())
      peer.socket->disconnectFromHost(); // no receipt
  };
  FileSender source(kId, kPeer, "receiver", dir.filePath("input"), "input", body.size(),
                    "127.0.0.1", peer.server.serverPort(), FT_Normal, 1000);
  int complete = 0, errors = 0;
  connect(&source, &FileSender::progressUpdated, this,
          [&](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
    complete += op == FO_Complete;
    errors += op == FO_Error;
  });
  source.init();
  QTRY_COMPARE(errors, 1);
  QCOMPARE(complete, 0);
}

void TstFileTransfer::missingStartTimesOut() {
  QTemporaryDir dir;
  QVERIFY(writeFile(dir.filePath("input"), "x"));
  RawPeer peer;
  QVERIFY(peer.listen());
  peer.onRouted = [&] { peer.socket->write("START"); }; // legacy peer
  FileSender source(kId, kPeer, "receiver", dir.filePath("input"), "input", 1,
                    "127.0.0.1", peer.server.serverPort(), FT_Normal, 100);
  int complete = 0, errors = 0;
  QString errorReason;
  connect(&source, &FileSender::progressUpdated, this,
          [&](FileMode, FileOp op, FileType, QString*, QString*, QString* data) {
    complete += op == FO_Complete;
    if(op == FO_Error) {
      ++errors;
      errorReason = *data;
    }
  });
  source.init();
  QTRY_COMPARE_WITH_TIMEOUT(errors, 1, 1000);
  QCOMPARE(complete, 0);
  QVERIFY(errorReason.contains("START2"));
}

void TstFileTransfer::senderBackpressureIsBounded() {
  QTemporaryDir dir;
  QFile large(dir.filePath("large"));
  QVERIFY(large.open(QIODevice::WriteOnly));
  QVERIFY(large.resize(64LL * 1024 * 1024));
  large.close();
  RawPeer peer;
  QVERIFY(peer.listen());
  peer.onRouted = [&] {
    peer.socket->setReadBufferSize(1);
    peer.socket->write("START2\n");
  };
  // Deliberately never consume the body: OS buffers eventually fill.
  FileSender source(kId, kPeer, "receiver", large.fileName(), "large", large.size(),
                    "127.0.0.1", peer.server.serverPort(), FT_Normal, 2000);
  source.init();
  QTRY_VERIFY(peer.routed);
  auto* socket = source.findChild<QTcpSocket*>();
  QVERIFY(socket);
  QTRY_VERIFY(socket->bytesToWrite() > 0);
  for(int sample = 0; sample < 30; ++sample) {
    QTest::qWait(10);
    QVERIFY2(socket->bytesToWrite() <= 512 * 1024, "userspace send queue exceeded its memory budget");
  }
  source.stop();
  QCOMPARE(socket->bytesToWrite(), 0);
}

void TstFileTransfer::cancelledReceivePreservesExistingFile() {
  QTemporaryDir dir;
  const QString path = dir.filePath("existing");
  QVERIFY(writeFile(path, "keep me"));
  QTcpServer server;
  QVERIFY(server.listen(QHostAddress::LocalHost, 0));
  QTcpSocket client;
  client.connectToHost(QHostAddress::LocalHost, server.serverPort());
  QTRY_VERIFY(server.hasPendingConnections());
  FileReceiver receiver(kId, kPeer, path, "existing", 1024 * 1024, "127.0.0.1", 0, FT_Normal, 500);
  int complete = 0;
  connect(&receiver, &FileReceiver::progressUpdated, this,
          [&](FileMode, FileOp op, FileType, QString*, QString*, QString*) { complete += op == FO_Complete; });
  receiver.init(server.nextPendingConnection());
  QTRY_VERIFY(client.bytesAvailable() >= 7);
  QCOMPARE(client.readAll(), QByteArray("START2\n"));
  client.write(QByteArray(1000, 'x'));
  QTest::qWait(30);
  receiver.stop();
  QCOMPARE(complete, 0);
  QCOMPARE(readFile(path), QByteArray("keep me"));
  QCOMPARE(QDir(dir.path()).entryList(QDir::Files | QDir::Hidden).size(), 1);
}

void TstFileTransfer::failedDestinationNeverCompletes() {
  TransferPair pair;
  pair.outputOverride = pair.directory.path(); // cannot replace a directory
  QVERIFY(pair.start("payload"));
  QTRY_COMPARE(pair.receiveErrors, 1);
  QTRY_COMPARE(pair.sendErrors, 1);
  QCOMPARE(pair.sent, 0);
  QCOMPARE(pair.received, 0);
  QVERIFY(QFileInfo(pair.directory.path()).isDir());
}

void TstFileTransfer::sourceSizeChangeFails() {
  TransferPair pair;
  QVERIFY(pair.start("short", 100));
  QTRY_COMPARE(pair.sendErrors, 1);
  QTRY_COMPARE(pair.receiveErrors, 1);
  QCOMPARE(pair.sent, 0);
  QCOMPARE(pair.received, 0);
  QVERIFY(!QFile::exists(pair.output()));
}

void TstFileTransfer::repeatedTransfersReleaseSockets() {
  for(int i = 0; i < 24; ++i) {
    TransferPair pair;
    pair.deleteOnComplete = true;
    QVERIFY(pair.start(bodyOfSize(i * 73)));
    QTRY_COMPARE_WITH_TIMEOUT(pair.sent, 1, 2000);
    QTRY_VERIFY(pair.destination.isNull());
    QTRY_VERIFY(pair.receivedSocket.isNull());
    const auto senderSocket = QPointer<QTcpSocket>(pair.source->findChild<QTcpSocket*>());
    pair.source->deleteLater();
    QTRY_VERIFY(pair.source.isNull());
    QVERIFY(senderSocket.isNull());
  }
}

void TstFileTransfer::fragmentedRoutingHeader() {
  QTemporaryDir dir;
  const QString path = dir.filePath("received");
  const QByteArray body("fragmented TCP routing");
  // Includes a UTF-8 peer ID and a header longer than the old 64-byte read.
  const QString peerId = QStringLiteral("192.0.2.10-测试用户-") + QString(100, QLatin1Char('x'));
  lmcTcpNetwork network;
  network.init(0);
  network.start();
  QVERIFY(network.serverPort() > 0);
  XmlMessage offer;
  offer.addData(XN_FILEID, kId);
  offer.addData(XN_FILETYPE, FileTypeNames[FT_Normal]);
  offer.addData(XN_FILEPATH, path);
  offer.addData(XN_FILENAME, "received");
  offer.addData(XN_FILESIZE, QString::number(body.size()));
  QString xml = offer.toString();
  QString peer = peerId, address = QStringLiteral("127.0.0.1");
  network.initReceiveFile(&peer, &address, &xml);
  QTcpSocket client;
  client.connectToHost(QHostAddress::LocalHost, network.serverPort());
  QTRY_COMPARE(client.state(), QAbstractSocket::ConnectedState);
  const QByteArray header = QByteArrayLiteral("FILE") + kId.toLatin1() + peerId.toUtf8();
  client.write(header.left(4));
  QTest::qWait(10);
  client.write(header.mid(4, 20));
  QTest::qWait(10);
  client.write(header.mid(24));
  QTRY_VERIFY(client.bytesAvailable() >= 7);
  QCOMPARE(client.readAll(), QByteArray("START2\n"));
  client.write(body);
  QTRY_VERIFY(QFile::exists(path));
  QTRY_VERIFY(client.bytesAvailable() > 0);
  QCOMPARE(client.readAll(), QByteArrayLiteral("DONE2 ") + digest(body) + '\n');
  QCOMPARE(readFile(path), body);
  network.stop();
}

void TstFileTransfer::excessBodyDoesNotReplaceDestination() {
  QTemporaryDir dir;
  const QString path = dir.filePath("existing");
  QVERIFY(writeFile(path, "original"));
  QTcpServer server;
  QVERIFY(server.listen(QHostAddress::LocalHost, 0));
  QTcpSocket client;
  client.connectToHost(QHostAddress::LocalHost, server.serverPort());
  QTRY_VERIFY(server.hasPendingConnections());
  FileReceiver receiver(kId, kPeer, path, "existing", 3, "127.0.0.1", 0, FT_Normal, 500);
  int errors = 0, complete = 0;
  connect(&receiver, &FileReceiver::progressUpdated, this,
          [&](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
    errors += op == FO_Error;
    complete += op == FO_Complete;
  });
  receiver.init(server.nextPendingConnection());
  QTRY_VERIFY(client.bytesAvailable() >= 7);
  client.readAll();
  client.write("1234");
  QTRY_COMPARE(errors, 1);
  QCOMPARE(complete, 0);
  QCOMPARE(readFile(path), QByteArray("original"));
}

namespace {
QByteArray messageFrame(const QByteArray& body) {
  const quint32 length = quint32(body.size());
  QByteArray frame;
  frame.append(char(length >> 24));
  frame.append(char(length >> 16));
  frame.append(char(length >> 8));
  frame.append(char(length));
  frame.append(body);
  return frame;
}
}

void TstFileTransfer::messageFramesHandleFragmentsAndCoalescing() {
  QTcpServer server;
  QVERIFY(server.listen(QHostAddress::LocalHost, 0));
  QTcpSocket client;
  client.connectToHost(QHostAddress::LocalHost, server.serverPort());
  QTRY_VERIFY(server.hasPendingConnections());
  MsgStream stream("local", "remote", "127.0.0.1", 0);
  QList<QByteArray> received;
  connect(&stream, &MsgStream::messageReceived, this,
          [&](QString*, QString*, QByteArray& data) { received.append(data); });
  stream.init(server.nextPendingConnection());
  const QByteArray first = messageFrame(QByteArray("hello\0world", 11));
  const QByteArray second = messageFrame(QByteArray("another message"));
  client.write(first.left(3));
  QTest::qWait(15);
  QVERIFY(received.isEmpty());
  client.write(first.mid(3, 3));
  QTest::qWait(15);
  QVERIFY(received.isEmpty());
  client.write(first.mid(6) + second);
  QTRY_COMPARE(received.size(), 2);
  QCOMPARE(received[0], QByteArray("hello\0world", 11));
  QCOMPARE(received[1], QByteArray("another message"));
}

void TstFileTransfer::oversizedMessageRejected() {
  QTcpServer server;
  QVERIFY(server.listen(QHostAddress::LocalHost, 0));
  QTcpSocket client;
  client.connectToHost(QHostAddress::LocalHost, server.serverPort());
  QTRY_VERIFY(server.hasPendingConnections());
  MsgStream stream("local", "remote", "127.0.0.1", 0);
  int messages = 0;
  connect(&stream, &MsgStream::messageReceived, this,
          [&](QString*, QString*, QByteArray&) { ++messages; });
  stream.init(server.nextPendingConnection());
  // 1 MiB + 1, only the header: reject without allocating/awaiting the body.
  client.write(QByteArray::fromHex("00100001"));
  QTRY_COMPARE(client.state(), QAbstractSocket::UnconnectedState);
  QCOMPARE(messages, 0);
}

QTEST_GUILESS_MAIN(TstFileTransfer)
#include "tst_filetransfer.moc"
