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
    Logger log(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/szpuler.log");
    Main app(argc,argv, &log);

    Splash splash(":/res/splash.jpg");
    splash.show();
    app.processEvents();

    MLoader ld(&log);
    MainWindow mainWin(&ld, &log);
    GuiLoaderContext ctx(&mainWin);
    ld.setContext(&ctx);
    ld.loadPlugins();

    Session session(&ld, &log, "configuration.ini", &mainWin);
    
    Tray tray(&mainWin);

    mainWin.setPlugins(&ld);
    session.restore();
    mainWin.show();
    splash.finish(&mainWin);
    tray.show();

    QObject::connect(&mainWin, SIGNAL(aboutToQuit()), &session, SLOT(store()));

    return app.exec();
}

