#include "PluginList.h"
#include <QCoreApplication>

struct PluginListMenu {
	PluginListMenu(QCoreApplication* app)
		: m_app(app) {
		if (app != nullptr) {
			m_pluginList = new QAction("Plugins");
			m_pluginList->setData(QVariant("PluginList"));
		}
	}

	QAction* m_pluginList = nullptr;
	QCoreApplication* m_app = nullptr;
};

static bool PluginList_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, PluginListMenu* ctx, Logger* log) {

	log->message("PluginList_register");

	GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
	if (gtx == nullptr) {
		log->message("PluginList_register(): application is non gui not registering");
		return false;
	}

	ctx->m_app = QCoreApplication::instance();
	ctx->m_pluginList = new QAction(ctx->m_app->translate("MainWindow", "Plugins"));
	ctx->m_pluginList->setData(QVariant("PluginList"));
	ctx->m_pluginList->setText(ctx->m_app->translate("MainWindow", "Plugins"));

	QMenu* settingsMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
	ctx->m_pluginList->setParent(settingsMenu);
	QObject::connect(ctx->m_pluginList, &QAction::triggered, gtx->m_win, &Window::createOrActivate);

	QList<QAction*> actions = settingsMenu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	settingsMenu->insertAction(actions.size() > 0 ? actions[1] : nullptr, ctx->m_pluginList);

	return true;
}

static bool PluginList_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, PluginListMenu* ctx, Logger* log) {

	return true;
}

REGISTER_STATIC_PLUGIN(
	PluginList,
	Plugin::Type::WIDGET,
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"Instances",
	PluginList_register,
	PluginList_unregister,
	PluginListMenu,
	{},
	false,
	1500
)

PluginList::PluginList(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
: Widget(ld, plugins, parent, settingsPath) {
	QStandardItemModel* model = new QStandardItemModel(0, 5);

	if (plugins != nullptr) {
		auto loader = reinterpret_cast<MLoader*>(plugins);
		auto list = loader->loaders();
		for (auto it = list.begin(), end = list.end(); it != end; ++it) {
			QList<QStandardItem*> row;
			row << new QStandardItem((*it)->enabled() ? tr("true") : tr("false"));
			row << new QStandardItem((*it)->name());
			row << new QStandardItem((*it)->version());
			row << new QStandardItem((*it)->description());
			row << new QStandardItem((*it)->author());
			model->appendRow(row);
		}
	}

	model->setHeaderData(0, Qt::Horizontal, tr("enabled"));
	model->setHeaderData(1, Qt::Horizontal, tr("name"));
	model->setHeaderData(2, Qt::Horizontal, tr("version"));
	model->setHeaderData(3, Qt::Horizontal, tr("description"));
	model->setHeaderData(4, Qt::Horizontal, tr("author"));

	QTableView* view = new QTableView();
	view->setShowGrid(true);
	view->setModel(model);
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	view->setAlternatingRowColors(true);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QBoxLayout* lay = new QVBoxLayout();
	setLayout(lay);
	lay->addWidget(view);
}

void PluginList::settingsChanged() {
}

SettingsMdi* PluginList::settingsWindow() const {
	return nullptr; // new SettingsDialog(nullptr, nullptr, settingsPath());
}