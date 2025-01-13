#include "Instances.h"
#include "MainWindow.h"
#include <QCoreApplication>

struct InstancesMenu {
    InstancesMenu(QCoreApplication* app)
        : m_app(app) {
        if (app != nullptr) {
            m_instances = new QAction("Instances");
            m_instances->setData(QVariant("Instances"));
        }
    }

    QAction* m_instances = nullptr;
    QCoreApplication* m_app = nullptr;
};

static bool Instances_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, InstancesMenu* ctx, Logger* log) {
    log->message("Instances_register");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("Instances_register(): application is non gui not registering");
        return false;
    }

    ctx->m_app = QCoreApplication::instance();
    ctx->m_instances = new QAction(ctx->m_app->translate("MainWindow", "Instances"));
    ctx->m_instances->setData(QVariant("Instances"));
    ctx->m_instances->setText(ctx->m_app->translate("MainWindow", "Instances"));

    QMenu* settingsMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    ctx->m_instances->setParent(settingsMenu);
    QObject::connect(ctx->m_instances, &QAction::triggered, gtx->m_win, &Window::createOrActivate);

    QList<QAction*> actions = settingsMenu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
    settingsMenu->insertAction(actions.size() > 0 ? actions[1] : nullptr, ctx->m_instances);

    return true;
}

static bool Instances_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, InstancesMenu* ctx, Logger* log) {
    log->message("Instances_unregister");
    return true;
}

REGISTER_STATIC_PLUGIN(
    Instances,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "Instances",
    Instances_register,
    Instances_unregister,
    InstancesMenu,
    {},
    false,
    1300,
    PluginSettings
)

Instances::Instances(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, PluginSettings* setts, const QString& uuid)
	: Widget(ld, plugins, parent, settingsPath, setts, uuid)
    , m_model(new QStandardItemModel(0, 4))
    , m_view(new QTableView()){
}

bool Instances::initialize() {
    emit message("Instances::initialize()");
    connect(plugins(), &PluginsLoader::loaded, this, &Instances::loaded);

    if (plugins() != nullptr) {
        auto mloader = reinterpret_cast<MLoader*>(plugins());
        auto list = mloader->instances();
        for (auto it = list.begin(), end = list.end(); it != end; ++it) {
            QList<QStandardItem*> row;
            row << new QStandardItem((*it)->name());
            row << new QStandardItem(dynamic_cast<QObject*>((*it).data())->objectName());
            row << new QStandardItem((*it)->version());
            row << new QStandardItem((*it)->uuid());
            m_model->appendRow(row);
        }
    }

    m_model->setHeaderData(Columns::NAME, Qt::Horizontal, tr("name"));
    m_model->setHeaderData(Columns::OBJECT, Qt::Horizontal, tr("object"));
    m_model->setHeaderData(Columns::VERSION, Qt::Horizontal, tr("version"));
    m_model->setHeaderData(Columns::UUID, Qt::Horizontal, tr("uuid"));

    m_view->setShowGrid(true);
    m_view->setModel(m_model);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QBoxLayout* lay = new QVBoxLayout();
    setLayout(lay);
    lay->addWidget(m_view);

    QObject::connect(m_view, &QTableView::activated, this, &Instances::activated);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)),
        SLOT(customMenuRequested(QPoint)));

    return true;
}

bool Instances::deinitialize() {
    return false;
}

//bool Instances::saveSettings() {
//	return true;
//}

void Instances::settingsChanged() {
}

SettingsMdi* Instances::settingsWindow() const {
    return nullptr;
}

void Instances::loaded(const Plugin* plugin) {
    emit message("Instances::loaded(const Plugin* plugin)");
    for (int row = 0; row < m_model->rowCount(); ++row) {
        const QStandardItem* item = m_model->item(row, Columns::UUID);
        if (item->text() == plugin->uuid()) {
            return;
        }
    }

    QList<QStandardItem*> row;
    row << new QStandardItem(plugin->name());
    row << new QStandardItem(dynamic_cast<const QObject*>(plugin)->objectName());
    row << new QStandardItem(plugin->version());
    row << new QStandardItem(plugin->uuid());
    m_model->appendRow(row);
}

