// SPDX-License-Identifier: GPL-3.0-or-later
#import <AppKit/AppKit.h>
#include <QtTest>
#include <QAccessible>
#include <QTreeWidget>

// Exercise the shipped Cocoa bridge in-process, without changing system
// accessibility settings or requiring permission to inspect other apps.
@interface NSObject (LanChatAccessibilityTest)
+ (id)elementWithInterface:(QAccessibleInterface*)interface;
@end

class TstMacOSAccessibility : public QObject {
  Q_OBJECT
private slots:
  void contactRowsSurviveModelReset_data() {
    QTest::addColumn<int>("columns");
    QTest::newRow("single-column") << 1;
    QTest::newRow("two-columns") << 2;
  }

  void contactRowsSurviveModelReset() {
    QFETCH(int, columns);
    QCOMPARE(QGuiApplication::platformName(), QStringLiteral("cocoa"));
    QAccessible::setActive(true);
    Class elementClass = NSClassFromString(@"QMacAccessibilityElement");
    QVERIFY(elementClass);
    for(int cycle = 0; cycle < 10; ++cycle) {
      @autoreleasepool {
        QTreeWidget tree;
        tree.setColumnCount(columns);
        new QTreeWidgetItem(&tree, {"First contact"});
        new QTreeWidgetItem(&tree, {"Second contact"});
        tree.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tree));
        QAccessibleInterface* interface = QAccessible::queryAccessibleInterface(&tree);
        QVERIFY(interface);
        id<NSAccessibility> element = [elementClass elementWithInterface:interface];
        NSArray* rows = [element accessibilityRows];
        QCOMPARE(rows.count, 2u);
        for(id<NSAccessibility> row in rows) {
          NSArray* cells = [row accessibilityChildren];
          QCOMPARE(cells.count, NSUInteger(columns));
          for(id<NSAccessibility> cell in cells)
            QVERIFY([cell accessibilityLabel] != nil);
        }
        tree.clear();
        QCoreApplication::processEvents();
      }
    }
  }
};

QTEST_MAIN(TstMacOSAccessibility)
#include "tst_macosaccessibility.moc"
