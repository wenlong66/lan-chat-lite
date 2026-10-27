// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QTextBlock>
#include <QTemporaryDir>
#include <QDesktopServices>
#include <QTcpServer>
#include <QPainter>
#include <QMessageBox>
#include <QTimer>
#include <QScrollBar>
#include "network/netstreamer.h"
#include "gui/chat/chatwindow.h"
#include "gui/widgets/chatimage.h"
#include "gui/chat/messagelog.h"
#include "gui/widgets/clipboardimage.h"

class ChatImageTest : public QObject {
  Q_OBJECT
private:
  QTemporaryDir temporary;
  QUrl openedUrl;

  void initLog(lmcMessageLog& log) {
    log.localId = "local";
    log.peerId = "peer";
    log.autoFile = false;
    log.initMessageLog(":/themes/Classic");
    log.resize(420, 400);
  }

  XmlMessage request(const QString& path, FileMode mode = FM_Receive, bool image = true) {
    XmlMessage message;
    message.addData(XN_FILEID, "image-1");
    message.addData(XN_FILETYPE, FileTypeNames[FT_Normal]);
    message.addData(XN_MODE, FileModeNames[mode]);
    message.addData(XN_FILEOP, FileOpNames[FO_Request]);
    message.addData(XN_FILEPATH, path);
    message.addData(XN_FILENAME, QFileInfo(path).fileName());
    message.addData(XN_FILESIZE, QString::number(QFileInfo(path).size()));
    if(image)
      message.addData("contenttype", "image/png");
    return message;
  }

  void append(lmcMessageLog& log, XmlMessage& message) {
    QString peer = "peer";
    QString name = "Other device";
    log.appendMessageLog(MT_File, &peer, &name, &message);
  }

  QList<QTextCharFormat> images(lmcMessageLog& log) {
    QList<QTextCharFormat> result;
    for(QTextBlock block = log.document()->begin(); block.isValid(); block = block.next()) {
      for(auto it = block.begin(); !it.atEnd(); ++it) {
        const auto format = it.fragment().charFormat();
        if(format.objectType() >= QTextFormat::UserObject)
          result.append(format);
      }
    }
    return result;
  }

