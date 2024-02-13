// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QSplashScreen>
#include <QDebug>
#include <QLibraryInfo>
#include <exception>

#include "main.h"
#include "mainwindow.h"
#include "logviewer.h"
#include "ModuleLoader.h"

int main(int argc, char *argv[]){
    Main app(argc,argv);
    
    QPixmap pixmap(":/res/arduino.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    MLoader ld;
    MainWindow mainWin(&ld);
    ld.setWindow(&mainWin);
    ld.loadPlugins();
    mainWin.setPlugins(&ld);
    mainWin.restoreSession();
    mainWin.show();

    splash.finish(&mainWin);

    return app.exec();
}

/*--------------------------------------------------------*/

Main::Main(int& argc, char *argv[])
    : QApplication(argc, argv){
        
    QCoreApplication::setApplicationName(Main::appName);
    QCoreApplication::setOrganizationName(Main::org);
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    
    setStyle(new ProxyStyle());

    QTranslator* m_translator = new QTranslator();
    QTranslator* m_qtTranslator = new QTranslator();
    QTranslator* m_qtBaseTranslator = new QTranslator();

    if (m_translator->load(QLocale::system(), "Translation", "_", "translations")) { //set directory of ts
        installTranslator(m_translator);
    }

    if (m_qtTranslator->load(QLocale::system(),
        "qt", "_",
        QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        installTranslator(m_qtTranslator);
    }

    
    if (m_qtBaseTranslator->load("qtbase_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        qDebug() << "qtBaseTranslator ok";
        installTranslator(m_qtBaseTranslator);
    }

    parseArgumnets();
}

void Main::parseArgumnets(){
    QCommandLineParser parser;
    parser.setApplicationDescription(Main::description);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);
}
