// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QMimeData>
#include <QTemporaryDir>
#include "gui/widgets/subcontrols.h"
#include "gui/widgets/clipboardimage.h"

class MessageEditProbe : public lmcMessageEdit {
public:
  using lmcMessageEdit::canInsertFromMimeData;
  using lmcMessageEdit::insertFromMimeData;
};

class ClipboardImageTest : public QObject {
  Q_OBJECT

private slots:
  void textPasteKeepsNormalEditing() {
    MessageEditProbe editor;
    QMimeData mime;
    mime.setText(QStringLiteral("a note\nwith a second line"));
    QSignalSpy images(&editor, &lmcMessageEdit::imagePasted);
    QVERIFY(editor.canInsertFromMimeData(&mime));
    editor.insertFromMimeData(&mime);
    QCOMPARE(editor.toPlainText(), mime.text());
    QCOMPARE(images.count(), 0);
  }

  void imagePasteIsAttachmentAndPreservesDraft() {
    MessageEditProbe editor;
    editor.setPlainText(QStringLiteral("unsent draft"));
    QImage image(48, 32, QImage::Format_ARGB32);
    image.fill(QColor(10, 90, 240, 180));
    QMimeData mime;
    mime.setImageData(image);
    QSignalSpy images(&editor, &lmcMessageEdit::imagePasted);
    QSignalSpy rejected(&editor, &lmcMessageEdit::imagePasteRejected);
    QVERIFY(editor.canInsertFromMimeData(&mime));
    editor.insertFromMimeData(&mime);
    QCOMPARE(images.count(), 1);
    QCOMPARE(images.at(0).at(0).value<QImage>(), image);
    QCOMPARE(rejected.count(), 0);
    QCOMPARE(editor.toPlainText(), QStringLiteral("unsent draft"));
    QVERIFY(!editor.toHtml().contains(QStringLiteral("<img")));
  }

  void oversizedImageIsRejectedWithoutLosingText() {
    MessageEditProbe editor;
    editor.setPlainText(QStringLiteral("keep this draft"));
    // Mono makes this >16MP fixture about 2MB, rather than >64MB of RGBA.
    QImage image(4096, 4097, QImage::Format_Mono);
    QVERIFY(!image.isNull());
    QMimeData mime;
    mime.setImageData(image);
    QSignalSpy images(&editor, &lmcMessageEdit::imagePasted);
    QSignalSpy rejected(&editor, &lmcMessageEdit::imagePasteRejected);
    editor.insertFromMimeData(&mime);
    QCOMPARE(images.count(), 0);
    QCOMPARE(rejected.count(), 1);
    QCOMPARE(editor.toPlainText(), QStringLiteral("keep this draft"));
  }

  void decodedBytesAreBoundedAsWellAsPixels() {
    QImage image(4096, 2049, QImage::Format_RGBA64);
    QVERIFY(!image.isNull());
    QVERIFY(qint64(image.width()) * image.height() < ClipboardImage::MaxPixels);
    QVERIFY(image.sizeInBytes() > ClipboardImage::MaxDecodedBytes);
    QVERIFY(!ClipboardImage::isWithinLimits(image));
    QVERIFY(!ClipboardImage::isWithinLimits(QImage()));
  }

  void pngStagingIsLosslessAndUnique() {
    QTemporaryDir temporary;
    QVERIFY(temporary.isValid());
    const QString directory = temporary.filePath(QStringLiteral("images with spaces"));
    QVERIFY(QDir().mkpath(directory));
    QImage original(50, 30, QImage::Format_ARGB32);
    original.fill(QColor(10, 20, 30, 160));
    original.setPixelColor(5, 8, QColor(100, 120, 140, 200));
    QString error;
    const QString first = ClipboardImage::savePng(original, directory, &error);
    QVERIFY2(!first.isEmpty(), qPrintable(error));
    const QString second = ClipboardImage::savePng(original, directory, &error);
    QVERIFY2(!second.isEmpty(), qPrintable(error));
    QVERIFY(first != second);
    QCOMPARE(QImage(first), original);
    QCOMPARE(QImage(second), original);
    QCOMPARE(QDir(directory).entryList(QDir::Files).size(), 2);
  }

  void stagingFailureCreatesNoAttachment() {
    QTemporaryDir temporary;
    QVERIFY(temporary.isValid());
    QImage image(8, 8, QImage::Format_ARGB32);
    image.fill(Qt::blue);
    QString error;
    QVERIFY(ClipboardImage::savePng(image, temporary.filePath("missing"), &error).isEmpty());
    QVERIFY(!error.isEmpty());
    QVERIFY(ClipboardImage::savePng(QImage(), temporary.path(), &error).isEmpty());
    QVERIFY(QDir(temporary.path()).entryList(QDir::Files).isEmpty());
  }
};

QTEST_MAIN(ClipboardImageTest)
#include "tst_clipboardimage.moc"
