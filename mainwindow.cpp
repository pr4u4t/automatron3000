// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>

#include "logviewer.h"
#include "Instances.h"
#include "PluginList.h"
#include "mainwindow.h"
#include "api/api.h"
#include "main.h"

MainWindow::MainWindow(MLoader* plugins)
    : m_mdiArea(new QMdiArea),
    m_logger(logPath,this),
    m_settings("configuration.ini", QSettings::IniFormat),
    m_plugins(plugins){
    qDebug().noquote() << "Using configuration: " << m_settings.fileName();
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
    
   // if(settings().value(SettingsDialog::autoConnectKey, SettingsDialog::autoConnectValue).toBool()){
   //     openSerialPort();
   // }
}

MainWindow::~MainWindow() {
    saveSession();
}

MLoader* MainWindow::plugins() {
    return m_plugins;
}

Logger* MainWindow::logger(){
    return &m_logger;
}

void MainWindow::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, /*APP_NAME*/"",
        tr("Are you sure?\n"),
        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
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
    m_logger.message("MainWindow::createOrActivate()");
    
    QMdiSubWindow *win;
    
    if((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))){
        m_logger.message("activating subwindow");
        m_mdiArea->setActiveSubWindow(win);
        return;
    }
    
    m_logger.message("creating new instance");
    QString name = qobject_cast<QAction*>(QObject::sender())->data().toString();
    
    createWindowByName(name);
    
    updateWindowMenu();
}

bool MainWindow::createWindowByName(const QString& name){
    if (plugins() == nullptr) {
        return false;
    }

    QSharedPointer<Plugin> plugin = plugins()->instance(name, this);
    QMdiSubWindow* win = nullptr;

    if (plugin.isNull()) {
        m_logger.message("failed to get plugin instance of " + name);
        QMessageBox::critical(this, tr(Main::appName),
            tr(fatalError),
            QMessageBox::Ok);
        return false;
    }

    MdiChild* child = plugin.dynamicCast<MdiChild>().data();
    if (child == nullptr) {
        return false;
    }

    if (!(win = m_mdiArea->addSubWindow(child))) {
        //child->deleteLater();
        return false;
    }

    win->setWindowTitle(name);
    win->show();

    const QMetaObject* mu = child->metaObject();
    if (child->metaObject()->indexOfSlot(QMetaObject::normalizedSignature("prepareForFocus()")) != QSlotInvalid) {
        connect(win, SIGNAL(aboutToActivate()), child, SLOT(prepareForFocus()));
    }

    /*
    QMdiSubWindow* win;
    const QMetaObject *mo = QMetaType::metaObjectForType(QMetaType::type((name+"*").toLocal8Bit()));
    if(mo == nullptr){
        m_logger.message("failed to get metaobject for "+name);
        QMessageBox::critical(this, tr(Main::appName),
                              tr(fatalError),
                              QMessageBox::Ok);
        return false;
    }
    
    QObject* child = mo->newInstance(Q_ARG(QWidget*,m_mdiArea),Q_ARG(QWidget*,this));
    if(child == nullptr){
        m_logger.message("failed to create instance");
        QMessageBox::critical(this, tr(Main::appName),
                              tr(fatalError),
                              QMessageBox::Ok);
        return false;
    }
    
    MdiChild* mdi = qobject_cast<MdiChild*>(child);
    connect(mdi, &MdiChild::logMessage, &m_logger, &Logger::message);
    QWidget* mchild = qobject_cast<QWidget*>(child);
    if (!(win = m_mdiArea->addSubWindow(mchild))) {
        child->deleteLater();
        return false;
    }
    const QMetaObject* mu = mdi->metaObject();
    if (mdi->metaObject()->indexOfSlot(QMetaObject::normalizedSignature("prepareForFocus()")) != QSlotInvalid) {
        connect(win, SIGNAL(aboutToActivate()), child, SLOT(prepareForFocus()));
    }

    win->setWindowTitle(name);
    mchild->show();
    */

    return true;
}

