// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QSplashScreen>
#include <QDebug>
#include <exception>

#include "main.h"
#include "mainwindow.h"
#include "logviewer.h"
#include "settingsdialog.h"
#include "ModuleLoader.h"

int main(int argc, char *argv[]){
    Main app(argc,argv);
    
    QPixmap pixmap(":/res/arduino.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    MainWindow mainWin;
    MLoader ld(&mainWin);
    mainWin.setPlugins(&ld);
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
    
    parseArgumnets();
}

void Main::parseArgumnets(){
    QCommandLineParser parser;
    parser.setApplicationDescription(Main::description);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);
}
