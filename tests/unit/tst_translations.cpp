// SPDX-License-Identifier: GPL-3.0-or-later
#include <QtTest>
#include <QComboBox>
#include <QFile>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTranslator>
#include <QXmlStreamReader>
#include "application.h"
#include "core/lmcstrings.h"
#include "core/settings.h"
#include "gui/main/settingsdialog.h"
#include "gui/widgets/clipboardimage.h"

class TranslationsTest : public QObject {
  Q_OBJECT

private:
  QTemporaryDir settingsDirectory;

  static QStringList placeholders(const QString& text) {
    const QRegularExpression pattern(QStringLiteral("%(?:L?[1-9][0-9]?|n)"));
    QStringList result;
    auto matches = pattern.globalMatch(text);
    while(matches.hasNext())
      result.append(matches.next().captured());
    result.sort();
    return result;
  }

private slots:
  void initTestCase() {
    QVERIFY(settingsDirectory.isValid());
    QStandardPaths::setTestModeEnabled(true);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDirectory.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsDirectory.path());
    Application::loadTranslations(settingsDirectory.path());
    QCOMPARE(Application::supportedLanguage("zh_CN"), QStringLiteral("en_US"));
    Application::loadTranslations(QStringLiteral(LMC_TRANSLATIONS_DIR));
    QVERIFY(Application::availableLanguages().contains("zh_CN"));
    QVERIFY(Application::availableLanguages().contains("en_US"));
  }

  void cleanup() {
    Application::setLanguage("en_US");
    lmcStrings::retranslate();
    QCoreApplication::processEvents();
    lmcSettings settings;
    settings.clear();
    settings.sync();
    QCOMPARE(settings.status(), QSettings::NoError);
  }

  void localeResolution() {
    QCOMPARE(Application::supportedLanguage("zh_CN"), QStringLiteral("zh_CN"));
    QCOMPARE(Application::supportedLanguage("zh-CN"), QStringLiteral("zh_CN"));
    QCOMPARE(Application::supportedLanguage("zh-Hans-CN"), QStringLiteral("zh_CN"));
    QCOMPARE(Application::supportedLanguage("zh_TW"), QStringLiteral("en_US"));
    QCOMPARE(Application::supportedLanguage("unknown"), QStringLiteral("en_US"));
  }

#ifdef Q_OS_MACOS
  void macBundleDeclaresShippedLanguages() {
    QFile file(QStringLiteral(LMC_MACOS_INFO_PLIST));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QXmlStreamReader xml(&file);
    QString key;
    QStringList languages;
    QString developmentRegion;
    while(!xml.atEnd()) {
      xml.readNext();
      if(!xml.isStartElement())
        continue;
      if(xml.name() == QLatin1String("key"))
        key = xml.readElementText();
      else if(xml.name() == QLatin1String("string")) {
        const QString value = xml.readElementText();
        if(key == QLatin1String("CFBundleLocalizations"))
          languages.append(value);
        else if(key == QLatin1String("CFBundleDevelopmentRegion"))
          developmentRegion = value;
      }
    }
    QVERIFY2(!xml.hasError(), qPrintable(xml.errorString()));
    QVERIFY2(languages.contains("zh-CN"), "macOS must know that the bundle supports Simplified Chinese");
    QCOMPARE(developmentRegion, QStringLiteral("en"));
    QStringList expected = Application::availableLanguages();
    for(QString& locale : expected)
      locale.replace('_', '-');
    expected.sort();
    languages.sort();
    QCOMPARE(languages, expected);
  }
