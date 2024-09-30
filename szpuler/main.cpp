// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QSplashScreen>
#include <QDebug>
#include <QLibraryInfo>
#include <exception>
#include <QStandardPaths>
#include <QScreen>

#include "main.h"
#include "mainwindow.h"
#include "logviewer.h"
#include "../core/core.h"
#include "Tray.h"
#include "Session.h"
#include "../api/api.h"
#include "Splash.h"

int main(int argc, char *argv[]){
    Settings::setConfigurationPath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + Main::appName);
    Logger log(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + Main::appName + "/" + Main::appName  + ".log");
    Main app(argc,argv, &log);

    Splash splash(":/res/splash.jpg");
    splash.show();
    app.processEvents();

    MLoader ld(&log);
    MainWindow mainWin(&ld, &log);
    GuiLoaderContext ctx(&mainWin);
    ld.setContext(&ctx);
    ld.loadPlugins();

    Session session(&ld, &log, QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + "configuration.ini", &mainWin);
    QObject::connect(&mainWin, &MainWindow::sessionStore, &session, &Session::store);
    Tray tray(&mainWin);

    mainWin.setPlugins(&ld);
    session.restore();
    mainWin.show();
    splash.finish(&mainWin);
    tray.show();

    QObject::connect(&mainWin, SIGNAL(aboutToQuit()), &session, SLOT(store()));

    return app.exec();
}