void Instances::activated(const QModelIndex& index) {
    emit message("Instances::activated(const QModelIndex& index)");
    if (index.isValid() == false) {
        return;
    }

    QModelIndex idx = index.siblingAtColumn(Columns::NAME);
    const QString name = index.data().toString();
    const QString uuid = idx.siblingAtColumn(Columns::UUID).data().toString();
    
    if (name.isEmpty() || uuid.isEmpty()) {
        return;
    }

    PluginsLoader* pld = plugins();
    MLoader* mld = reinterpret_cast<ModuleLoader<Loader>*>(pld);
    MainWindow* win = qobject_cast<MainWindow*>(mld->context()->to<GuiLoaderContext>()->m_win);
    Widget* widget = win->find(uuid);
    if (widget != nullptr) {
        return;
    }

    auto p = mld->find(uuid);
    if ((widget = dynamic_cast<Widget*>(p.data())) == nullptr) {
        return;
    }
    win->addSubWindow(widget);
}

void Instances::customMenuRequested(QPoint point) {
    emit message("Instances::customMenuRequested(QPoint point)");
    QMenu menu;
    menu.addAction(tr("Copy"), this, &Instances::copyRequested);
    menu.addAction(tr("Close"), this, &Instances::deleteRequested);
    menu.addSeparator();
    menu.addAction(tr("Settings"), this, &Instances::settingsRequested);
    menu.exec(m_view->mapToGlobal(point));
}

MainWindow* Instances::mainWindow() const {
    emit message("Instances::mainWindow() const");
    const QWidgetList& list = QApplication::topLevelWidgets();

    for (QWidget* w : list) {
        MainWindow* mainWindow = qobject_cast<MainWindow*>(w);
        if (mainWindow) {
            return mainWindow;
        }
    }

    return nullptr;
}

std::optional<QPair<QString, QString>> Instances::instanceInfo() const {
    emit message("Instances::instanceInfo()");
    QItemSelectionModel* selected = m_view->selectionModel();

    if (selected->hasSelection() != true) {
        emit message("Instances::instanceInfo(): no selection");
        return std::nullopt;
    }

    QModelIndexList list = selected->selectedRows();

    if (list.size() != 1) {
        emit message("Instances::instanceInfo(): selection size =! 1");
        return std::nullopt;
    }

    const QString name = list[0].siblingAtColumn(Columns::NAME).data().toString();
    const QString uuid = list[0].siblingAtColumn(Columns::UUID).data().toString();

    return QPair<QString, QString>(name, uuid);
}

void Instances::settingsRequested() {
    emit message("Instances::settingsRequested()");
    MainWindow* win = mainWindow();

    if (win == nullptr) {
        return;
    }

    auto info = instanceInfo();

    if (info.has_value() == false) {
        emit message("Instances::settingsRequested(): instance info from selection is empty");
        return;
    }

    auto plugin = plugins()->find(info.value().second);
    
    if (plugin.isNull() == true) {
        emit message("Instances::settingsRequested(): failed to find plugin by uuid");
        return;
    }

    SettingsMdi* dialog = plugin->settingsWindow();

    if (dialog == nullptr) {
        return;
    }

    win->addSubWindow(dialog, info.value().first + "/Settings");
}

void Instances::deleteRequested() {
    emit message("Instances::deleteRequested()");
    auto info = instanceInfo();
}

void Instances::copyRequested() {
    emit message("Instances::copyRequested()");
    auto info = instanceInfo();

    if (info.has_value() == false) {
        emit message("Instances::copyRequested(): instance info from selection is empty");
        return;
    }

    auto ld = plugins();

    if (ld == nullptr) {
        emit message("Instances::copyRequested(): plugins loader == nullptr");
        return;
    }

    auto clone = ld->copy(info.value().second);
    if (clone.isNull()) {
        emit message(QString("Instances::copyRequested(): failed to clone %1").arg(info.value().second));
        return;
    }
    emit message(QString("Instances::copyRequested(): cloned (%1, %2) -> (%3, %4)").arg(info.value().first).arg(info.value().second).arg(clone->name()).arg(clone->uuid()));

    if (clone->type() != Plugin::Type::WIDGET) {
        return;
    }
    
    PluginsLoader* pld = plugins();
    MLoader* mld = reinterpret_cast<ModuleLoader<Loader>*>(pld);

    MainWindow* win = qobject_cast<MainWindow*>(mld->context()->to<GuiLoaderContext>()->m_win);
    if (win == nullptr) {
        emit message("Instances::copyRequested(): failed to obtain main window");
        return;
    }

    win->addSubWindow(clone.dynamicCast<Widget>().data());
}
