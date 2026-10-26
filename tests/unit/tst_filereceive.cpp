// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 LanChat Lite contributors.
#include <QtTest>
#include <QSaveFile>
#include <QScopeGuard>
#include <QStandardPaths>
#include <QTemporaryDir>

#include "messaging/messaging.h"
#include "gui/files/transferwindow.h"

namespace {
QByteArray readFile(const QString& path) {
  QFile file(path);
  return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}
} // namespace

class TstFileReceive : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void init();
  void concurrentNamesDoNotOverwrite();
  void existingNamesAreSkipped();
  void nameSuffix_data();
  void nameSuffix();
  void cancelledNameCanBeReused();
  void caseCollisionsAreReserved();
  void iconPreservesExistingTemporaryFile();

private:
  QString accept(lmcMessaging& messaging, const QString& id, const QString& name);
  QTemporaryDir profile;
  QTemporaryDir destination;
};

void TstFileReceive::initTestCase() {
  QVERIFY(profile.isValid());
  QVERIFY(destination.isValid());
  QStandardPaths::setTestModeEnabled(true);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, profile.path());
  QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, profile.path());
}

void TstFileReceive::init() {
  lmcSettings settings;
  settings.clear();
  settings.setValue(IDS_FILESTORAGEPATH, destination.path());
  settings.sync();
  QCOMPARE(settings.status(), QSettings::NoError);
}

QString TstFileReceive::accept(lmcMessaging& messaging, const QString& id, const QString& name) {
  QString peer = QStringLiteral("test-peer");
  XmlMessage message;
  message.addData(XN_FILEID, id);
  message.addData(XN_FILENAME, name);
  message.addData(XN_FILESIZE, QStringLiteral("5"));
  message.addData(XN_FILETYPE, FileTypeNames[FT_Normal]);
  if(!messaging.addFileTransfer(FM_Receive, &peer, &message) ||
     !messaging.updateFileTransfer(FM_Receive, FO_Accept, &peer, &message))
    return QString();
  return message.data(XN_FILEPATH);
}

void TstFileReceive::concurrentNamesDoNotOverwrite() {
  lmcMessaging messaging;
  const QString first = accept(messaging, "first", "concurrent.txt");
  const QString second = accept(messaging, "second", "concurrent.txt");
  QCOMPARE(QFileInfo(first).fileName(), QStringLiteral("concurrent.txt"));
  QCOMPARE(QFileInfo(second).fileName(), QStringLiteral("concurrent_1.txt"));
  QVERIFY(!QFile::exists(first));
  QVERIFY(!QFile::exists(second));

  QSaveFile firstFile(first);
  QSaveFile secondFile(second);
  QVERIFY(firstFile.open(QIODevice::WriteOnly));
  QVERIFY(secondFile.open(QIODevice::WriteOnly));
  QCOMPARE(firstFile.write("first"), qint64(5));
  QCOMPARE(secondFile.write("other"), qint64(5));
  QVERIFY(firstFile.commit());
  QVERIFY(secondFile.commit());
  QCOMPARE(readFile(first), QByteArray("first"));
  QCOMPARE(readFile(second), QByteArray("other"));
}

void TstFileReceive::existingNamesAreSkipped() {
  const QString original = destination.filePath("existing.txt");
  QSaveFile file(original);
  QVERIFY(file.open(QIODevice::WriteOnly));
  QCOMPARE(file.write("original"), qint64(8));
  QVERIFY(file.commit());

  lmcMessaging messaging;
  const QString first = accept(messaging, "first", "existing.txt");
  const QString second = accept(messaging, "second", "existing.txt");
  QCOMPARE(QFileInfo(first).fileName(), QStringLiteral("existing_1.txt"));
  QCOMPARE(QFileInfo(second).fileName(), QStringLiteral("existing_2.txt"));
  QCOMPARE(readFile(original), QByteArray("original"));
}

void TstFileReceive::nameSuffix_data() {
  QTest::addColumn<QString>("name");
  QTest::addColumn<QString>("expected");
  QTest::newRow("no-extension") << QStringLiteral("README") << QStringLiteral("README_1");
  QTest::newRow("dotfile") << QStringLiteral(".env") << QStringLiteral(".env_1");
  QTest::newRow("multiple-dots") << QStringLiteral("archive.tar.gz") << QStringLiteral("archive.tar_1.gz");
  QTest::newRow("unicode") << QStringLiteral("测试 文件.txt") << QStringLiteral("测试 文件_1.txt");
}

void TstFileReceive::nameSuffix() {
  QFETCH(QString, name);
  QFETCH(QString, expected);
  lmcMessaging messaging;
  QCOMPARE(QFileInfo(accept(messaging, "first", name)).fileName(), name);
  QCOMPARE(QFileInfo(accept(messaging, "second", name)).fileName(), expected);
}

void TstFileReceive::cancelledNameCanBeReused() {
  lmcMessaging messaging;
  const QString first = accept(messaging, "cancelled", "cancelled.txt");
  QCOMPARE(QFileInfo(first).fileName(), QStringLiteral("cancelled.txt"));
  QString peer = QStringLiteral("test-peer");
  XmlMessage cancel;
  cancel.addData(XN_FILEID, QStringLiteral("cancelled"));
  cancel.addData(XN_FILETYPE, FileTypeNames[FT_Normal]);
  QVERIFY(messaging.updateFileTransfer(FM_Receive, FO_Cancel, &peer, &cancel));
  QCOMPARE(accept(messaging, "retry", "cancelled.txt"), first);
}

void TstFileReceive::caseCollisionsAreReserved() {
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
  lmcMessaging messaging;
  accept(messaging, "first", "Case.txt");
  QCOMPARE(QFileInfo(accept(messaging, "second", "case.txt")).fileName(), QStringLiteral("case_1.txt"));
#else
  QSKIP("Case-insensitive destination reservation applies to Windows and macOS.");
#endif
}

void TstFileReceive::iconPreservesExistingTemporaryFile() {
  QFile existing(QDir::temp().filePath("lanchat-icon-sentinel-" + QFileInfo(profile.path()).fileName() + ".txt"));
  QVERIFY(existing.open(QIODevice::WriteOnly | QIODevice::NewOnly));
  const auto cleanup = qScopeGuard([&existing] { existing.remove(); });
  const QByteArray original("another application's temporary content");
  QCOMPARE(existing.write(original), qint64(original.size()));
  existing.close();
  const QString incoming = destination.filePath(QFileInfo(existing.fileName()).fileName());
  QVERIFY(!QFile::exists(incoming));
  const QStringList before = QDir::temp().entryList({"lanchat-lite-icons-*"}, QDir::Dirs | QDir::NoDotAndDotDot);

  lmcTransferWindow window;
  window.getIcon(incoming);

  QVERIFY(QFile::exists(existing.fileName()));
  QCOMPARE(readFile(existing.fileName()), original);
  QVERIFY(!QFile::exists(incoming));
  QCOMPARE(QDir::temp().entryList({"lanchat-lite-icons-*"}, QDir::Dirs | QDir::NoDotAndDotDot), before);
}

QTEST_MAIN(TstFileReceive)
#include "tst_filereceive.moc"
