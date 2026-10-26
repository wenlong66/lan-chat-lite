// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QFileInfo>
#include <QTemporaryDir>
#include "core/settings.h"
#include "core/stdlocation.h"
#include "gui/chat/broadcastwindow.h"
#include "gui/contacts/userselectdialog.h"
#include "gui/main/mainwindow.h"
#include "messaging/messaging.h"

class TstContactGroups : public QObject {
  Q_OBJECT

  static User contact(const QString& id, const QString& name,
                      const QString& status = statusCode[0]) {
    return User(id, IDA_VERSION, QStringLiteral("127.0.0.1"), name, status,
                QStringLiteral("stored-group"), 0, QStringLiteral("note"), QString(),
                QString::number(UC_File | UC_GroupMessage | UC_Folder));
  }

  static void storeGroups(QSettings& settings) {
    settings.setValue("Groups/size", 1);
    settings.setValue("Groups/1/Group", "stored-group");
    settings.setValue("Groups/1/GroupName", "Work");
    settings.setValue("GroupMap/size", 1);
    settings.setValue("GroupMap/1/User", "alice");
    settings.setValue("GroupMap/1/Group", "stored-group");
    settings.setValue("GroupExp/size", 1);
    settings.setValue("GroupExp/1/Group", false);
    settings.sync();
  }

  static QByteArray contents(const QString& path) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
      QTest::qFail(qPrintable(file.errorString()), __FILE__, __LINE__);
      return QByteArray();
    }
    return file.readAll();
  }

private slots:
  void init() {
    lmcSettings settings;
    settings.clear();
    settings.setValue(IDS_SOUND, false);
    settings.setValue(IDS_ALERT, false);
    settings.setValue(IDS_SYSTRAY, false);
    settings.setValue(IDS_SYSTRAYMSG, false);
    settings.sync();
    if(QFile::exists(StdLocation::groupFile()))
      QVERIFY(QFile::remove(StdLocation::groupFile()));
  }

  void migrationPreservesStoredData_data() {
    QTest::addColumn<QString>("version");
    QTest::newRow("lite") << QString(IDA_VERSION);
    QTest::newRow("upstream-old") << QStringLiteral("1.2.10");
    QTest::newRow("upstream-current") << QStringLiteral("1.3.9");
    QTest::newRow("unversioned") << QString();
  }

  void migrationPreservesStoredData() {
    QFETCH(QString, version);
    lmcSettings settings;
    if(!version.isNull())
      settings.setValue(IDS_VERSION, version);
    storeGroups(settings);
    QSettings groups(StdLocation::groupFile(), QSettings::IniFormat);
    storeGroups(groups);
    const QByteArray preferencesBefore = contents(settings.fileName());
    const QByteArray groupsBefore = contents(groups.fileName());
    QVERIFY(!preferencesBefore.isEmpty());
    QVERIFY(!groupsBefore.isEmpty());

    QVERIFY(settings.migrateSettings());
    QVERIFY(settings.migrateSettings());
    settings.sync();
    QCOMPARE(contents(settings.fileName()), preferencesBefore);
    QCOMPARE(contents(groups.fileName()), groupsBefore);
  }

  void groupPersistenceIsDisabled() {
    lmcMessaging messaging;
    messaging.saveGroups();
    QVERIFY(!QFile::exists(StdLocation::groupFile()));
    QSettings groups(StdLocation::groupFile(), QSettings::IniFormat);
    storeGroups(groups);
    const QByteArray before = contents(groups.fileName());

    messaging.loadGroups();
    QVERIFY(messaging.groupList.isEmpty());
    messaging.updateGroup(GO_New, QStringLiteral("new-group"), QStringLiteral("New"));
    QVERIFY(messaging.groupList.isEmpty());
    QVERIFY(messaging.addUser("alice", IDA_VERSION, "127.0.0.1", "Alice", statusCode[0],
                              "0", "note", QString::number(UC_File)));
    QVERIFY(messaging.userList.first().group.isEmpty());
    messaging.updateGroupMap("stored-group", "new-group");
    QString userId = QStringLiteral("alice");
    XmlMessage change;
    change.addData(XN_GROUP, QStringLiteral("new-group"));
    messaging.sendMessage(MT_Group, &userId, &change);
    QVERIFY(messaging.userList.first().group.isEmpty());
    QVERIFY(messaging.userGroupMap.isEmpty());
    messaging.saveGroups();
    QCOMPARE(contents(groups.fileName()), before);
  }

  void contactsStayFlatAndRemainUsable() {
    lmcSettings settings;
    storeGroups(settings);
    User local = contact("local", "Local");
    QList<Group> groups{Group("stored-group", "Work"), Group(GRP_DEFAULT_ID, GRP_DEFAULT)};
    lmcMainWindow window;
    window.init(&local, &groups, false);
    auto* tree = window.findChild<lmcUserTreeWidget*>("tvUserList");
    QVERIFY(tree);
    QCOMPARE(tree->topLevelItemCount(), 0);
    QCOMPARE(tree->dragDropMode(), QAbstractItemView::NoDragDrop);
    QVERIFY(!tree->dragEnabled());
    QVERIFY(!tree->rootIsDecorated());
    User bob = contact("bob", "Bob", statusCode[1]);
    User alice = contact("alice", "Alice");
    window.addUser(&bob);
    window.addUser(&alice);
    QCOMPARE(tree->topLevelItemCount(), 2);
    QCOMPARE(tree->topLevelItem(0)->data(0, IdRole).toString(), alice.id);
    QVERIFY(!tree->topLevelItem(0)->parent());
    tree->resize(300, 300);
    tree->doItemsLayout();
    QTest::mouseClick(tree->viewport(), Qt::LeftButton, Qt::NoModifier,
                      tree->visualItemRect(tree->topLevelItem(0)).center());
    User updated = contact("alice", "Zoe", statusCode[1]);
    window.updateUser(&updated);
    QCOMPARE(tree->topLevelItem(1)->text(0), updated.name);
    settings.setValue(IDS_USERLISTVIEW, ULV_Compact);
    settings.setValue(IDS_STATUSTOOLTIP, true);
    window.settingsChanged();
    QCOMPARE(tree->topLevelItem(0)->sizeHint(0).height(), itemViewHeight[ULV_Compact]);
    QCOMPARE(tree->topLevelItem(0)->toolTip(0), lmcStrings::statusDesc()[1]);
    window.removeUser(&alice.id);
    QCOMPARE(tree->topLevelItemCount(), 1);
    window.stop();
    settings.sync();
    QCOMPARE(settings.value("GroupExp/size").toInt(), 1);
    QCOMPARE(settings.value("GroupExp/1/Group").toBool(), false);
  }

  void groupMenusAreDisabled() {
    User local = contact("local", "Local");
    QList<Group> groups;
    lmcMainWindow window;
    window.init(&local, &groups, false);
    int groupActionCount = 0;
    for(QAction* action : window.findChildren<QAction*>()) {
      if(!action->text().contains("Group"))
        continue;
      QVERIFY(!action->isVisible());
      QVERIFY(!action->isEnabled());
      groupActionCount++;
    }
    QCOMPARE(groupActionCount, 3);
  }

  void flatContactsCanBeSelected() {
    User local = contact("local", "Local");
    User alice = contact("alice", "Alice");
    QList<Group> groups;
    lmcMainWindow window;
    window.init(&local, &groups, false);
    window.addUser(&alice);
    QList<QTreeWidgetItem*> contacts = window.getContactsList();
    QTreeWidget owner;
    owner.addTopLevelItems(contacts);
    lmcUserSelectDialog dialog;
    dialog.init(&contacts);
    auto* tree = dialog.findChild<lmcUserTreeWidget*>("tvUserList");
    auto* button = dialog.findChild<QPushButton*>("btnOK");
    QVERIFY(tree);
    QVERIFY(button);
    QCOMPARE(tree->topLevelItemCount(), 1);
    QVERIFY(!button->isEnabled());
    tree->topLevelItem(0)->setCheckState(0, Qt::Checked);
    QVERIFY(button->isEnabled());
    tree->setView(ULV_Compact);
    QVERIFY(button->isEnabled());
    button->click();
    QCOMPARE(dialog.selectedContacts, QStringList{alice.id});
    tree->topLevelItem(0)->setCheckState(0, Qt::Unchecked);
    QVERIFY(!button->isEnabled());
  }

  void broadcastConsumerAcceptsFlatContacts() {
    User local = contact("local", "Local");
    User alice = contact("alice", "Alice");
    QList<Group> groups;
    lmcMainWindow window;
    window.init(&local, &groups, false);
    window.addUser(&alice);
    QList<QTreeWidgetItem*> contacts = window.getContactsList();
    lmcBroadcastWindow broadcast;
    broadcast.setAttribute(Qt::WA_DeleteOnClose, false);
    broadcast.init(true);
    broadcast.show(&contacts);
    auto* editor = broadcast.findChild<QTextEdit*>("txtMessage");
    auto* button = broadcast.findChild<QPushButton*>("btnSend");
    QVERIFY(editor);
    QVERIFY(button);
    QStringList recipients;
    connect(&broadcast, &lmcBroadcastWindow::messageSent, &broadcast,
            [&recipients](MessageType type, QString* userId, XmlMessage*) {
              QCOMPARE(type, MT_Broadcast);
              recipients.append(*userId);
            });
    editor->setPlainText("test");
    button->click();
    QCOMPARE(recipients, QStringList{alice.id});
  }
};

