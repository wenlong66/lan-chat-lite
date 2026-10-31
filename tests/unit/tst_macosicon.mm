// SPDX-License-Identifier: GPL-3.0-or-later
#import <AppKit/AppKit.h>
#include <QtTest>
#include "core/uidefinitions.h"

class TstMacOSIcon : public QObject {
  Q_OBJECT
private slots:
  void dockIconKeepsHighResolution() {
    QCOMPARE(QGuiApplication::platformName(), QStringLiteral("cocoa"));
    const QIcon icon(IDR_APPICON);
    QVERIFY(!icon.isNull());
    qInfo() << "Application icon sizes:" << icon.availableSizes();
    QApplication::setWindowIcon(icon);
    NSImage* dockIcon = NSApp.applicationIconImage;
    QVERIFY(dockIcon);
    NSInteger largest = 0;
    for(NSImageRep* representation in dockIcon.representations)
      largest = qMax(largest, representation.pixelsWide);
    qInfo() << "Largest native Dock representation:" << largest;
    QVERIFY2(largest >= 256, "Starting the app replaced its Dock icon with a low resolution image");
  }
};

QTEST_MAIN(TstMacOSIcon)
#include "tst_macosicon.moc"
