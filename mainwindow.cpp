// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>

#include "logviewer.h"
#include "Instances.h"
#include "PluginList.h"
#include "mainwindow.h"
#include "api/api.h"
#include "main.h"

MainWindow::MainWindow(MLoader* plugins, Logger* log)
    : m_mdiArea(new QMdiArea)
    , m_logger(log)
    , m_settings("configuration.ini", QSettings::IniFormat)
    , m_plugins(plugins){

    log->message(QString("MainWindow::MainWindow: Using configuration: %1").arg(m_settings.fileName()));
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_mdiArea);
    
    connect(m_mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateMenus);

    createActions();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr(winTitle));
    setUnifiedTitleAndToolBarOnMac(true);

    connect(m_mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::subWindowActivated);
}

MainWindow::~MainWindow() {
    m_logger->message("MainWindow::~MainWindow", LoggerSeverity::LOG_DEBUG);
    saveSession();
}

MLoader* MainWindow::plugins() {
    return m_plugins;
}

Logger* MainWindow::logger(){
    return m_logger;
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
        event->accept();
    }
}

QSettings& MainWindow::settings(){
    return m_settings;
}  

MainWindow& MainWindow::operator<< (const QString& msg){
    statusBar()->showMessage(msg, statusTimeOut);
    return *this;
}

void MainWindow::about(){
   QMessageBox::about(this, tr("About"),
            tr(aboutText));
}

void MainWindow::updateMenus(){
    m_logger->message("MainWindow::updateMenus()", LoggerSeverity::LOG_DEBUG);
    bool hasMdiChild = (activeMdiChild() != nullptr);

    m_closeAct->setEnabled(hasMdiChild);
    m_closeAllAct->setEnabled(hasMdiChild);
    m_tileAct->setEnabled(hasMdiChild);
    m_cascadeAct->setEnabled(hasMdiChild);
    m_nextAct->setEnabled(hasMdiChild);
    m_previousAct->setEnabled(hasMdiChild);
    m_windowMenuSeparatorAct->setVisible(hasMdiChild);
}

void MainWindow::updateWindowMenu(){
    m_logger->message("MainWindow::updateWindowMenu()", LoggerSeverity::LOG_DEBUG);
    m_windowMenu->clear();
    m_windowMenu->addAction(m_closeAct);
    m_windowMenu->addAction(m_closeAllAct);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_tileAct);
    m_windowMenu->addAction(m_cascadeAct);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_nextAct);
    m_windowMenu->addAction(m_previousAct);
    m_windowMenu->addAction(m_windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = m_mdiArea->subWindowList();
    m_windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());
        QAction *action = m_windowMenu->addAction(mdiSubWindow->windowTitle(), mdiSubWindow, [this, mdiSubWindow]() {
            m_mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
    }
}

void MainWindow::createOrActivate(){
    m_logger->message(QString("MainWindow::createOrActivate()"));
    
    QMdiSubWindow *win = nullptr;
    QString name = qobject_cast<QAction*>(QObject::sender())->data().toString();

    if((win = findMdiChild(name))){
        m_logger->message(QString("MainWindow::activating subwindow: %1").arg(name));
        win->show();
        m_mdiArea->setActiveSubWindow(win);
        return;
    }
    
    m_logger->message("creating new instance: "+name);
    createWindowByName(name);
    
    updateWindowMenu();
}

bool MainWindow::createWindowByName(const QString& name){
    m_logger->message("MainWindow::createWindowByName("+name+")");
    if (plugins() == nullptr) {
        m_logger->message("MainWindow::createWindowByName: plugins are null");
        return false;
    }

    QSharedPointer<Plugin> plugin = plugins()->instance(name, this);
    QMdiSubWindow* win = nullptr;

    if (plugin.isNull()) {
        m_logger->message(QString("MainWindow::createWindowByName: failed to get plugin instance of %1").arg(name));
        QMessageBox::critical(this, tr(Main::appName),
            tr(fatalError),
            QMessageBox::Ok);
        return false;
    }

    MdiChild* child = plugin.dynamicCast<MdiChild>().data();
    if (child == nullptr) {
        m_logger->message("MainWindow::createWindowByName: plugin is not widget");
        return false;
    }

    if (!(win = m_mdiArea->addSubWindow(new MdiWindow(nullptr, child)))) {
        m_logger->message("MainWindow::createWindowByName: failed to add subwindow");
        //child->deleteLater();
        return false;
    }

    win->setWindowTitle(name);
    win->show();

    const QMetaObject* mu = child->metaObject();
    if (child->metaObject()->indexOfSlot(QMetaObject::normalizedSignature("prepareForFocus()")) != QSlotInvalid) {
        connect(win, SIGNAL(aboutToActivate()), child, SLOT(prepareForFocus()));
    }

    return true;
}

