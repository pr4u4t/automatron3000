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

#include "api/api.h"
#include "ModuleLoader.h"
#include "MdiWindow.h"

#include "DockManager.h"
#include "DockAreaWidget.h"

class MainWindow : public Window {
    Q_OBJECT

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

    void setPlugins(MLoader* loader) {
        m_plugins = loader;
    }

    bool addSubWindow(QWidget* widget, const QString& title = QString()) override;

    qint64 restoreSession() {
        QString session = settings().value("session/plugins").toString();
        QStringList list = session.split(" ", Qt::SkipEmptyParts);
        qint64 ret = 0;
        QRegExp rx("^([A-Za-z0-9]+)");

        for (auto it = list.begin(), end = list.end(); it != end; ++it, ++ret) {
            qDebug() << *it;
            if (rx.indexIn(*it) != -1) {
                QString name = rx.cap(1);

                auto plugin = plugins()->instance(name, this, *it);

                if (plugin.isNull()) {
                    continue;
                }

                if (plugin->type() == Plugin::Type::WIDGET) {
                    ads::CDockWidget* dockWidget = new ads::CDockWidget(plugin->name());
 
                    dockWidget->setWidget(dynamic_cast<Widget*>(plugin.data()));
                    dockWidget->setFeature(ads::CDockWidget::DontDeleteContent, true);
                    
                    m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget, m_area);
                }
            }
        }

        QByteArray state = settings().value("session/state").toByteArray();
        m_dockManager->restoreState(state);
        m_area = m_dockManager->dockArea(0);
        return ret;
    }
    

protected:

    ads::CDockWidget* addSubWindowInternal(QWidget* widget, const QString& title = QString());

    void closeEvent(QCloseEvent* ev) override;

    qint64 saveSession() {
        QSettings& setts = settings();
        QString value;
        
        QByteArray state = m_dockManager->saveState();
        settings().setValue("session/state", state);

        for (auto extension : plugins()->instances()) {
            if (extension->type() == Plugin::Type::WIDGET) {
                continue;
            }

            value += extension->name() + "-" + extension->uuid() + " ";
        }

        settings().setValue("session/plugins", value);
        
        return 0;
    }

signals:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

public slots:

    void createOrActivate() override;

    void showStatusMessage(const QString& msg, int timeout = 0) override;

private slots:
    void about();
    //void updateMenus();
    //void updateWindowMenu();
    void switchLayoutDirection();
    void createOrActivatePlugins();
    void createOrActivateInstances();

    //void subWindowActivated(QMdiSubWindow* window) {
    //    if (window) {
    //        m_current = window;
    //    }
    //}

private:

    bool createWindowByName(const QString& name);
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    
    //MdiChild *activeMdiChild() const;
    /*QMdiSubWindow**/ ads::CDockWidget* findChildWindow(const QString& name) const;
    
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
