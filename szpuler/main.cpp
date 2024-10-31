// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QSplashScreen>
#include <QLibraryInfo>
#include <exception>
#include <QScreen>
#include <QLockFile>

#include "main.h"
#include "mainwindow.h"
#include "logviewer.h"
#include "../core/core.h"
#include "Tray.h"
#include "Session.h"
#include "../api/api.h"
#include "Splash.h"
#include "WebServer.h"

int main(int argc, char *argv[]){
    Settings::setConfigurationPath(Main::configurationPath());
    Logger log(Main::configurationPath() + "/" + Main::appName  + ".log");
    Main app(argc,argv, &log);
    QLockFile lock(Main::configurationPath() + "/" + Main::appName + ".lock");

    if (lock.tryLock() == false) {
        log.message(QString("Faild to obtain lock %1 it seems another instance is running").arg(Main::configurationPath() + "/" + Main::appName + ".lock"));
        QMessageBox::critical(nullptr, Main::appName, QString("Failed to obtain lock file %1, another instance of application is already running").arg(lock.fileName()));
        return -1;
    }

    Splash splash(":/res/splash.jpg");
    splash.show();
    splash.setProgress(0);
    //app.processEvents();

    splash << "creating module loader...";
    MLoader ld(Settings::get(), "Modules", &log);
    splash.setProgress(20);

    splash << "creating main window...";
    MainWindow mainWin(Settings::get(), "Window", &ld, &log);
    GuiLoaderContext ctx(&mainWin);
    ld.setContext(&ctx);
    splash.setProgress(40);

    splash << "loading plugins...";
    const auto pluginCount = ld.loadPlugins();
    log.message(QString("Loaded %1 plugins").arg(pluginCount));
    splash.setProgress(50);

    splash << "creating session...";
    Session session(&ld, &log, Main::configurationPath() + "/" + "configuration.ini", &mainWin);
    QObject::connect(&session, &Session::sessionRestored, &mainWin, &MainWindow::sessionRestored);
    splash.setProgress(70);

    splash << "creating tray icon...";
    Tray tray(&mainWin);
    splash.setProgress(80);
    mainWin.setPlugins(&ld);

    splash << "restoring session...";
    session.restore();
    splash.setProgress(90);

    splash << "startup finished";
    splash.setProgress(100);
    mainWin.show();
    splash.finish(&mainWin);
    tray.show();

    QObject::connect(&mainWin, SIGNAL(aboutToQuit(const MainWindowQuit&)), &session, SLOT(aboutToQuit(const MainWindowQuit&)));

    WebServer server(&ld);
    server.start();

    return app.exec();
}

