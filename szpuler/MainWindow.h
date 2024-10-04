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
#include <QToolBar>
#include <QComboBox>
#include <QWidgetAction>
#include <QInputDialog>
#include <QMessageBox>

#include "../api/api.h"
#include "../core/core.h"
#include "MdiWindow.h"

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "Session.h"
#include "DockComponentsFactory.h"
#include "DockWidgetTab.h"
#include "DockWidget.h"

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
    static constexpr const char* const lockKey = "lock";
    static constexpr const bool lockValue = false;
    static constexpr const char* const kioskKey = "kiosk";
    static constexpr const bool kioskValue = false;

    MainWindowSettings()
        : geometry(geometryValue)
        , fontSize(fontSizeValue)
        , statusTimeout(statusTimeoutValue)
        , translations(translationsValue)
        , hideMenu(hideMenuValue)
        , lock(lockValue)
        , kiosk(kioskValue){
    }

    MainWindowSettings(const QSettings& settings, const QString& settingsPath) 
        : geometry(settings.value(geometryKey, geometryValue).toByteArray())
        , fontSize(settings.value(fontSizeKey, fontSizeValue).toInt())
        , statusTimeout(settings.value(statusTimeoutKey, statusTimeoutValue).toInt())
        , translations(settings.value(translationsKey, translationsValue).toBool())
        , hideMenu(settings.value(hideMenuKey, hideMenuValue).toBool())
        , lock(settings.value(lockKey, lockValue).toBool())
        , kiosk(settings.value(kioskKey, kioskValue).toBool()){
    }

    void save(QSettings& settings, const QString& settingsPath) {
        settings.setValue(geometryKey, geometry);
        settings.setValue(fontSizeKey, fontSize);
        settings.setValue(statusTimeoutKey, statusTimeout);
        settings.setValue(translationsKey, translations);
        settings.setValue(hideMenuKey, hideMenu);
        settings.setValue(lockKey, lock);
        settings.setValue(kioskKey, kiosk);
    }

    QByteArray geometry;
    int fontSize;
    int statusTimeout;
    bool translations;
    bool hideMenu;
    bool lock;
    bool kiosk;
};

class MainWindow : public Window {
    Q_OBJECT

    friend class Session;

signals:
    void settingsChanged();
    
public:
   
    MainWindow(MLoader* plugins = nullptr, Logger* logger = nullptr);

    ~MainWindow(); 
    
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

    void savePerspectives(){
        QSettings set = Settings::get();
        m_dockManager->savePerspectives(set);
    }

    void restorePerspectives(){
        QSettings set = Settings::get();
        m_dockManager->loadPerspectives(set);
        m_perspectiveComboBox->clear();
        m_perspectiveComboBox->addItems(m_dockManager->perspectiveNames());
    }

    void toggleToolbar() {
        if (m_tbar->isHidden()) {
            m_tbar->show();
        } else {
            m_tbar->hide();
        }
    }

    void lockWorkspace(bool value){
        if (value) {
            m_dockManager->lockDockWidgetFeaturesGlobally();
        } else {
            m_dockManager->lockDockWidgetFeaturesGlobally(ads::CDockWidget::NoDockWidgetFeatures);
        }
    }

    void newPerspective(const QString& name){
        QString perspectiveName = name;

        if (perspectiveName.isEmpty()) {
            perspectiveName = QInputDialog::getText(this, tr("Save Perspective"), tr("Enter unique name:"));
            if (perspectiveName.isEmpty()) {
                return;
            }
        }

        if (m_dockManager->perspectiveNames().contains(perspectiveName) == true) {
            QMessageBox::critical(this, tr("Error perspective already exists"), tr("Perspective %1 already exists").arg(perspectiveName));
            return;
        }

        m_dockManager->addPerspective(perspectiveName);
        QSignalBlocker Blocker(m_perspectiveComboBox);
        m_perspectiveComboBox->clear();
        m_perspectiveComboBox->addItems(m_dockManager->perspectiveNames());
        m_perspectiveComboBox->setCurrentText(perspectiveName);

        savePerspectives();
    }

    void newPerspective(bool checked = false) {
        QString perspectiveName = QInputDialog::getText(this, tr("Save Perspective"), tr("Enter unique name:"));
        if (perspectiveName.isEmpty()) {
            return;
        }

        if (m_dockManager->perspectiveNames().contains(perspectiveName) == true) {
            QMessageBox::critical(this, tr("Error perspective already exists"), tr("Perspective %1 already exists").arg(perspectiveName));
            return;
        }

        m_dockManager->addPerspective(perspectiveName);
        QSignalBlocker Blocker(m_perspectiveComboBox);
        m_perspectiveComboBox->clear();
        m_perspectiveComboBox->addItems(m_dockManager->perspectiveNames());
        m_perspectiveComboBox->setCurrentText(perspectiveName);

        savePerspectives();
    }

    void savePerspective(bool checked = false) {
        QString perspectiveName = m_perspectiveComboBox->currentText();
        m_dockManager->addPerspective(perspectiveName);
        savePerspectives();
    }

private:

    std::optional<QString> windowTitleByInstance(const Widget* instance) const;

    int findFreeIndex(const QString& name) const;

    bool createWindowByName(const QString& name, bool newInstance = false);

    void createActions();
    
    void createStatusBar();
    
    void writeSettings();
    
    void createToolbar();

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
    QAction *m_actionConnect = nullptr;
    QAction *m_actionDisconnect = nullptr;
    QAction *m_actionConfigure = nullptr;
    MLoader* m_plugins = nullptr;
    //QMdiSubWindow* m_current = nullptr;
    Logger* m_logger = nullptr;
    MainWindowSettings m_winSettings;
    QToolBar* m_tbar = nullptr;
    QComboBox* m_perspectiveComboBox = nullptr;
    QWidgetAction* m_perspectiveListAction = nullptr;
    QMenu* m_viewMenu = nullptr;
    QAction* m_toggleToolbar = nullptr;
    QAction* m_saveState = nullptr;
    QAction* m_restoreState = nullptr;
    QAction* m_newPerspective = nullptr;
};

class CustomDockWidgetTab : public ads::CDockWidgetTab {

    Q_OBJECT

public:
    CustomDockWidgetTab(ads::CDockWidget* DockWidget, MainWindow* win)
        : CDockWidgetTab(DockWidget)
        , m_win(win){
    }

protected:
    void fillMenu(QMenu* Menu) const {
        ads::CDockWidgetTab::fillMenu(Menu);
        Plugin* plugin = dynamic_cast<Plugin*>(dockWidget()->widget());
        if (plugin != nullptr) {
            QObject* o = dynamic_cast<QObject*>(plugin);
            const QList<QAction*> actions = Menu->actions();
            QAction* settings = new QAction(tr("Settings"));
            Menu->insertAction(actions[actions.size() - 1], settings);
            Menu->insertSeparator(actions[actions.size() - 1]);
            QObject* w = dockAreaWidget()->parent();

            QObject::connect(settings, &QAction::triggered, [plugin, o, this]() {
                SettingsMdi* settings = plugin->settingsWindow();
                this->m_win->addSubWindow(settings, o->objectName() + "/Settings");

            });
        }
    }
private:

    MainWindow* m_win = nullptr;
};

#endif
