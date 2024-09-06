// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include <QWebEngineView>
#include <QRegularExpression>

#include "logviewer.h"
#include "Instances.h"
#include "PluginList.h"
#include "mainwindow.h"
#include "../api/api.h"
#include "main.h"

MainWindow::MainWindow(MLoader* plugins, Logger* logger)
    : m_settings("configuration.ini", QSettings::IniFormat)
    , m_plugins(plugins)
    , m_logger(logger)
    , m_winSettings(m_settings, "General") {

    m_logger->message(QString("MainWindow::MainWindow: Using configuration: %1").arg(m_settings.fileName()));
    
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);
    ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::AutoHideShowOnMouseOver, true);

    m_dockManager = new ads::CDockManager();
    setCentralWidget(m_dockManager);
    
    QLabel* l = new QLabel();

    //QWebEngineView* l = new QWebEngineView();
    //l->setUrl(QUrl("https://github.com/pr4u4t/szpuler"));

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* dockWidget = new ads::CDockWidget("Central Widget");
    dockWidget->setWidget(l);
    dockWidget->setFeature(ads::CDockWidget::NoTab, true);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetMovable, false);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    

    // Add the dock widget to the top dock widget area
    m_area = m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget);
    
    createActions();
    createStatusBar();

    if (m_winSettings.geometry.isEmpty()) {
        m_logger->message("MainWindow::MainWindow(): new geometry");
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
            (availableGeometry.height() - height()) / 2);
    } else {
        m_logger->message("MainWindow::MainWindow(): restoring geometry");
        restoreGeometry(m_winSettings.geometry);
    }

    if (m_winSettings.fontSize != -1) {
        QFont newFont(font());
        newFont.setPixelSize(m_winSettings.fontSize);
        qobject_cast<QApplication*>(QApplication::instance())->setFont(newFont, "QWidget");
    }

    if (m_winSettings.hideMenu == true) {
        menuBar()->hide();
    }

    setWindowTitle(tr(winTitle));
    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow() {
    m_logger->message("MainWindow::~MainWindow", LoggerSeverity::LOG_DEBUG);
}

MLoader* MainWindow::plugins() {
    return m_plugins;
}

void MainWindow::closeEvent(QCloseEvent *event){
    m_logger->message("MainWindow::closeEvent()", LoggerSeverity::LOG_DEBUG);

    QMessageBox::StandardButton resBtn = QMessageBox::question(this, /*APP_NAME*/"",
        tr("Are you sure?\n"),
        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        m_logger->message("MainWindow::closeEvent: event inored", LoggerSeverity::LOG_DEBUG);
        event->ignore();
    } else {
        m_logger->message("MainWindow::closeEvent: event accepted", LoggerSeverity::LOG_DEBUG);
        writeSettings();
        emit aboutToQuit();
        event->accept();
    }
}

QSettings& MainWindow::settings(){
    return m_settings;
}  

MainWindow& MainWindow::operator<<(const QString& msg){
    statusBar()->showMessage(msg, m_winSettings.statusTimeout);
    return *this;
}

void MainWindow::about(){
   QMessageBox::about(this, tr("About"),
            tr(aboutText));
}

void MainWindow::createOrActivate(){
    m_logger->message(QString("MainWindow::createOrActivate()"));
    
    ads::CDockWidget *win = nullptr;

    if (QObject::sender() == nullptr) {
        m_logger->message(QString("MainWindow::createOrActivate(): sender == nullptr"));
        return;
    }

    const QAction* sndr = qobject_cast<QAction*>(QObject::sender());

    if (sndr == nullptr) {
        m_logger->message(QString("MainWindow::createOrActivate(): sender != QAction"));
        return;
    }

    const QString name = qobject_cast<QAction*>(QObject::sender())->data().toString();

    if (name.isEmpty()) {
        m_logger->message(QString("MainWindow::createOrActivate(): plugin name empty"));
        return;
    }

    if((win = findChildWindow(name))){
        m_logger->message(QString("MainWindow::createOrActivate(): activating subwindow: %1").arg(name));
        win->toggleView(true);
        //m_dockManager->addDockWidget(ads::CenterDockWidgetArea, win, m_area);
        return;
    }
    
    m_logger->message("MainWindow::createOrActivate(): creating new instance: "+name);
    createWindowByName(name);
}