bool MainWindow::addSubWindow(QWidget* widget) {
    QMdiSubWindow* win = nullptr;

    if (!(win = m_mdiArea->addSubWindow(widget))) {
        //child->deleteLater();
        return false;
    }

    win->setWindowTitle("");
    win->show();

    return true;
}

void MainWindow::createActions(){
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    //QToolBar *fileToolBar = addToolBar(tr("File"));

    QAction *logviewer = new QAction(tr("Log Viewer"), this);
    logviewer->setData(QVariant("LogViewer"));
    connect(logviewer, &QAction::triggered, this, &MainWindow::createOrActivateLogViewer);
    fileMenu->addAction(logviewer);
    
    fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);
    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::quit);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
    
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
    
    if ((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger.message("activating subwindow");
        m_mdiArea->setActiveSubWindow(win);
        return;
    }

    MdiChild *child = new PluginList(m_mdiArea, this, plugins());
    if (!(win = m_mdiArea->addSubWindow(child))) {
        child->deleteLater();
        return;
    }

    win->setWindowTitle("Plugins");
    child->show();
}

void MainWindow::createOrActivateInstances() {
    QMdiSubWindow* win;

    if ((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger.message("activating subwindow");
        m_mdiArea->setActiveSubWindow(win);
        return;
    }

    MdiChild* child = new Instances(m_mdiArea, this, plugins());
    if (!(win = m_mdiArea->addSubWindow(child))) {
        child->deleteLater();
        return;
    }

    win->setWindowTitle("Instances");
    child->show();
}

void MainWindow::createOrActivateLogViewer() {
    QMdiSubWindow* win;

    if ((win = findMdiChild(qobject_cast<QAction*>(QObject::sender())->data().toString()))) {
        m_logger.message("activating subwindow");
        m_mdiArea->setActiveSubWindow(win);
        return;
    }

    MdiChild* child = new LogViewer(m_mdiArea, this);
    if (!(win = m_mdiArea->addSubWindow(child))) {
        child->deleteLater();
        return;
    }

    win->setWindowTitle("LogViewer");
    child->show();
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
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings(){
    const QSettings &setts = settings();
    const QByteArray geometry = setts.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
    
    /*
    QStringList wins = setts.value("windows", QString()).toString().split(",");
    
    if(wins[0].isEmpty()){
        return;
    }
    
    for ( const auto& win : wins ){
        qDebug() << win;
        createWindowByName(win);
    }
    */
}

void MainWindow::writeSettings(){
    QSettings &setts = settings();
    setts.setValue("geometry", saveGeometry());
    //setts.setValue("windows",subWindows().join(","));
}

MdiChild *MainWindow::activeMdiChild() const{
    m_logger.message("activeMdiChild()");
    if (QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow()){
        return qobject_cast<MdiChild*>(activeSubWindow->widget());
    }
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &key) const{
    m_logger.message("findMdiChild("+key+")");
    
    const QList<QMdiSubWindow *> subWindows = m_mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        MdiChild *mdiChild = qobject_cast<MdiChild*>(window->widget());
        Plugin* plugin = mdiChild->plugin();

        if (plugin == nullptr || plugin->name() != key) {
            continue;
        }

        if (plugin->name() == key) {
            return window;
        }
        //if (mdiChild->metaObject()->metaType().name() == key){
        //    m_logger.message(QString("child found: ")+mdiChild->metaObject()->metaType().name());
        //    return window;
        //}
    }
    m_logger.message("child not found");
    return nullptr;
}

void MainWindow::switchLayoutDirection(){
    m_logger.message("switchLayoutDirection");
    
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
    statusBar()->showMessage(msg, timeout);
}

//void MainWindow::showWriteError(const QString &message){
//   QMessageBox::warning(this, tr("Warning"), message);
//}

/*
QStringList MainWindow::subWindows() const {
    QStringList ret;
    const QList<QMdiSubWindow *> subWindows = m_mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        MdiChild *mdiChild = qobject_cast<MdiChild*>(window->widget());
        ret << mdiChild->metaObject()->metaType().name();
        qDebug() << mdiChild->metaObject()->metaType().name();
    }
    qDebug() << ret;
    return ret;
}
*/