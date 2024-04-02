// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QMenu>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QAction>
#include <QString>
#include <QRegExp>
#include <QHash>
#include <functional>
#include <QSet>

#include "../api/api.h"
#include "../core/core.h"
#include "MdiWindow.h"

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "Session.h"

struct MainWindowSettings {

    static constexpr const char* geometryKey = "geometry";
    static constexpr const char* geometryValue = "";

    MainWindowSettings() 
    : geometry(geometryValue,strlen(geometryValue)){
    }

    MainWindowSettings(const QSettings& settings, const QString& settingsPath) 
    : geometry(){
    }

    QByteArray geometry;
};

class MainWindow : public Window {
    Q_OBJECT

    friend class Session;

signals:
    void settingsChanged();
    
public:
    static constexpr const char* winTitle = "szpuler";
    static constexpr const char* fatalError = "Fatal error occurred please contact support";

    MainWindow(MLoader* plugins = nullptr, Logger* logger = nullptr);

    ~MainWindow();

    QSettings& settings();  
    
    MainWindow& operator<< (const QString& msg);
    
    //Logger* logger();
    
    MLoader* plugins();

    void setPlugins(MLoader* loader);

    bool addSubWindow(QWidget* widget, const QString& title = QString()) override;

protected:

    QByteArray state() const;

    void setState(const QByteArray& state);

    ads::CDockWidget* addSubWindowInternal(QWidget* widget, const QString& title = QString());

    void closeEvent(QCloseEvent* ev) override;

signals:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

    void aboutToQuit();

public slots:

    void createOrActivate() override;

    void showStatusMessage(const QString& msg, int timeout = 0) override;

    void createPreferences();

private slots:

    void about();

    void switchLayoutDirection();

private:

    bool createWindowByName(const QString& name);

    void createActions();
    
    void createStatusBar();
    
    void readSettings();
    
    void writeSettings();
    
    ads::CDockWidget* findChildWindow(const QString& name) const;
    
    static constexpr const int statusTimeOut = 2000;
    static constexpr const char* aboutText = "The <b>MDI</b> example demonstrates how to write multiple "
                                             "document interface applications using Qt.";
    static constexpr int QSlotInvalid = -1;

    ads::CDockManager* m_dockManager = nullptr;
    ads::CDockAreaWidget* m_area = nullptr;

    QMenu *m_windowMenu = nullptr;
    QAction *m_closeAct = nullptr;
    QAction *m_closeAllAct = nullptr;
    QAction *m_tileAct = nullptr;
    QAction *m_cascadeAct = nullptr;
    QAction *m_nextAct = nullptr;
    QAction *m_previousAct = nullptr;
    QAction *m_windowMenuSeparatorAct = nullptr;
    QSettings m_settings;
    QAction *m_actionConnect = nullptr;
    QAction *m_actionDisconnect = nullptr;
    QAction *m_actionConfigure = nullptr;
    MLoader* m_plugins = nullptr;
    //QMdiSubWindow* m_current = nullptr;
    Logger* m_logger = nullptr;
};

#endif
