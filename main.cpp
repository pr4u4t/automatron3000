// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "main.h"
#include "mainwindow.h"
#include <QDebug>
#include <exception>
#include "datawindow.h"

int main(int argc, char *argv[]){
    qRegisterMetaType<DataWindow*>("DataWindow*");
    
    Main app(argc,argv);
    MainWindow mainWin;
    mainWin.show();
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