void MainWindow::create() {
    m_logger->message(QString("MainWindow::create()"));
    if (QObject::sender() == nullptr) {
        m_logger->message(QString("MainWindow::create(): sender == nullptr"));
        return;
    }

    const QAction* sndr = qobject_cast<QAction*>(QObject::sender());

    if (sndr == nullptr) {
        m_logger->message(QString("MainWindow::create: sender != QAction"));
        return;
    }

    const QString name = qobject_cast<QAction*>(QObject::sender())->data().toString();

    if (name.isEmpty()) {
        m_logger->message(QString("MainWindow::create: plugin name empty"));
        return;
    }
    m_logger->message("MainWindow::create: creating new instance: " + name);
    createWindowByName(name, true);
}

bool MainWindow::createWindowByName(const QString& name, bool newInstance){
    m_logger->message("MainWindow::createWindowByName("+name+")");
    
    if (plugins() == nullptr) {
        m_logger->message("MainWindow::createWindowByName: plugins are null");
        return false;
    }

    QSharedPointer<Plugin> plugin = newInstance ? plugins()->newInstance(name, this) : plugins()->instance(name, this);

    if (plugin.isNull()) {
        m_logger->message(QString("MainWindow::createWindowByName: failed to get plugin instance of %1").arg(name));
        //QMessageBox::critical(this, tr(Main::appName),
        //    tr(fatalError),
        //    QMessageBox::Ok);
        return false;
    }

    MdiChild* child = plugin.dynamicCast<MdiChild>().data();
    if (child == nullptr) {
        m_logger->message("MainWindow::createWindowByName: plugin is not widget");
        return false;
    }
    
    ads::CDockWidget* w = addSubWindowInternal(child, name);
    if (w == nullptr) {
        return false;
    }

    return true;
}

ads::CDockWidget* MainWindow::addSubWindowInternal(QWidget* widget, const QString& title) {
    m_logger->message(QString("MainWindow::addSubWindowInternal(%1, %2)").arg((long long)widget, 0, 16).arg(title), LoggerSeverity::LOG_DEBUG);

    const int idx = findFreeIndex(title);
    const QString newTitle = idx > 0 ? QString("%1 %2").arg(title).arg(idx) : title;

    ads::CDockWidget* child = new ads::CDockWidget(newTitle);
    if (child == nullptr) {
        m_logger->message("MainWindow::addSubWindow: failed to add subwindow");
        return nullptr;
    }
    
    child->setWidget(widget);
    child->setFeature(ads::CDockWidget::DontDeleteContent, true);
    m_dockManager->addDockWidget(ads::CenterDockWidgetArea, child, m_area);

    //const QMetaObject* mu = widget->metaObject();
    //if (child->metaObject()->indexOfSlot(QMetaObject::normalizedSignature("prepareForFocus()")) != QSlotInvalid) {
    //    connect(win, SIGNAL(aboutToActivate()), child, SLOT(prepareForFocus()));
    //}

    connect(widget, &QObject::destroyed, child, &ads::CDockWidget::close);

    return child;
}

int MainWindow::findFreeIndex(const QString& name) const {
    int ret = -1;
    QRegularExpression rx("[0-9]+$");
    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it : map) {
        auto match = rx.match(it->objectName());
        if (match.hasMatch() == false) {
            if (it->objectName().startsWith(name)) {
                ++ret;
            }
            continue;
        }

        const int i = match.captured().toInt();
        if (i > ret) {
            ret = i;
        }
    }

    return ++ret;
}

bool MainWindow::addSubWindow(QWidget* widget, const QString& title) {
    return addSubWindowInternal(widget, title) != nullptr;
}

