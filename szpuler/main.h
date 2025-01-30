#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProxyStyle>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStyleHints>
#include <QStandardPaths>

#include "../api/api.h"

class ProxyStyle : public QProxyStyle{
public:
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override {
        if (hint == QStyle::SH_UnderlineShortcut) {
            return 1;
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

class Main : public QApplication{
    
    Q_OBJECT

public:
    static constexpr const char* const appName = "Automatron 3000";
    static constexpr const char* const org = "Pawel Ciejka";
    static constexpr const char* const description = "Automatron 3000";

    Main(int& argc, char* argv[], Logger* log)
        : QApplication(argc, argv)
        , m_logger(log) {
        log->message("Main::Main()", LoggerSeverity::LOG_DEBUG);
        log->message("Main::Main: using configuration: "+Settings::settingsPath());

        QCoreApplication::setApplicationName(Main::appName);
        QCoreApplication::setOrganizationName(Main::org);
        QCoreApplication::setApplicationVersion(QT_VERSION_STR);

        setStyle(new ProxyStyle());
        setWindowIcon(QIcon(":/res/arduino.ico"));

        QLocale locale = QLocale::system();

        if (Settings::localeNeeded()) {
            log->message("Main::Main: locale enabled");
            QTranslator* m_translator = new QTranslator();
            QTranslator* m_qtTranslator = new QTranslator();
            QTranslator* m_qtBaseTranslator = new QTranslator();
            QTranslator* m_apiTranslator = new QTranslator();

            if (m_translator->load(locale, "Translation", "_", "translations")) { //set directory of ts
                installTranslator(m_translator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("Translation"));
            } else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("Translation"));
            }

            if (m_apiTranslator->load(locale, "api", "_", "translations")) { //set directory of ts
                installTranslator(m_apiTranslator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("api"));
            }
            else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("api"));
            }

            if (m_qtTranslator->load(locale,
                "qt", "_",
                QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
                installTranslator(m_qtTranslator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("qt"));
            } else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("qt"));
            }

            if (m_qtBaseTranslator->load("qtbase_" + locale.name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
                installTranslator(m_qtBaseTranslator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("qtbase"));
            } else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("qtbase"));
            }
        }

        QStyleHints* hints = styleHints();
        switch (hints->colorScheme()) {
        case Qt::ColorScheme::Light:
        case Qt::ColorScheme::Unknown:
            log->message("Main::Main: found light or unknown color scheme");
            break;

        case Qt::ColorScheme::Dark:
            log->message("Main::Main: found dark color scheme");
            setStyle("fusion");
            break;
        }

        parseArgumnets();
    }
    
    static QString configurationPath() {
        return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + appName;
    }
    
protected:
    
    void parseArgumnets() {
        m_logger->message("Main::parseArgumnets");
        QCommandLineParser parser;
        parser.setApplicationDescription(Main::description);
        parser.addHelpOption();
        parser.addVersionOption();
        parser.process(*this);
    }

private:

    Logger* m_logger = nullptr;
};

#endif
