// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>

#include "mainwindow.h"
#include "mdichild.h"

MainWindow::MainWindow()
    : m_mdiArea(new QMdiArea){
        
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_mdiArea);
    
    //connect(mdiArea, &QMdiArea::subWindowActivated,
    //        this, &MainWindow::updateMenus);

    createActions();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr(winTitle));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event){
    m_mdiArea->closeAllSubWindows();
    if (m_mdiArea->currentSubWindow()) {
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
    /*
    bool hasMdiChild = (activeMdiChild() != nullptr);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);

    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);
    */
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

        QString text;
        if (i < 9) {
            //text = tr("&%1 %2").arg(i + 1)
            //                   .arg(child->userFriendlyCurrentFile());
        } else {
            //text = tr("%1 %2").arg(i + 1)
            //                 .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = m_windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            m_mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
    }
}
#include "datawindow.h"
void MainWindow::createOrActivate(){
    qDebug() << "click" << QObject::sender() << qobject_cast<QAction*>(QObject::sender())->data();
    const QMetaObject *mo = QMetaType::metaObjectForType(QMetaType::type(qobject_cast<QAction*>(QObject::sender())->data().toByteArray()+"*"));
    QObject* child = mo->newInstance(Q_ARG(QWidget*,m_mdiArea),Q_ARG(QWidget*,this));
    QWidget* mchild = qobject_cast<QWidget*>(child);
    QMdiSubWindow *win = m_mdiArea->addSubWindow(mchild);
    mchild->show();
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    QAction *database = new QAction(tr("Database"), this);
    database->setData(QVariant("DataWindow"));
    connect(database, &QAction::triggered, this, &MainWindow::createOrActivate);
    fileMenu->addAction(database);
    
    fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);
    fileMenu->addSeparator();

//! [0]
    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::quit);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
//! [0]

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

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createStatusBar(){
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings(){
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings(){
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

MdiChild *MainWindow::activeMdiChild() const{
    if (QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow()){
        return qobject_cast<MdiChild*>(activeSubWindow->widget());
    }
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &key) const{
    const QList<QMdiSubWindow *> subWindows = m_mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        MdiChild *mdiChild = qobject_cast<MdiChild*>(window->widget());
        if (mdiChild->metaObject()->metaType().name() == key){
            return window;
        }
    }
    return nullptr;
}

void MainWindow::switchLayoutDirection(){
    if (layoutDirection() == Qt::LeftToRight){
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    } else {
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
    }
}
