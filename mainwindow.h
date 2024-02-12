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

#include "api/api.h"
#include "ModuleLoader.h"

class MainWindow : public Window {
    Q_OBJECT

signals:
    void settingsChanged();
    
public:
    static constexpr const char* winTitle = "szpuler";
    static constexpr const char* fatalError = "Fatal error occurred please contact support";

    MainWindow(MLoader* loader = nullptr);

    ~MainWindow();

    QSettings& settings();  
    
    MainWindow& operator<< (const QString& msg);
    
    Logger* logger();
    
    MLoader* plugins();

    void setPlugins(MLoader* loader) {
        m_plugins = loader;
    }

    bool addSubWindow(QWidget* widget);

    qint64 restoreSession() {
        QString session = settings().value("session/plugins").toString();
        QStringList list = session.split(" ", Qt::SkipEmptyParts);
        qint64 ret = 0;
        QRegExp rx("^([A-Za-z0-9]+)");
        
        for (auto it = list.begin(), end = list.end(); it != end; ++it, ++ret) {
            qDebug() << *it;
            if (rx.indexIn(*it) != -1) {
                QString name = rx.cap(1);
                qDebug() << name;
                plugins()->instance(name, this, *it);
            }
            //
        }

        

        return ret;
    }

protected:
    qint64 saveSession() {
        auto instances = plugins()->instances();
        QString value;

        for (auto it = instances.begin(), end = instances.end(); it != end; ++it) {
            value += (*it)->name() + "-" + (*it)->uuid() + " ";

            if ((*it)->type() == Plugin::Type::WIDGET) {
                qDebug() << "HOYT";
            
            }
        }

        settings().setValue("session/plugins", value);

        return 0;
    }

    void closeEvent(QCloseEvent *event) override;
    
    void showStatusMessage(const QString &message, int timeout = statusTimeOut);
    
    //void showWriteError(const QString &message);
    
    //QStringList subWindows() const;

//public slots:

//    void writeSerial(const QString& msg);

public slots:

    void createOrActivate() override;

private slots:
    void about();
    void updateMenus();
    void updateWindowMenu();
    void switchLayoutDirection();
    //void openSerialPort();
    //void closeSerialPort();
    void createOrActivatePlugins();
    void createOrActivateInstances();
    void createOrActivateLogViewer();
    //void createOrActivateSettings();
    
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

    
    static constexpr const int statusTimeOut = 2000;
    static constexpr const char* aboutText = "The <b>MDI</b> example demonstrates how to write multiple "
                                         "document interface applications using Qt.";
    static constexpr const char* logPath = "szpuler.log";
    //static constexpr const char* settingsPath = "szpuler.ini";
    
    static constexpr int QSlotInvalid = -1;

    QMdiArea *m_mdiArea = nullptr;
    QMenu *m_windowMenu = nullptr;
    QAction *m_closeAct = nullptr;
    QAction *m_closeAllAct = nullptr;
    QAction *m_tileAct = nullptr;
    QAction *m_cascadeAct = nullptr;
    QAction *m_nextAct = nullptr;
    QAction *m_previousAct = nullptr;
    QAction *m_windowMenuSeparatorAct = nullptr;
    QSettings m_settings;
    mutable Logger m_logger;
    //QSerialPort *m_serial = nullptr;
    QAction *m_actionConnect = nullptr;
    QAction *m_actionDisconnect = nullptr;
    QAction *m_actionConfigure = nullptr;
    MLoader* m_plugins = nullptr;
};

#endif