  QString png() {
    QImage image(800, 400, QImage::Format_ARGB32);
    image.fill(QColor(30, 140, 210));
    return ClipboardImage::savePng(image, temporary.path());
  }

private slots:
  void initTestCase() {
    QVERIFY(temporary.isValid());
    QStandardPaths::setTestModeEnabled(true);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, temporary.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, temporary.path());
    lmcSettings settings;
    settings.setValue(IDS_FILESTORAGEPATH, temporary.filePath("received"));
    settings.setValue(IDS_CLEARONCLOSE, true);
    settings.setValue(IDS_HISTORY, false);
    settings.setValue(IDS_SOUND, false);
  }

  void captureUrl(const QUrl& url) { openedUrl = url; }

  void receivesImageAutomaticallyButWaitsForCompletion() {
    lmcMessageLog log;
    initLog(log);
    const QString path = png();
    QVERIFY(!path.isEmpty());
    auto message = request(path);
    int accepted = 0;
    connect(&log, &lmcMessageLog::messageSent, this,
      [&](MessageType type, QString*, XmlMessage* reply) {
        QCOMPARE(type, MT_File);
        QCOMPARE(reply->data(XN_FILEID), QStringLiteral("image-1"));
        QCOMPARE(reply->data(XN_FILEOP), FileOpNames[FO_Accept]);
        ++accepted;
      });
    append(log, message);
    QVERIFY(images(log).isEmpty());
    QTRY_COMPARE(accepted, 1);
    log.updateFileMessage(FM_Receive, FO_Complete, "image-1", path);
    QCOMPARE(images(log).size(), 1);
    QCOMPARE(QUrl(images(log).first().anchorHref()).toLocalFile(), path);
    QDesktopServices::setUrlHandler("file", this, "captureUrl");
    const QUrl url(images(log).first().anchorHref());
    QVERIFY(QMetaObject::invokeMethod(&log, "onAnchorClicked", Q_ARG(QUrl, url)));
    QDesktopServices::unsetUrlHandler("file");
    QCOMPARE(openedUrl, url);
    log.reloadMessageLog();
    QCOMPARE(images(log).size(), 1);
    QCoreApplication::processEvents();
    QCOMPARE(accepted, 1);
    QVERIFY(!log.toPlainText().contains("Accept"));
  }

  void senderSeesImageAndCompletedHistoryRestoresIt() {
    lmcMessageLog log;
    initLog(log);
    auto message = request(png(), FM_Send);
    append(log, message);
    QCOMPARE(images(log).size(), 1);
    log.updateFileMessage(FM_Send, FO_Complete, "image-1");
    const QString history = temporary.filePath("chat.log");
    log.saveMessageLog(history);
    lmcMessageLog restored;
    initLog(restored);
    restored.restoreMessageLog(history);
    QCOMPARE(images(restored).size(), 1);
    QVERIFY(restored.toPlainText().contains("Completed"));
    QVERIFY(!restored.toPlainText().contains("Cancel"));
    QVERIFY(log.prepareMessageLogForSave().contains(QUrl::fromLocalFile(message.data(XN_FILEPATH))
                                                    .toString(QUrl::FullyEncoded)));
  }

  void ordinaryPngRemainsAFile() {
    lmcMessageLog log;
    initLog(log);
    auto message = request(png(), FM_Receive, false);
    int replies = 0;
    connect(&log, &lmcMessageLog::messageSent, this, [&] { ++replies; });
    append(log, message);
    QCoreApplication::processEvents();
    QCOMPARE(replies, 0);
    QVERIFY(log.toPlainText().contains("Accept"));
    log.updateFileMessage(FM_Receive, FO_Complete, "image-1");
    QVERIFY(images(log).isEmpty());
  }

  void failedReceiveNeverDisplaysImage() {
    lmcMessageLog log;
    initLog(log);
    auto message = request(png());
    append(log, message);
    log.updateFileMessage(FM_Receive, FO_Error, "image-1");
    QCoreApplication::processEvents();
    QVERIFY(images(log).isEmpty());
    log.reloadMessageLog();
    QVERIFY(images(log).isEmpty());
    QVERIFY(log.toPlainText().contains("Interrupted"));
  }

  void oversizedOfferNeedsManualAcceptance() {
    lmcMessageLog log;
    initLog(log);
    auto message = request(png());
    message.removeData(XN_FILESIZE);
    message.addData(XN_FILESIZE, QString::number(1024LL * 1024 * 1024));
    int replies = 0;
    connect(&log, &lmcMessageLog::messageSent, this, [&] { ++replies; });
    append(log, message);
    QCoreApplication::processEvents();
    QCOMPARE(replies, 0);
    QVERIFY(log.toPlainText().contains("Accept"));
    QVERIFY(images(log).isEmpty());
  }

  void corruptMissingAndOversizedImagesHaveFallbacks() {
    const QString path = temporary.filePath("bad.png");
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("not a PNG");
    file.close();
    QVERIFY(ChatImage::thumbnail(path).isNull());
    QImage oversized(4096, 4097, QImage::Format_Mono);
    oversized.fill(0);
    const QString hugePath = temporary.filePath("huge.png");
    QVERIFY(oversized.save(hugePath));
    QVERIFY(ChatImage::thumbnail(hugePath).isNull());
    lmcMessageLog log;
    initLog(log);
    auto message = request(path);
    append(log, message);
    log.updateFileMessage(FM_Receive, FO_Complete, "image-1", path);
    QVERIFY(images(log).isEmpty());
    QVERIFY(log.toPlainText().contains("Image unavailable"));
    const QString validPath = png();
    QVERIFY(!ChatImage::thumbnail(validPath).isNull());
    QVERIFY(QFile::remove(validPath));
    QVERIFY(ChatImage::thumbnail(validPath).isNull());
  }

  void clipboardSendKeepsOriginalAfterCompletion() {
    User local, peer;
    local.id = "local";
    local.name = "Me";
    peer.id = "peer";
    peer.name = "Other device";
    peer.caps = UC_File;
    lmcChatWindow chat;
    chat.init(&local, &peer, true);
    XmlMessage offered;
    connect(&chat, &lmcChatWindow::messageSent, this,
      [&](MessageType type, QString*, XmlMessage* message) {
        if(type != MT_File || message->data(XN_FILEOP) != FileOpNames[FO_Request])
          return;
        offered = message->clone();
        const QFileInfo file(offered.data(XN_FILEPATH));
        offered.addData(XN_FILEID, "clipboard-send");
        offered.addData(XN_MODE, FileModeNames[FM_Send]);
        offered.addData(XN_FILENAME, file.fileName());
        offered.addData(XN_FILESIZE, QString::number(file.size()));
        chat.receiveMessage(type, nullptr, &offered);
      });
    QTimer::singleShot(0, &chat, [] {
      if(auto* dialog = qobject_cast<QMessageBox*>(QApplication::activeModalWidget()))
        dialog->done(QMessageBox::Ok);
    });
    QImage original(64, 32, QImage::Format_ARGB32);
    original.fill(Qt::green);
    QVERIFY(QMetaObject::invokeMethod(&chat, "pasteImage", Q_ARG(QImage, original)));
    QVERIFY(ChatImage::isImageMessage(&offered));
    const QString staged = offered.data(XN_FILEPATH);
    QVERIFY(QFile::exists(staged));
    auto* log = chat.findChild<lmcMessageLog*>();
    QVERIFY(log);
    QCOMPARE(images(*log).size(), 1);
    XmlMessage complete;
    complete.addData(XN_MODE, FileModeNames[FM_Send]);
    complete.addData(XN_FILEOP, FileOpNames[FO_Complete]);
    complete.addData(XN_FILEID, offered.data(XN_FILEID));
    complete.addData(XN_FILEPATH, staged);
    chat.receiveMessage(MT_File, &peer.id, &complete);
    QCOMPARE(images(*log).size(), 1);
    const QString saved = QUrl(images(*log).first().anchorHref()).toLocalFile();
    QVERIFY(saved != staged);
    QVERIFY(!QFile::exists(staged));
    QCOMPARE(QImage(saved), original);
    log->reloadMessageLog();
    QCOMPARE(images(*log).size(), 1);
    chat.stop();
  }

  void realTcpPngCompletionRendersThumbnail() {
    lmcMessageLog log;
    initLog(log);
    const QString input = png();
    const QString output = temporary.filePath(QString::fromUtf8("收到的 截图 #1.png"));
    auto message = request(input);
    const QString id(32, 'a');
    message.removeData(XN_FILEID);
    message.addData(XN_FILEID, id);
    const qint64 bytes = QFileInfo(input).size();
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    FileReceiver receiver(id, "source", output, "image.png", bytes, "127.0.0.1", 0, FT_Normal);
    FileSender sender(id, "source", "receiver", input, "image.png", bytes,
                      "127.0.0.1", server.serverPort(), FT_Normal);
    const QByteArray header = "FILE" + id.toLatin1() + "source";
    connect(&server, &QTcpServer::newConnection, this, [&] {
      auto* socket = server.nextPendingConnection();
      connect(socket, &QIODevice::readyRead, &receiver, [&, socket] {
        if(socket->property("routed").toBool() || socket->bytesAvailable() < header.size())
          return;
        QCOMPARE(socket->read(header.size()), header);
        socket->setProperty("routed", true);
        receiver.init(socket);
      });
    });
    bool sent = false;
    connect(&sender, &FileSender::progressUpdated, this,
      [&](FileMode, FileOp op, FileType, QString*, QString*, QString*) {
        QVERIFY(op != FO_Error);
        if(op == FO_Complete)
          sent = true;
      });
    connect(&receiver, &FileReceiver::progressUpdated, this,
      [&](FileMode, FileOp op, FileType, QString*, QString*, QString* path) {
        QVERIFY(op != FO_Error);
        if(op == FO_Complete) {
          QVERIFY(QFile::exists(*path));
          log.updateFileMessage(FM_Receive, op, id, *path);
        }
      });
    connect(&log, &lmcMessageLog::messageSent, this,
      [&](MessageType, QString*, XmlMessage* reply) {
        QCOMPARE(reply->data(XN_FILEOP), FileOpNames[FO_Accept]);
        sender.init();
      });
    append(log, message);
    QVERIFY(images(log).isEmpty());
    QTRY_VERIFY_WITH_TIMEOUT(sent, 5000);
    QCOMPARE(QImage(input), QImage(output));
    QCOMPARE(images(log).size(), 1);
    QCOMPARE(QUrl(images(log).first().anchorHref()).toLocalFile(), output);
    log.show();
    QTest::qWait(50);
    const QImage painted = log.grab().toImage();
    int imagePixels = 0;
    for(int y = 0; y < painted.height(); ++y)
      for(int x = 0; x < painted.width(); ++x)
        if(painted.pixelColor(x, y) == QColor(30, 140, 210))
          ++imagePixels;
    QVERIFY2(imagePixels > 1000, "The image object must actually paint the received pixels");
    const QString screenshot = qEnvironmentVariable("LMC_CHATIMAGE_SCREENSHOT");
    if(!screenshot.isEmpty())
      QVERIFY(painted.save(screenshot));
    log.resize(240, 320);
    QCoreApplication::processEvents();
    QVERIFY(log.horizontalScrollBar()->maximum() == 0);
  }
};

QTEST_MAIN(ChatImageTest)
#include "tst_chatimage.moc"
