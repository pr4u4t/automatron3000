// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include <QWebEngineView>
#include <QRegularExpression>
#include <QVariant>

#include "logviewer.h"
#include "Instances.h"
#include "Plugins.h"
#include "Preferences.h"
#include "mainwindow.h"
#include "../api/api.h"
#include "main.h"

static QIcon svgIcon(const QString& File){
    // This is a workaround, because in item views SVG icons are not
    // properly scaled and look blurry or pixelate
    QIcon SvgIcon(File);
    SvgIcon.addPixmap(SvgIcon.pixmap(92));
    return SvgIcon;
}

class CCustomComponentsFactory : public ads::CDockComponentsFactory {
public:
    using Super = ads::CDockComponentsFactory;

    CCustomComponentsFactory(MainWindow* win)
        : m_win(win) {}

    ads::CDockWidgetTab* createDockWidgetTab(ads::CDockWidget* DockWidget) const{
        return new CustomDockWidgetTab(DockWidget, m_win);
    }

private:

    MainWindow* m_win = nullptr;
};

MainWindow::MainWindow(const QSettings& settings, const QString& settingsPath, MLoader* plugins, Logger* logger)
    : m_plugins(plugins)
    , m_logger(logger)
    , m_winSettings(settings, "Window")
    , m_tbar(new QToolBar())
    , m_perspectiveComboBox(new QComboBox())
    , m_perspectiveListAction(new QWidgetAction(this))
    , m_loading(this, logger){

    m_logger->message(QString("MainWindow::MainWindow: Using configuration: %1").arg(Settings::settingsPath()));
    
    if (m_winSettings.kiosk) {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    }

    ads::CDockComponentsFactory::setFactory(new CCustomComponentsFactory(this));

    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::XmlCompressionEnabled, false);
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);
    ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::AutoHideShowOnMouseOver, true);

    m_dockManager = new ads::CDockManager(this);
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
    //m_area = m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget);
    
    createActions();
    createStatusBar();
    createToolbar();

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

    //if (m_winSettings.lock == true) {
    //    //QTimer::singleShot(0, m_dockManager, &ads::CDockManager::lockDockWidgetFeaturesGlobally);
    //    m_dockManager->lockDockWidgetFeaturesGlobally();
    //}
    addToolBar(m_tbar);

    connect(m_dockManager, &ads::CDockManager::openingPerspective, this, &MainWindow::startPerspectiveChange);
    connect(m_dockManager, &ads::CDockManager::perspectiveOpened, this, &MainWindow::endPerspectiveChange);
}

MainWindow::~MainWindow() {
    m_logger->message("MainWindow::~MainWindow", LoggerSeverity::LOG_DEBUG);
    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it : map) {
        disconnect(it, &ads::CDockWidget::destroyed, this, &MainWindow::subWindowClosed);
    }
}

MLoader* MainWindow::plugins() {
    return m_plugins;
}