int main(int argc, char** argv) {
  qputenv("QT_QPA_PLATFORM", "offscreen");
  QStandardPaths::setTestModeEnabled(true);
  QTemporaryDir settingsDirectory;
  if(!settingsDirectory.isValid()) {
    qCritical("Cannot create temporary settings directory: %s", qPrintable(settingsDirectory.errorString()));
    return 1;
  }
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDirectory.path());
  QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsDirectory.path());
  QApplication application(argc, argv);
  QCoreApplication::setOrganizationName("LanChatLiteTests");
  QCoreApplication::setApplicationName("contactgroups");
  const QString dataParent = QFileInfo(QStandardPaths::writableLocation(
                                         QStandardPaths::AppLocalDataLocation))
                               .absolutePath();
  if(!QDir().mkpath(dataParent)) {
    qCritical("Cannot create test data parent directory: %s", qPrintable(dataParent));
    return 1;
  }
  QTemporaryDir dataDirectory(QDir(dataParent).filePath("contactgroups-XXXXXX"));
  if(!dataDirectory.isValid()) {
    qCritical("Cannot create temporary app-data directory: %s", qPrintable(dataDirectory.errorString()));
    return 1;
  }
  QCoreApplication::setApplicationName(QFileInfo(dataDirectory.path()).fileName());
  if(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)) !=
     QDir::cleanPath(dataDirectory.path())) {
    qCritical("Application data is not isolated in the temporary test directory");
    return 1;
  }
  TstContactGroups test;
  return QTest::qExec(&test, argc, argv);
}

#include "tst_contactgroups.moc"
