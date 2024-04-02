#include "Instances.h"
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
    false
)

Instances::Instances(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
	: Widget(ld, plugins, parent, settingsPath)
    , m_model(new QStandardItemModel(0, 3))
    , m_view(new QTableView()){

    connect(plugins, &PluginsLoader::loaded, this, &Instances::loaded);

	if (plugins != nullptr) {
        auto mloader = reinterpret_cast<MLoader*>(plugins);
		auto list = mloader->instances();
		for (auto it = list.begin(), end = list.end(); it != end; ++it) {
			QList<QStandardItem*> row;
			row << new QStandardItem((*it)->name());
			row << new QStandardItem((*it)->version());
            row << new QStandardItem((*it)->uuid());
			m_model->appendRow(row);
		}
	}

	m_model->setHeaderData(0, Qt::Horizontal, tr("name"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("version"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("uuid"));

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
}

bool Instances::saveSettings() {
	return true;
}

void Instances::settingsChanged() {
}

void Instances::loaded(const Plugin* plugin) {
    QList<QStandardItem*> row;
    row << new QStandardItem(plugin->name());
    row << new QStandardItem(plugin->version());
    row << new QStandardItem(plugin->uuid());
    m_model->appendRow(row);
}