void MainWindow::closeEvent(QCloseEvent *event){
    m_logger->message("MainWindow::closeEvent()", LoggerSeverity::LOG_DEBUG);
    QSettings set = Settings::get();

    QMessageBox::StandardButton resBtn = QMessageBox::question(this, /*APP_NAME*/"",
        tr("Save current session?\n"),
        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
    switch (resBtn) {
    case QMessageBox::Yes:
        m_logger->message("MainWindow::closeEvent: event accepted", LoggerSeverity::LOG_DEBUG);
        m_winSettings.geometry = saveGeometry();
        m_winSettings.perspective = m_perspectiveComboBox->currentText();
        m_winSettings.save(set, "Window");
        //writeSettings(); /TODO: save settings
        emit aboutToQuit(MainWindowQuit::QUIT_SAVE);
        event->accept();
        break;
    case QMessageBox::No:
        emit aboutToQuit(MainWindowQuit::QUIT_WITHOUT_SAVE);
        event->accept();
        break;
    case QMessageBox::Cancel:
        m_logger->message("MainWindow::closeEvent: event ignored", LoggerSeverity::LOG_DEBUG);
        event->ignore();
        break;
    }
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
    
    Widget* w = plugin.dynamicCast<Widget>().data();

    return addSubWindow(w);
}

ads::CDockWidget* MainWindow::addSubWindowInternal(QWidget* widget, const QString& title) {
    m_logger->message(QString("MainWindow::addSubWindowInternal(%1, %2)").arg((long long)widget, 0, 16).arg(title), LoggerSeverity::LOG_DEBUG);

    ads::CDockWidget* child = new ads::CDockWidget(title);
    if (child == nullptr) {
        m_logger->message("MainWindow::addSubWindow: failed to add subwindow");
        return nullptr;
    }
    
    child->setWidget(widget);
    child->setFeature(ads::CDockWidget::DontDeleteContent, true);
    child->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    if (m_winSettings.lock == true) {
        child->setFeature(ads::CDockWidget::NoTab, true);
        child->setFeature(ads::CDockWidget::DockWidgetMovable, false);
        child->setFeature(ads::CDockWidget::DockWidgetClosable, false);
        child->setFeature(ads::CDockWidget::DockWidgetFloatable, false);
        child->setFeature(ads::CDockWidget::DockWidgetPinnable, false);
        
    }

    child->setFeature(ads::CDockWidget::CustomCloseHandling, true);

    QObject::connect(child, &ads::CDockWidget::closeRequested, [child, this]() {
        QWidget* widget = child->widget();
        
        if (qobject_cast<Widget*>(widget) != nullptr) {
            child->toggleView(false); 
        } else {
            child->deleteDockWidget();
        }
    });
    
    if (title.endsWith("Settings")) {
        QStringList split = title.split("/");
        const auto tmp = findChildWindowByTitle(split[0]);
        m_dockManager->addDockWidget(ads::CenterDockWidgetArea, child, (tmp == nullptr) ? m_area : tmp->dockAreaWidget());

    } else {
        m_dockManager->addDockWidget(ads::CenterDockWidgetArea, child, m_area);
    }

    //const QMetaObject* mu = widget->metaObject();
    //if (child->metaObject()->indexOfSlot(QMetaObject::normalizedSignature("prepareForFocus()")) != QSlotInvalid) {
    //    connect(win, SIGNAL(aboutToActivate()), child, SLOT(prepareForFocus()));
    //}
    connect(child, &ads::CDockWidget::destroyed, this, &MainWindow::subWindowClosed);
    connect(widget, &QObject::destroyed, child, &ads::CDockWidget::close);
    QAction* tmp = m_windowMenu->addAction(title, this, [child]() {
        child->toggleView();
    });
    tmp->setData(QVariant::fromValue(child));
    return child;
}

void MainWindow::subWindowClosed(QObject* ptr) {
    const QString name = ptr->objectName();
    QList<QAction*> actions = m_windowMenu->actions();

    for (auto act : actions) {
        const QVariant var = act->data();
        const ads::CDockWidget* child = var.value<ads::CDockWidget*>();
        
        if (child->objectName() == name) {
            m_windowMenu->removeAction(act);
        }
    }
}

std::optional<QString> MainWindow::windowTitleByInstance(const Widget* instance) const {
    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it : map) {
        if (it->widget() == static_cast<const QWidget*>(instance)) {
            return std::optional<QString>(it->objectName());
        }
    }

    return std::nullopt;
}

