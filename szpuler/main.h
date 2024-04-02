#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProxyStyle>
#include <QTranslator>
#include <QLibraryInfo>

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
    Main(int& argc, char* argv[], Logger* log)
        : QApplication(argc, argv)
        , m_logger(log) {
        log->message("Main::Main()", LoggerSeverity::LOG_DEBUG);

        QCoreApplication::setApplicationName(Main::appName);
        QCoreApplication::setOrganizationName(Main::org);
        QCoreApplication::setApplicationVersion(QT_VERSION_STR);

        setStyle(new ProxyStyle());
        setWindowIcon(QIcon(":/res/splash.jpg"));

        QLocale locale = QLocale::system();

        if (Settings::localeNeeded()) {
            log->message("Main::Main: locale enabled");
            QTranslator* m_translator = new QTranslator();
            QTranslator* m_qtTranslator = new QTranslator();
            QTranslator* m_qtBaseTranslator = new QTranslator();

            if (m_translator->load(locale, "Translation", "_", "translations")) { //set directory of ts
                installTranslator(m_translator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("Translation"));
            }
            else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("Translation"));
            }

            if (m_qtTranslator->load(locale,
                "qt", "_",
                QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
                installTranslator(m_qtTranslator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("qt"));
            }
            else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("qt"));
            }

            if (m_qtBaseTranslator->load("qtbase_" + locale.name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
                installTranslator(m_qtBaseTranslator);
                log->message(QString("Main::Main: Successfully installed %1 translation").arg("qtbase"));
            }
            else {
                log->message(QString("Main::Main: Failed to install %1 translation").arg("qtbase"));
            }
        }

        parseArgumnets();
    }
    
    static constexpr const char* appName = "Szpuler";
    static constexpr const char* org = "Pawel Ciejka";
    static constexpr const char* description = "Szpuler";
    
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