void MainWindow::createActions(){
    m_logger->message("MainWindow::createActions()", LoggerSeverity::LOG_DEBUG);

    QMenu* fileMenu = findMenu(tr("&File"));
    if (fileMenu == nullptr) {
        fileMenu = menuBar()->addMenu(tr("&File"));
    }

    if (fileMenu) {
        fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);
        fileMenu->addSeparator();

        const QIcon exitIcon = QIcon::fromTheme("application-exit");
        QAction* exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::quit);
        exitAct->setShortcuts(QKeySequence::Quit);
        exitAct->setStatusTip(tr("Exit the application"));
        fileMenu->addAction(exitAct);
    }

    QMenu* settingsMenu = menuBar()->addMenu(tr("&Settings"));
    //QAction* pluginsList = new QAction(tr("Plugins"), this);
    //pluginsList->setData(QVariant("PluginsList"));
    //connect(pluginsList, &QAction::triggered, this, &MainWindow::createOrActivatePlugins);
    //settingsMenu->addAction(pluginsList);

    QAction* preferences = new QAction(tr("Preferences"), this);
    //instances->setData(QVariant("Instances"));
    connect(preferences, &QAction::triggered, this, &MainWindow::createPreferences);
    settingsMenu->addAction(preferences);

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createPreferences() {
    m_logger->message(QString("MainWindow::createPreferences()"));

    ads::CDockWidget* win = nullptr;

    if ((win = findChildWindow("Preferences"))) {
        m_logger->message(QString("MainWindow::activating subwindow: %1").arg("Preferences"));
        win->toggleView(true);
        //m_dockManager->addDockWidget(ads::CenterDockWidgetArea, win, m_area);
        return;
    }

    m_logger->message("creating new instance: Preferences");
    //createWindowByName(name);
}

/*
void MainWindow::createOrActivatePlugins() {
    ads::CDockWidget* win;
    m_logger->message("MainWindow::createOrActivatePlugins()", LoggerSeverity::LOG_DEBUG);

    if ((win = findChildWindow(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger->message("MainWindow::createOrActivatePlugins: activating plugins subwindow", LoggerSeverity::LOG_DEBUG);
        m_dockManager->addDockWidget(ads::CenterDockWidgetArea, win, m_area);
        return;
    }

    MdiChild *child = new PluginList(m_dockManager, this, plugins());
    addSubWindow(child, "Plugins");
}
*/

/*
void MainWindow::createOrActivateInstances() {
    ads::CDockWidget* win = nullptr;
    m_logger->message("MainWindow::createOrActivateInstances()");

    if ((win = findChildWindow(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger->message("MainWindow::createOrActivateInstances: activating subwindow");
        m_dockManager->addDockWidget(ads::CenterDockWidgetArea, win, m_area);
        return;
    }

    MdiChild* child = new Instances(m_dockManager, this, plugins());
    if (child == nullptr) {
        m_logger->message("MainWindow::createOrActivateInstances: failed to create instance");
        return;
    }

    addSubWindow(child, "Instances");
}
*/

void MainWindow::setPlugins(MLoader* loader) {
    m_plugins = loader;
}

QByteArray MainWindow::state() const {
    return m_dockManager != nullptr ? m_dockManager->saveState() : QByteArray();
}

void MainWindow::setState(const QByteArray& state) {
    if (m_dockManager != nullptr) {
        m_dockManager->restoreState(state);
        m_area = m_dockManager->dockArea(0);
    }
}

void MainWindow::createStatusBar(){
    m_logger->message("MainWindow::createStatusBar()", LoggerSeverity::LOG_DEBUG);
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::writeSettings(){
    m_logger->message("MainWindow::writeSettings()", LoggerSeverity::LOG_DEBUG);
    QSettings &setts = settings();
    setts.setValue("geometry", saveGeometry());
}

ads::CDockWidget* MainWindow::findChildWindow(const QString& name) const {
    m_logger->message(QString("MainWindow::findMdiChild(%1)").arg(name));
    
    //ads::CDockWidget* ret = m_dockManager->findDockWidget(name);
    ads::CDockWidget* ret = nullptr;

    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it = map.cbegin(), end = map.cend(); it != end; ++it) {
        const QString k = it.key();
        QWidget* w = it.value()->widget();
        const Widget* child = qobject_cast<Widget*>(w);
        if (child == nullptr) {
            continue;
        }

        if (child->name() == name) {
            ret = it.value();
            break;
        }
    }

    if (ret != nullptr) {
        m_logger->message("MainWindow::findChildWindow: child found");
        return ret;
    }

    m_logger->message("MainWindow::findChildWindow: child not found");
    return nullptr;
}

void MainWindow::switchLayoutDirection(){
    m_logger->message(QString("MainWindow::switchLayoutDirection()"), LoggerSeverity::LOG_DEBUG);
    
    if (layoutDirection() == Qt::LeftToRight){
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    } else {
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
    }
}

void MainWindow::showStatusMessage(const QString &msg, int timeout){
    m_logger->message(QString("MainWindow::showStatusMessage(%1, %2)").arg(msg).arg(timeout), LoggerSeverity::LOG_DEBUG);
    QStatusBar* bar = statusBar();
    if (bar != nullptr) {
        bar->showMessage(msg, timeout);
    }
}