int MainWindow::findFreeIndex(const QString& name) const {
    int ret = -1;
    QRegularExpression rx(name+" [0-9]+$");
    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it : map) {
        auto match = rx.match(it->objectName());
        if (match.hasMatch() == false) {
            if (it->objectName() == name) {
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

Widget* MainWindow::find(const QString& uuid) const {
    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (const auto it : map) { 
        Widget* w = dynamic_cast<Widget*>(it->widget());
        if (w == nullptr) {
            continue;
        }
        
        if (w->uuid() == uuid) {
            return w;
        }
    }

    return nullptr;
}

bool MainWindow::addSubWindow(Widget* widget) {
    return addSubWindowInternal(widget, widget->objectName()) != nullptr;
}

bool MainWindow::addSubWindow(QWidget* widget, const QString& title) {
    const int idx = findFreeIndex(title);
    return addSubWindowInternal(widget, idx > 0 ? QString("%1 %2").arg(title).arg(idx) : title) != nullptr;
}

void MainWindow::createActions(){
    m_logger->message("MainWindow::createActions()", LoggerSeverity::LOG_DEBUG);

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    if (fileMenu) {
        fileMenu->addAction(tr("Save session"), this, &MainWindow::storeSettings);

        fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);
        fileMenu->addSeparator();

        const QIcon exitIcon = QIcon::fromTheme("application-exit");
        QAction* exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::quit);
        exitAct->setShortcuts(QKeySequence::Quit);
        exitAct->setStatusTip(tr("Exit the application"));
        fileMenu->addAction(exitAct);
    }

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_toggleToolbar = new QAction(tr("Toggle toolbar"), this);
    connect(m_toggleToolbar, &QAction::triggered, this, &MainWindow::toggleToolbar);
    m_viewMenu->addAction(m_toggleToolbar);

    m_windowMenu = menuBar()->addMenu(tr("&Window"));

    m_visualMenu = menuBar()->addMenu(tr("V&isualization"));
    //m_visualMenu->addAction(tr("(empty)"));

    menuBar()->addMenu(tr("Input/Output"));

    menuBar()->addMenu(tr("Actions"));

    menuBar()->addMenu(tr("Data"));

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

    QAction* help = helpMenu->addAction(tr("Documentation"), this, &MainWindow::help);
}

void MainWindow::help() {
    HelpBrowser* hb = new HelpBrowser();
    hb->setAttribute(Qt::WA_DeleteOnClose);
    hb->show();

}

bool MainWindow::toggleWindow(const QString& title) {
    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it = map.cbegin(), end = map.cend(); it != end; ++it) {
        if (it.value()->objectName() == title) {
            it.value()->toggleView();
            return true;
        }
    }

    return false;
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
    Preferences* pref = new Preferences(this, nullptr, QString(), plugins());
    
    ads::CDockWidget* child = new ads::CDockWidget(tr("Preferences"));
    if (child == nullptr) {
        m_logger->message("MainWindow::addSubWindow: failed to add subwindow");
        return;
    }

    child->setWidget(pref);
    m_dockManager->addDockWidgetFloating(child);
}

void MainWindow::setPlugins(MLoader* loader) {
    m_plugins = loader;
}

bool MainWindow::openPerspective(const QString& state) {
    m_logger->message("MainWindow::openPerspective(const QString& state)", LoggerSeverity::LOG_DEBUG);
    if (!m_dockManager->hasPerspective(state)) {
        m_logger->message(QString("MainWindow::openPerspective: perspective %1 does not exist").arg(state), LoggerSeverity::LOG_DEBUG);
        return false;
    }
    m_perspectiveComboBox->setCurrentText(state);
    m_dockManager->openPerspective(state);
    return true;
}

void MainWindow::sessionRestored() {
    m_logger->message("MainWindow::sessionRestored()", LoggerSeverity::LOG_DEBUG);
    openPerspective(m_winSettings.perspective);
}

