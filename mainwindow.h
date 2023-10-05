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
#include <QSerialPort>

#include "mdichild.h"
#include "logger.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void settingsChanged();
    
public:
    MainWindow();

    QSettings& settings();  
    
    MainWindow& operator<< (const QString& msg);
    
    Logger* logger();
    
    static constexpr const char* fatalError = "Fatal error occurred please contact support";
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
    void showStatusMessage(const QString &message);
    
    void showWriteError(const QString &message);
    
    QStringList subWindows() const;
    
private slots:
    void about();
    void updateMenus();
    void updateWindowMenu();
    void switchLayoutDirection();
    void createOrActivate();
    void openSerialPort();
    void closeSerialPort();
    
private:

    bool createWindowByName(const QString& name);
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    MdiChild *activeMdiChild() const;
    QMdiSubWindow *findMdiChild(const QString &fileName) const;

    static constexpr const char* winTitle = "szpuler";
    static constexpr const int statusTimeOut = 2000;
    static constexpr const char* aboutText = "The <b>MDI</b> example demonstrates how to write multiple "
                                         "document interface applications using Qt.";
    static constexpr const char* logPath = "szpuler.log";
    //static constexpr const char* settingsPath = "szpuler.ini";
    
    QMdiArea *m_mdiArea;
    QMenu *m_windowMenu;
    QAction *m_closeAct;
    QAction *m_closeAllAct;
    QAction *m_tileAct;
    QAction *m_cascadeAct;
    QAction *m_nextAct;
    QAction *m_previousAct;
    QAction *m_windowMenuSeparatorAct;
    QSettings m_settings;
    mutable Logger m_logger;
    QSerialPort *m_serial = nullptr;
    QAction *m_actionConnect;
    QAction *m_actionDisconnect;
    QAction *m_actionConfigure;
};

#endif