#endif

  void defaultAndSavedLanguage() {
    lmcSettings settings;
    QVERIFY(!settings.contains(IDS_LANGUAGE));
    QCOMPARE(settings.language(), Application::supportedLanguage(QLocale::system().name()));
    settings.setValue(IDS_LANGUAGE, "zh_CN");
    QCOMPARE(settings.language(), QStringLiteral("zh_CN"));
    settings.setValue(IDS_LANGUAGE, "en_US");
    settings.sync();
    QCOMPARE(lmcSettings().language(), QStringLiteral("en_US"));
    settings.setValue(IDS_LANGUAGE, "unsupported");
    QCOMPARE(settings.language(), QStringLiteral("en_US"));
  }

  void catalogIsCompleteAndPreservesPlaceholders() {
    QFile file(QStringLiteral(LMC_CHINESE_TS));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QTranslator translator;
    QVERIFY(translator.load(QStringLiteral(LMC_TRANSLATIONS_DIR) + "/zh_CN.qm"));
    QXmlStreamReader xml(&file);
    QString context;
    QString source;
    QString translation;
    QString type;
    int messages = 0;
    while(!xml.atEnd()) {
      xml.readNext();
      if(xml.isStartElement()) {
        if(xml.name() == QLatin1String("name"))
          context = xml.readElementText();
        else if(xml.name() == QLatin1String("message")) {
          source.clear();
          translation.clear();
          type.clear();
        } else if(xml.name() == QLatin1String("source"))
          source = xml.readElementText();
        else if(xml.name() == QLatin1String("translation")) {
          type = xml.attributes().value("type").toString();
          translation = xml.readElementText();
        }
      } else if(xml.isEndElement() && xml.name() == QLatin1String("message")) {
        QVERIFY2(type.isEmpty(), qPrintable(context + ": " + source));
        QVERIFY2(!translation.isEmpty(), qPrintable(context + ": " + source));
        QCOMPARE(placeholders(translation), placeholders(source));
        const QString compiled = translator.translate(context.toUtf8().constData(), source.toUtf8().constData());
        QCOMPARE(compiled, translation);
        ++messages;
      }
    }
    QVERIFY2(!xml.hasError(), qPrintable(xml.errorString()));
    QVERIFY(messages > 300);
  }

  void applicationAndQtDialogsSwitchBackToEnglish() {
    Application::setLanguage("zh_CN");
    lmcStrings::retranslate();
    QCOMPARE(QCoreApplication::translate("lmcSettingsDialog", "Preferences"),
             QStringLiteral("首选项"));
    QCOMPARE(Helper::formatSize(12), QStringLiteral("12 字节"));
    QString error;
    QVERIFY(ClipboardImage::savePng(QImage(), settingsDirectory.path(), &error).isEmpty());
    QVERIFY(error != QStringLiteral("The image exceeds the clipboard attachment limit."));
    QMessageBox box(QMessageBox::Question, "test", "test", QMessageBox::Cancel);
    QVERIFY(box.button(QMessageBox::Cancel));
    QCOMPARE(box.button(QMessageBox::Cancel)->text().remove('&'), QStringLiteral("取消"));
#ifdef Q_OS_MACOS
    QCOMPARE(QCoreApplication::translate("MAC_APPLICATION_MENU", "Services"), QStringLiteral("服务"));
    QCOMPARE(QCoreApplication::translate("MAC_APPLICATION_MENU", "Hide %1"), QStringLiteral("隐藏 %1"));
    QCOMPARE(QCoreApplication::translate("MAC_APPLICATION_MENU", "Quit %1"), QStringLiteral("退出 %1"));
    QCOMPARE(QCoreApplication::translate("QCocoaTheme", "Don't Save"), QStringLiteral("不保存"));
#endif
    Application::setLanguage("unsupported");
    QCOMPARE(QCoreApplication::translate("lmcSettingsDialog", "Preferences"), QStringLiteral("Preferences"));
    QCOMPARE(Helper::formatSize(12), QStringLiteral("12 bytes"));
#ifdef Q_OS_MACOS
    QCOMPARE(QCoreApplication::translate("MAC_APPLICATION_MENU", "Services"), QStringLiteral("Services"));
#endif
  }

  void settingsSelectorSwitchesAndPersistsEnglish() {
    lmcSettings settings;
    settings.setValue(IDS_LANGUAGE, "zh_CN");
    Application::setLanguage(settings.language());
    lmcStrings::retranslate();
    lmcSettingsDialog dialog;
    dialog.setAttribute(Qt::WA_DeleteOnClose, false);
    dialog.init();
    auto* selector = dialog.findChild<QComboBox*>("cboLanguage");
    auto* ok = dialog.findChild<QPushButton*>("btnOK");
    QVERIFY(selector);
    QVERIFY(ok);
    QCOMPARE(selector->currentData().toString(), QStringLiteral("zh_CN"));
    QCOMPARE(selector->currentText(), QLocale("zh_CN").nativeLanguageName());
    QCOMPARE(dialog.windowTitle(), QStringLiteral("首选项"));
    selector->setCurrentIndex(selector->findData("en_US"));
    ok->click();
    QVERIFY(settings.contains(IDS_LANGUAGE));
    QCOMPARE(lmcSettings().language(), QStringLiteral("en_US"));
    Application::setLanguage("en_US");
    lmcStrings::retranslate();
    QCoreApplication::processEvents();
    QCOMPARE(dialog.windowTitle(), QStringLiteral("Preferences"));
    auto* fontSizes = dialog.findChild<QComboBox*>("cboFontSize");
    QVERIFY(fontSizes);
    QCOMPARE(fontSizes->itemText(0), QStringLiteral("Small text"));
  }
};

int main(int argc, char** argv) {
  Application application("lanchat-lite-translations-test", argc, argv);
  TranslationsTest test;
  return QTest::qExec(&test, argc, argv);
}

#include "tst_translations.moc"