bool MainWindow::addSubWindow(QWidget* widget, const QString& title) {
    QMdiSubWindow* win = nullptr;
    m_logger->message(QString("MainWindow::addSubWindow(%1, %2)").arg((long long)widget,0,16).arg(title), LoggerSeverity::LOG_DEBUG);

    if (!(win = m_mdiArea->addSubWindow(widget))) {
        m_logger->message("MainWindow::addSubWindow: failed to add subwindow");
        return false;
    }

    win->setWindowTitle(title);
    win->show();

    return true;
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

    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(m_windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    m_closeAct = new QAction(tr("Cl&ose"), this);
    m_closeAct->setStatusTip(tr("Close the active window"));
    connect(m_closeAct, &QAction::triggered,
            m_mdiArea, &QMdiArea::closeActiveSubWindow);

    m_closeAllAct = new QAction(tr("Close &All"), this);
    m_closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(m_closeAllAct, &QAction::triggered, m_mdiArea, &QMdiArea::closeAllSubWindows);

    m_tileAct = new QAction(tr("&Tile"), this);
    m_tileAct->setStatusTip(tr("Tile the windows"));
    connect(m_tileAct, &QAction::triggered, m_mdiArea, &QMdiArea::tileSubWindows);

    m_cascadeAct = new QAction(tr("&Cascade"), this);
    m_cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(m_cascadeAct, &QAction::triggered, m_mdiArea, &QMdiArea::cascadeSubWindows);

    m_nextAct = new QAction(tr("Ne&xt"), this);
    m_nextAct->setShortcuts(QKeySequence::NextChild);
    m_nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(m_nextAct, &QAction::triggered, m_mdiArea, &QMdiArea::activateNextSubWindow);

    m_previousAct = new QAction(tr("Pre&vious"), this);
    m_previousAct->setShortcuts(QKeySequence::PreviousChild);
    m_previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(m_previousAct, &QAction::triggered, m_mdiArea, &QMdiArea::activatePreviousSubWindow);

    m_windowMenuSeparatorAct = new QAction(this);
    m_windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu* settingsMenu = menuBar()->addMenu(tr("&Settings"));
    QAction* pluginsList = new QAction(tr("Plugins"), this);
    pluginsList->setData(QVariant("PluginsList"));
    connect(pluginsList, &QAction::triggered, this, &MainWindow::createOrActivatePlugins);
    settingsMenu->addAction(pluginsList);

    QAction* instances = new QAction(tr("Instances"), this);
    instances->setData(QVariant("Instances"));
    connect(instances, &QAction::triggered, this, &MainWindow::createOrActivateInstances);
    settingsMenu->addAction(instances);

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createOrActivatePlugins() {
    QMdiSubWindow* win;
    m_logger->message("MainWindow::createOrActivatePlugins()", LoggerSeverity::LOG_DEBUG);

    if ((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger->message("MainWindow::createOrActivatePlugins: activating plugins subwindow", LoggerSeverity::LOG_DEBUG);
        m_mdiArea->setActiveSubWindow(win);
        return;
    }

    MdiChild *child = new PluginList(m_mdiArea, this, plugins());
    addSubWindow(child, tr("Plugins"));

    /*
    if (!(win = m_mdiArea->addSubWindow(child))) {
        m_logger->message("MainWindow::createOrActivatePlugins: failed to add subwindow");
        child->deleteLater();
        return;
    }

    win->setWindowTitle("Plugins");
    child->show();
    */
}

void MainWindow::createOrActivateInstances() {
    QMdiSubWindow* win;
    m_logger->message("MainWindow::createOrActivateInstances()");

    if ((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger->message("MainWindow::createOrActivateInstances: activating subwindow");
        m_mdiArea->setActiveSubWindow(win);
        return;
    }

    MdiChild* child = new Instances(m_mdiArea, this, plugins());
    if (child == nullptr) {
        m_logger->message("MainWindow::createOrActivateInstances: failed to create instance");
        return;
    }

    addSubWindow(child, tr("Instances"));

    /*if (!(win = m_mdiArea->addSubWindow(child))) {
        m_logger->message("MainWindow::createOrActivateInstances: failed to add subwindow");
        child->deleteLater();
        return;
    }

    win->setWindowTitle("Instances");
    child->show();
    */
}

/*
void MainWindow::createOrActivateSettings() {
    QMdiSubWindow* win;

    if ((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger.message("activating subwindow");
        m_mdiArea->setActiveSubWindow(win);
        return;
    }

    MdiChild* child = new SettingsDialog(this, plugins());
    if (!(win = m_mdiArea->addSubWindow(child))) {
        child->deleteLater();
        return;
    }

    win->setWindowTitle("Plugins");
    child->show();
}
*/

void MainWindow::createStatusBar(){
    m_logger->message("MainWindow::createStatusBar()", LoggerSeverity::LOG_DEBUG);
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings(){
    m_logger->message("MainWindow::readSettings()", LoggerSeverity::LOG_DEBUG);

    const QSettings &setts = settings();
    const QByteArray geometry = setts.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        m_logger->message("MainWindow::readSettings(): new geometry");
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        m_logger->message("MainWindow::readSettings(): restoring geometry");
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings(){
    m_logger->message("MainWindow::writeSettings()", LoggerSeverity::LOG_DEBUG);
    QSettings &setts = settings();
    setts.setValue("geometry", saveGeometry());
}

MdiChild *MainWindow::activeMdiChild() const{
    m_logger->message("MainWindow::activeMdiChild()", LoggerSeverity::LOG_DEBUG);
    if (QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow()){
        return qobject_cast<MdiChild*>(activeSubWindow->widget());
    }
    m_logger->message("MainWindow::activeMdiChild: no windows found", LoggerSeverity::LOG_DEBUG);
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &key) const{
    m_logger->message(QString("MainWindow::findMdiChild(%1)").arg(key));
    
    const QList<QMdiSubWindow*> subWindows = m_mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        MdiChild *mdiChild = qobject_cast<MdiChild*>(window->widget());
        Widget* plugin = qobject_cast<Widget*>(mdiChild);

        if (plugin == nullptr || plugin->name() != key) {
            continue;
        }

        if (plugin->name() == key) {
            m_logger->message("MainWindow::findMdiChild: child found");
            return window;
        }
    }
    m_logger->message("MainWindow::findMdiChild: child not found");
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

/*void MainWindow::openSerialPort() {
    const SettingsDialog::SerialSettings p(settings());
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    
    if (m_serial->open(QIODevice::ReadWrite)) {
        //m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_actionConnect->setEnabled(false);
        m_actionDisconnect->setEnabled(true);
        m_actionConfigure->setEnabled(false);
        
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
        .arg(p.name, QString::number(p.baudRate), QString::number(p.dataBits),
             QString::number(p.parity), QString::number(p.stopBits), QString::number(p.flowControl)), 0);
    } else {
        QMessageBox::critical(this, tr("Serial port open error"), m_serial->errorString());
        
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::writeSerial(const QString& msg) {
    if (!m_serial->isOpen()) {
        return;
    }

    qint64 len = m_serial->write(msg.toLocal8Bit());

    if (len != msg.size()) {
    
    }
}

void MainWindow::closeSerialPort(){
    if (m_serial->isOpen()) {
        m_serial->close();
    }
    //m_console->setEnabled(false);
    m_actionConnect->setEnabled(true);
    m_actionDisconnect->setEnabled(false);
    m_actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"), 0);
}
*/

void MainWindow::showStatusMessage(const QString &msg, int timeout){
    m_logger->message(QString("MainWindow::showStatusMessage(%1, %2)").arg(msg).arg(timeout), LoggerSeverity::LOG_DEBUG);
    QStatusBar* bar = statusBar();
    if (bar != nullptr) {
        bar->showMessage(msg, timeout);
    }
}