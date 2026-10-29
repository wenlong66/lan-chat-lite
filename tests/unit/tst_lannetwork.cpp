// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QNetworkProxy>
#include <QSemaphore>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <atomic>
#include <memory>

#include "network/udpnetwork.h"
#include "network/tcpnetwork.h"

namespace {
// A local SOCKS5 endpoint that drops its control connection after UDP
// association, as happens when a network/proxy connection disappears.
class DisconnectingProxy : public QThread {
public:
  QSemaphore ready;
  quint16 port = 0;
  std::atomic<int> associations{0};

  ~DisconnectingProxy() override {
    requestInterruption();
    wait();
  }

private:
  static QByteArray readBytes(QTcpSocket& socket, qsizetype size) {
    QByteArray data;
    while(data.size() < size) {
      if(socket.bytesAvailable() == 0 && !socket.waitForReadyRead(1000))
        break;
      data += socket.read(size - data.size());
    }
    return data;
  }

  void run() override {
    QTcpServer server;
    server.setProxy(QNetworkProxy::NoProxy);
    QUdpSocket relay;
    relay.setProxy(QNetworkProxy::NoProxy);
    if(server.listen(QHostAddress::LocalHost, 0) && relay.bind(QHostAddress::LocalHost, 0))
      port = server.serverPort();
    ready.release();
    if(port == 0)
      return;
    while(!isInterruptionRequested()) {
      if(!server.waitForNewConnection(50))
        continue;
      std::unique_ptr<QTcpSocket> socket(server.nextPendingConnection());
      const QByteArray greeting = readBytes(*socket, 2);
      if(greeting.size() != 2 || greeting[0] != '\x05')
        continue;
      if(!readBytes(*socket, quint8(greeting[1])).contains('\0'))
        continue;
      socket->write(QByteArray::fromHex("0500"));
      socket->waitForBytesWritten(1000);
      const QByteArray request = readBytes(*socket, 10);
      if(request.size() != 10 || quint8(request[1]) != 3)
        continue;
      QByteArray reply = QByteArray::fromHex("050000017f000001");
      reply += char(relay.localPort() >> 8);
      reply += char(relay.localPort() & 0xff);
      socket->write(reply);
      socket->waitForBytesWritten(1000);
      ++associations;
      msleep(20);
      socket->disconnectFromHost();
      if(socket->state() != QAbstractSocket::UnconnectedState)
        socket->waitForDisconnected(1000);
    }
  }
};
}