void MainWindow::createStatusBar(){
    m_logger->message("MainWindow::createStatusBar()", LoggerSeverity::LOG_DEBUG);
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createToolbar() {
    m_logger->message("MainWindow::createToolbar()", LoggerSeverity::LOG_DEBUG);
    m_perspectiveComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_perspectiveComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_perspectiveListAction->setDefaultWidget(m_perspectiveComboBox);
    m_tbar->addAction(m_perspectiveListAction);

    if (Settings::get().value("perspectiveManage", true).toBool() == true) {
        m_saveState = new QAction(tr("Save workspace"));
        m_tbar->addAction(m_saveState);
        m_tbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_saveState->setIcon(svgIcon(":/images/save.svg"));
        QObject::connect(m_saveState, &QAction::triggered, this, &MainWindow::savePerspective);

        //m_restoreState = new QAction(tr("Restore state"));
        //m_tbar->addAction(m_restoreState);
        //m_restoreState->setIcon(svgIcon(":/images/restore.svg"));

        m_tbar->addSeparator();

        QAction* a = m_tbar->addAction(tr("Lock Workspace"));
        a->setIcon(svgIcon(":/images/lock_outline.svg"));
        a->setCheckable(true);
        a->setChecked(false);
        QObject::connect(a, &QAction::triggered, this, &MainWindow::lockWorkspace);

        m_newPerspective = new QAction(tr("Create workspace"));
        m_newPerspective->setIcon(svgIcon(":/images/picture_in_picture.svg"));
        QObject::connect(m_newPerspective, &QAction::triggered, this, [this](bool checked = false) {
            this->newPerspective(checked);
        });
        m_tbar->addAction(m_newPerspective);

        m_copyPerspective = new QAction(tr("Copy workspace"));
        m_copyPerspective->setIcon(svgIcon(":/images/note_add.svg"));
        QObject::connect(m_copyPerspective, &QAction::triggered, this, [this](bool checked = false) {
            this->copyPerspective(this->m_currentPerspective);
        });
        m_tbar->addAction(m_copyPerspective);

        m_deletePerspective = new QAction(tr("Delete workspace"));
        m_deletePerspective->setIcon(svgIcon(":/images/edit.svg"));
        QObject::connect(m_deletePerspective, &QAction::triggered, this, [this](bool checked = false) {
            DeleteWorkspaceDialog dw(this, this->m_dockManager->perspectiveNames());
            if (dw.exec() != QDialog::Accepted) {
                emit message("MainWindow::deleteWorkspace: name dialog discarded");
                return;
            }

            const auto deletion = dw.selectedOption();
            if (this->m_currentPerspective == deletion) {
                emit message("MainWindow::deleteWorkspace: cannot delete current workspace");
                return;
            }

            this->deletePerspective(deletion);
        });
        m_tbar->addAction(m_deletePerspective);
    }

    connect(m_perspectiveComboBox, SIGNAL(textActivated(QString)),
        this, SLOT(changePerspective(QString)));

    restorePerspectives();
    if (m_perspectiveComboBox->currentText().isEmpty()) {
        const QString defName("Default");
        newPerspective(defName);
    }
}

/*
void MainWindow::writeSettings(){
    m_logger->message("MainWindow::writeSettings()", LoggerSeverity::LOG_DEBUG);
    QSettings setts = Settings::get();
    setts.setValue("geometry", saveGeometry());
    setts.setValue("perspective", m_perspectiveComboBox->currentText());
}
*/

ads::CDockWidget* MainWindow::findChildWindow(const QString& name) const {
    m_logger->message(QString("MainWindow::findMdiChild(%1)").arg(name));
   
    ads::CDockWidget* ret = nullptr;

    auto map = m_dockManager->dockWidgetsMap();
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        const auto value = it.value();
        QWidget* w = value->widget();
        const Widget* child = qobject_cast<Widget*>(w);
        if (child == nullptr) {
            continue;
        }

        if (child->name() == name) {
            ret = value;
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

ads::CDockWidget* MainWindow::findChildWindowByTitle(const QString& name) const {
    m_logger->message(QString("MainWindow::findMdiChild(%1)").arg(name));
    //TODO: check this

    ads::CDockWidget* ret = nullptr;

    QMap<QString, ads::CDockWidget*> map = m_dockManager->dockWidgetsMap();
    for (auto it = map.cbegin(), end = map.cend(); it != end; ++it) {
        const QString k = it.key();
        QWidget* w = it.value()->widget();
        const Widget* child = qobject_cast<Widget*>(w);
        if (child == nullptr) {
            continue;
        }

        if (child->objectName() == name) {
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