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

    static constexpr const char* const geometryKey = "geometry";
    static constexpr const char* const geometryValue = nullptr;
    static constexpr const char* const fontSizeKey = "fontSize";
    static constexpr const int fontSizeValue = -1;
    static constexpr const char* const statusTimeoutKey = "statusTimeout";
    static constexpr const int statusTimeoutValue = 2000;
    static constexpr const char* const translationsKey = "translations";
    static constexpr const bool translationsValue = false;
    static constexpr const char* const hideMenuKey = "hideMenu";
    static constexpr const bool hideMenuValue = false;

    MainWindowSettings()
        : geometry(geometryValue)
        , fontSize(fontSizeValue)
        , statusTimeout(statusTimeoutValue)
        , translations(translationsValue)
        , hideMenu(hideMenuValue){
    }

    MainWindowSettings(const QSettings& settings, const QString& settingsPath) 
        : geometry(settings.value(geometryKey, geometryValue).toByteArray())
        , fontSize(settings.value(fontSizeKey, fontSizeValue).toInt())
        , statusTimeout(settings.value(statusTimeoutKey, statusTimeoutValue).toInt())
        , translations(settings.value(translationsKey, translationsValue).toBool())
        , hideMenu(settings.value(hideMenuKey, hideMenuValue).toBool()){
    }

    void save(QSettings& settings, const QString& settingsPath) {
        settings.setValue(geometryKey, geometry);
        settings.setValue(fontSizeKey, fontSize);
        settings.setValue(statusTimeoutKey, statusTimeout);
        settings.setValue(translationsKey, translations);
        settings.setValue(hideMenuKey, hideMenu);
    }

    QByteArray geometry;
    int fontSize;
    int statusTimeout;
    bool translations;
    bool hideMenu;
};

class MainWindow : public Window {
    Q_OBJECT

    friend class Session;

signals:
    void settingsChanged();
    
public:
   
    MainWindow(MLoader* plugins = nullptr, Logger* logger = nullptr);

    ~MainWindow();

    QSettings& settings();  
    
    MainWindow& operator<< (const QString& msg);
    
    MLoader* plugins();

    void setPlugins(MLoader* loader);

    bool addSubWindow(QWidget* widget, const QString& title = QString()) override;

    bool addSubWindow(Widget* widget);

    Widget* find(const QString& uuid) const;

    bool toggleWindow(const QString& title);

protected:

    QByteArray state() const;

    void setState(const QByteArray& state);

    ads::CDockWidget* addSubWindowInternal(QWidget* widget, const QString& title = QString());

    void closeEvent(QCloseEvent* ev) override;

signals:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

    void aboutToQuit();

    void sessionStore();

public slots:

    void createOrActivate() override;

    void create() override;

    void showStatusMessage(const QString& msg, int timeout = 0) override;

    void createPreferences();

private slots:

    void about();

    void switchLayoutDirection();

    void subWindowClosed(QObject* ptr = nullptr);

private:

    std::optional<QString> windowTitleByInstance(const Widget* instance) const;

    int findFreeIndex(const QString& name) const;

    bool createWindowByName(const QString& name, bool newInstance = false);

    void createActions();
    
    void createStatusBar();
    
    void writeSettings();
    
    ads::CDockWidget* findChildWindow(const QString& name) const;
    
    static constexpr const char* winTitle = "Automatron 3000";
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
    MainWindowSettings m_winSettings;

};

#endif
