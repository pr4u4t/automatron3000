// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QSplashScreen>
#include <QDebug>
#include <QLibraryInfo>
#include <exception>
#include <QStandardPaths>

#include "main.h"
#include "mainwindow.h"
#include "logviewer.h"
#include "ModuleLoader.h"
#include "Tray.h"

#include "api/api.h"

int main(int argc, char *argv[]){
    Logger log(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/szpuler.log");
    Main app(argc,argv, &log);

    QPixmap pixmap(":/res/arduino.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    MLoader ld(QString(),&log);
    MainWindow mainWin(&ld, &log);
    ld.loadPlugins(&mainWin);

    
    Tray tray(&mainWin);

    mainWin.setPlugins(&ld);
    mainWin.restoreSession();
    mainWin.show();
    splash.finish(&mainWin);
    tray.show();

    return app.exec();
}

/*--------------------------------------------------------*/

Main::Main(int& argc, char *argv[], Logger* log)
    : QApplication(argc, argv)
    , m_logger(log){
    log->message("Main::Main()", LoggerSeverity::LOG_DEBUG);

    QCoreApplication::setApplicationName(Main::appName);
    QCoreApplication::setOrganizationName(Main::org);
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    
    setStyle(new ProxyStyle());
    setWindowIcon(QIcon(":/res/arduino.png"));
    QTranslator* m_translator = new QTranslator();
    QTranslator* m_qtTranslator = new QTranslator();
    QTranslator* m_qtBaseTranslator = new QTranslator();

    if (m_translator->load(QLocale::system(), "Translation", "_", "translations")) { //set directory of ts
        installTranslator(m_translator);
        log->message(QString("Main::Main: Successfully installed %1 translation").arg("Translation"));
    } else {
        log->message(QString("Main::Main: Failed to install %1 translation").arg("Translation"));
    }

    if (m_qtTranslator->load(QLocale::system(),
        "qt", "_",
        QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        installTranslator(m_qtTranslator);
        log->message(QString("Main::Main: Successfully installed %1 translation").arg("qt"));
    } else {
        log->message(QString("Main::Main: Failed to install %1 translation").arg("qt"));
    }
    
    if (m_qtBaseTranslator->load("qtbase_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        installTranslator(m_qtBaseTranslator);
        log->message(QString("Main::Main: Successfully installed %1 translation").arg("qtbase"));
    } else {
        log->message(QString("Main::Main: Failed to install %1 translation").arg("qtbase"));
    }

    parseArgumnets();
}

void Main::parseArgumnets(){
    m_logger->message("Main::parseArgumnets");
    QCommandLineParser parser;
    parser.setApplicationDescription(Main::description);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);
}