class TstLanNetwork : public QObject {
  Q_OBJECT
  QNetworkProxy previousProxy;

private slots:
  void init() {
    previousProxy = QNetworkProxy::applicationProxy();
    QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,
                                                    QStringLiteral("127.0.0.1"), 1));
  }

  void cleanup() {
    QNetworkProxy::setApplicationProxy(previousProxy);
  }

  void udpSurvivesProxyDisconnect() {
    DisconnectingProxy proxy;
    proxy.start();
    QVERIFY(proxy.ready.tryAcquire(1, 3000));
    QVERIFY(proxy.port != 0);
    QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,
                                                    QStringLiteral("127.0.0.1"), proxy.port));
    lmcUdpNetwork network;
    network.init(0);
    QString received;
    connect(&network, &lmcUdpNetwork::broadcastReceived, this,
            [&](DatagramHeader* header, QString* data) {
      received = *data;
      delete header;
    });
    QUdpSocket peer;
    peer.setProxy(QNetworkProxy::NoProxy);
    for(int cycle = 0; cycle < 5; ++cycle) {
      network.start();
      // The pre-fix Qt SOCKS5 UDP engine crashes here when the proxy drops.
      QTest::qWait(150);
      QVERIFY(network.canReceive);
      const auto sockets = network.findChildren<QUdpSocket*>();
      QCOMPARE(sockets.size(), 2);
      quint16 receiverPort = 0;
      for(QUdpSocket* socket : sockets) {
        QCOMPARE(socket->proxy().type(), QNetworkProxy::NoProxy);
        if(socket->state() == QAbstractSocket::BoundState)
          receiverPort = socket->localPort();
      }
      QVERIFY(receiverPort != 0);
      received.clear();
      const QByteArray data = QByteArrayLiteral("network-recovered-") + QByteArray::number(cycle);
      QCOMPARE(peer.writeDatagram(data, QHostAddress::LocalHost, receiverPort), data.size());
      QTRY_COMPARE_WITH_TIMEOUT(received.toUtf8(), data, 1000);
      network.stop();
    }
    QCOMPARE(proxy.associations.load(), 0);
  }

  void tcpListenerIgnoresProxy() {
    lmcTcpNetwork network;
    network.init(0);
    auto* server = network.findChild<QTcpServer*>();
    QVERIFY(server);
    QCOMPARE(server->proxy().type(), QNetworkProxy::NoProxy);
    for(int cycle = 0; cycle < 5; ++cycle) {
      network.start();
      QVERIFY(network.serverPort() != 0);
      QTcpSocket peer;
      peer.setProxy(QNetworkProxy::NoProxy);
      peer.connectToHost(QHostAddress::LocalHost, network.serverPort());
      QTRY_COMPARE_WITH_TIMEOUT(peer.state(), QAbstractSocket::ConnectedState, 1000);
      QTRY_VERIFY_WITH_TIMEOUT(!server->findChildren<QTcpSocket*>().isEmpty(), 1000);
      network.stop();
      QTRY_COMPARE_WITH_TIMEOUT(peer.state(), QAbstractSocket::UnconnectedState, 1000);
      QCOMPARE(network.serverPort(), 0);
    }
  }

  void messageConnectionIgnoresProxy() {
    QTcpServer peer;
    peer.setProxy(QNetworkProxy::NoProxy);
    QVERIFY(peer.listen(QHostAddress::LocalHost, 0));
    MsgStream stream("local", "peer", "127.0.0.1", peer.serverPort());
    stream.init();
    auto* socket = stream.findChild<QTcpSocket*>();
    QVERIFY(socket);
    QCOMPARE(socket->proxy().type(), QNetworkProxy::NoProxy);
    QTRY_VERIFY_WITH_TIMEOUT(peer.hasPendingConnections(), 1000);
    std::unique_ptr<QTcpSocket> incoming(peer.nextPendingConnection());
    QTRY_COMPARE_WITH_TIMEOUT(incoming->bytesAvailable(), 8, 1000);
    QCOMPARE(incoming->readAll(), QByteArrayLiteral("MSGlocal"));
  }

  void fileConnectionIgnoresProxy() {
    QTcpServer peer;
    peer.setProxy(QNetworkProxy::NoProxy);
    QVERIFY(peer.listen(QHostAddress::LocalHost, 0));
    QTemporaryFile file;
    QVERIFY(file.open());
    file.close();
    const QString id(32, QLatin1Char('a'));
    FileSender sender(id, "local", "peer", file.fileName(), "empty.bin", 0,
                      "127.0.0.1", peer.serverPort(), FT_Normal);
    sender.init();
    auto* socket = sender.findChild<QTcpSocket*>();
    QVERIFY(socket);
    QCOMPARE(socket->proxy().type(), QNetworkProxy::NoProxy);
    QTRY_VERIFY_WITH_TIMEOUT(peer.hasPendingConnections(), 1000);
    std::unique_ptr<QTcpSocket> incoming(peer.nextPendingConnection());
    const QByteArray header = QByteArrayLiteral("FILE") + id.toLatin1() + "local";
    QTRY_COMPARE_WITH_TIMEOUT(incoming->bytesAvailable(), header.size(), 1000);
    QCOMPARE(incoming->readAll(), header);
  }
};

int main(int argc, char** argv) {
  QTemporaryDir settings;
  if(!settings.isValid())
    return 1;
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settings.path());
  QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settings.path());
  QCoreApplication application(argc, argv);
  QCoreApplication::setOrganizationName("LanChatLiteTests");
  QCoreApplication::setApplicationName("lannetwork");
  TstLanNetwork test;
  return QTest::qExec(&test, argc, argv);
}

#include "tst_lannetwork.moc"
