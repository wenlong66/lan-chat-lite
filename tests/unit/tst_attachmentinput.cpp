// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QFileSystemModel>
#include <QListView>
#include <QMimeData>
#include <QTemporaryDir>
#include <QTreeView>
#include <QTimer>
#include <QScopeGuard>
#include <memory>
#include "gui/chat/chatwindow.h"
#include "gui/main/mainwindow.h"

class AttachmentInputTest : public QObject {
  Q_OBJECT

  QTemporaryDir temporary;
  User local, peer;
  QStringList files, folders;

  struct Offer {
    MessageType type;
    QString path;
    QString contentType;
  };

  template<class Window>
  void capture(Window& window, QList<Offer>& offers) {
    connect(&window, &Window::messageSent, &window,
      [&offers](MessageType type, QString*, XmlMessage* message) {
        if(type == MT_File || type == MT_Folder)
          offers.append({type, message->data(XN_FILEPATH), message->data(XN_CONTENTTYPE)});
      });
  }

private slots:
  void initTestCase() {
    QVERIFY(temporary.isValid());
    QStandardPaths::setTestModeEnabled(true);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, temporary.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, temporary.path());
    qputenv("LMC_NON_NATIVE_DIALOGS", "1");
    local.id = "local";
    local.name = "Me";
    peer.id = "peer";
    peer.name = "Other device";
    peer.status = statusCode[0];
    for(const QString& name : {QStringLiteral("first file.txt"), QStringLiteral("中文图片.png")}) {
      const QString path = temporary.filePath(name);
      QFile file(path);
      QVERIFY(file.open(QIODevice::WriteOnly));
      QCOMPARE(file.write("test attachment"), 15);
      files.append(path);
    }
    for(const QString& name : {QStringLiteral("first folder"), QStringLiteral("中文目录")}) {
      const QString path = temporary.filePath(name);
      QVERIFY(QDir().mkpath(path));
      folders.append(path);
    }
  }

  void init() {
    peer.caps = UC_File | UC_Folder;
    lmcSettings settings;
    settings.setValue(IDS_OPENPATH, temporary.path());
    settings.setValue(IDS_HISTORY, false);
    settings.setValue(IDS_SOUND, false);
    settings.setValue(IDS_CLEARONCLOSE, true);
    settings.setValue(IDS_SYSTRAY, false);
  }

  void pickerSendsEverySelection_data() {
    QTest::addColumn<bool>("directories");
    QTest::addColumn<bool>("cancel");
    QTest::addColumn<bool>("contacts");
    QTest::addColumn<bool>("detail");
    for(bool contacts : {false, true})
      for(bool detail : {false, true}) {
        const QString prefix = QString(contacts ? "contacts-" : "chat-") + (detail ? "detail-" : "list-");
        QTest::newRow(qPrintable(prefix + "files")) << false << false << contacts << detail;
        QTest::newRow(qPrintable(prefix + "folders")) << true << false << contacts << detail;
        QTest::newRow(qPrintable(prefix + "cancel-files")) << false << true << contacts << detail;
        QTest::newRow(qPrintable(prefix + "cancel-folders")) << true << true << contacts << detail;
      }
  }

  void pickerSendsEverySelection() {
    QFETCH(bool, directories);
    QFETCH(bool, cancel);
    QFETCH(bool, contacts);
    QFETCH(bool, detail);
    QList<Offer> offers;
    std::unique_ptr<QWidget> window;
    QList<Group> groups{Group("test-group", "Test")};
    if(contacts) {
      auto* main = new lmcMainWindow;
      window.reset(main);
      main->init(&local, &groups, true);
      peer.group = "test-group";
      main->addUser(&peer);
      auto* tree = main->findChild<lmcUserTreeWidget*>("tvUserList");
      QVERIFY(tree);
      const auto items = tree->findItems(peer.name, Qt::MatchExactly | Qt::MatchRecursive);
      QVERIFY(!items.isEmpty());
      tree->setCurrentItem(items.first());
      capture(*main, offers);
    } else {
      auto* chat = new lmcChatWindow;
      window.reset(chat);
      chat->init(&local, &peer, true);
      capture(*chat, offers);
    }
    auto* editor = window->findChild<lmcMessageEdit*>("txtMessage");
    if(editor)
      editor->setPlainText("keep this draft");
    const QStringList expected = directories ? folders : files;
    bool visited = false;
    QTimer::singleShot(0, window.get(), [&] {
      auto* dialog = window->findChild<QFileDialog*>();
      QVERIFY(dialog);
      auto rejectOnFailure = qScopeGuard([&] { dialog->reject(); });
      visited = true;
      if(cancel)
        return;
      dialog->setViewMode(detail ? QFileDialog::Detail : QFileDialog::List);
      auto* view = dialog->findChild<QAbstractItemView*>(detail ? "treeView" : "listView");
      QVERIFY(view);
      QCOMPARE(view->selectionMode(), QAbstractItemView::ExtendedSelection);
      auto* model = qobject_cast<QFileSystemModel*>(view->model());
      QVERIFY(model);
      bool first = true;
      for(const QString& path : expected) {
        QTRY_VERIFY(model->index(path).isValid());
        const QModelIndex index = model->index(path);
        view->scrollTo(index);
        QTRY_VERIFY(!view->visualRect(index).isEmpty());
        QTest::mouseClick(view->viewport(), Qt::LeftButton, first ? Qt::NoModifier : Qt::ControlModifier,
                          view->visualRect(index).center());
        first = false;
      }
      QCOMPARE(dialog->selectedFiles().size(), 2);
      rejectOnFailure.dismiss();
      QVERIFY(QMetaObject::invokeMethod(dialog, "accept"));
    });
    const char* slot = contacts ? (directories ? "userFolderAction_triggered" : "userFileAction_triggered")
                               : (directories ? "btnFolder_clicked" : "btnFile_clicked");
    QVERIFY(QMetaObject::invokeMethod(window.get(), slot));
    QVERIFY(visited);
    if(editor)
      QCOMPARE(editor->toPlainText(), QString("keep this draft"));
    QCOMPARE(lmcSettings().value(IDS_OPENPATH).toString(), temporary.path());
    QCOMPARE(offers.size(), cancel ? 0 : 2);
    QStringList actual;
    for(const Offer& offer : offers) {
      QCOMPARE(offer.type, directories ? MT_Folder : MT_File);
      QVERIFY(offer.contentType.isEmpty());
      actual.append(offer.path);
    }
    if(!cancel) {
      QStringList sortedExpected = expected;
      sortedExpected.sort();
      actual.sort();
      QCOMPARE(actual, sortedExpected);
    }
  }

  void dropSendsSupportedLocalAttachments_data() {
    QTest::addColumn<QString>("surface");
    QTest::addColumn<int>("capabilities");
    QTest::addColumn<bool>("connected");
    QTest::addColumn<int>("expectedFiles");
    QTest::addColumn<int>("expectedFolders");
    for(const QString& surface : {QString("window"), QString("editor"), QString("log")}) {
      QTest::newRow(qPrintable(surface + "-mixed")) << surface << int(UC_File | UC_Folder) << true << 2 << 2;
      QTest::newRow(qPrintable(surface + "-files-only")) << surface << int(UC_File) << true << 2 << 0;
      QTest::newRow(qPrintable(surface + "-folders-only")) << surface << int(UC_Folder) << true << 0 << 2;
      QTest::newRow(qPrintable(surface + "-unsupported")) << surface << 0 << true << 0 << 0;
      QTest::newRow(qPrintable(surface + "-disconnected")) << surface << int(UC_File | UC_Folder) << false << 0 << 0;
    }
  }

  void dropSendsSupportedLocalAttachments() {
    QFETCH(QString, surface);
    QFETCH(int, capabilities);
    QFETCH(bool, connected);
    QFETCH(int, expectedFiles);
    QFETCH(int, expectedFolders);
    peer.caps = capabilities;
    lmcChatWindow chat;
    chat.init(&local, &peer, connected);
    chat.show();
    auto* editor = chat.findChild<lmcMessageEdit*>("txtMessage");
    auto* log = chat.findChild<lmcMessageLog*>();
    QVERIFY(editor);
    QVERIFY(log);
    editor->setPlainText("unsent draft");
    QList<Offer> offers;
    capture(chat, offers);
    QMimeData mime;
    QList<QUrl> urls = {QUrl("https://example.com/file.txt"),
                       QUrl::fromLocalFile(temporary.filePath("missing.txt"))};
    for(const QString& path : files + folders)
      urls.append(QUrl::fromLocalFile(path));
    mime.setUrls(urls);
    QWidget* target = &chat;
    if(surface == "editor")
      target = editor->viewport();
    else if(surface == "log")
      target = log->viewport();
    const QPoint pos = target->rect().center();
    QDragEnterEvent enter(pos, Qt::CopyAction | Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(target, &enter);
    const bool accepted = expectedFiles + expectedFolders > 0;
    QCOMPARE(enter.isAccepted(), accepted);
    if(accepted) {
      QDragMoveEvent move(pos, Qt::CopyAction | Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
      QApplication::sendEvent(target, &move);
      QVERIFY(move.isAccepted());
      QCOMPARE(move.dropAction(), Qt::CopyAction);
      QDropEvent drop(pos, Qt::CopyAction | Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
      QApplication::sendEvent(target, &drop);
      QVERIFY(drop.isAccepted());
      QCOMPARE(drop.dropAction(), Qt::CopyAction);
    }
    QCOMPARE(editor->toPlainText(), QString("unsent draft"));
    QCOMPARE(offers.size(), expectedFiles + expectedFolders);
    QStringList actual;
    for(const Offer& offer : offers) {
      QCOMPARE(offer.type, files.contains(offer.path) ? MT_File : MT_Folder);
      QVERIFY(offer.contentType.isEmpty());
      actual.append(offer.path);
    }
    QStringList expected;
    if(expectedFiles)
      expected.append(files);
    if(expectedFolders)
      expected.append(folders);
    QCOMPARE(actual, expected);
    for(const QString& path : files + folders)
      QVERIFY(QFileInfo::exists(path));
  }

  void textAndLinkDropsKeepNormalEditing_data() {
    QTest::addColumn<bool>("link");
    QTest::newRow("text") << false;
    QTest::newRow("web-link") << true;
  }

  void textAndLinkDropsKeepNormalEditing() {
    QFETCH(bool, link);
    lmcChatWindow chat;
    chat.init(&local, &peer, true);
    chat.show();
    auto* editor = chat.findChild<lmcMessageEdit*>("txtMessage");
    QVERIFY(editor);
    QList<Offer> offers;
    capture(chat, offers);
    QMimeData mime;
    mime.setText(link ? "https://example.com/file.txt" : "dragged text");
    if(link)
      mime.setUrls({QUrl(mime.text())});
    const QPoint pos = editor->viewport()->rect().center();
    QDragEnterEvent enter(pos, Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(editor->viewport(), &enter);
    QVERIFY(enter.isAccepted());
    QDropEvent drop(pos, Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(editor->viewport(), &drop);
    QVERIFY(drop.isAccepted());
    QCOMPARE(editor->toPlainText(), mime.text());
    QVERIFY(offers.isEmpty());
  }
};

QTEST_MAIN(AttachmentInputTest)
#include "tst_attachmentinput.moc"
