#ifndef WINDOW_H
#define WINDOW_H

//#ifndef DAEMON

#include <QSettings>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QCoreApplication>

#include "logger.h"
#include "api_global.h"

class Widget;

class API_EXPORT Window : public QMainWindow{

	Q_OBJECT

public:
	
	Window() = default;

	virtual ~Window() {}

	virtual bool addSubWindow(QWidget* widget, const QString& title = QString()) = 0;

	virtual bool addSubWindow(Widget* widget) = 0;

	QMenu* findMenu(const QString& title) {
		QList<QMenu*> menus = menuBar()->findChildren<QMenu*>(Qt::FindChildrenRecursively);

		for (auto it = menus.begin(), end = menus.end(); it != end; ++it) {
			if ((*it)->title() == title) {
				return *it;
			}
		}

		return nullptr;
	}

	virtual Widget* find(const QString& uuid) const = 0;

	virtual bool toggleWindow(const QString& title) = 0;

public slots:

	virtual void createOrActivate() = 0;

	virtual void create() = 0;

	virtual void showStatusMessage(const QString& msg, int timeout = 0) = 0;

};

template<typename T, typename C, typename G, typename D, typename P>
void windowAddPluginSettingsAction(const P* plugin, const QString& name, G* gtx, C* ctx, Logger* log) {
	if (plugin->name() != name) {
		return;
	}

	const auto actions = ctx->m_settings->actions();
	if (actions.size() == 1) {
		actions[0]->setVisible(false);
	}

	const T* mod = dynamic_cast<const T*>(plugin);

	if (mod == nullptr) {
		log->message("windowAddPluginSettingsAction(): Failed to cast plugin to QObject");
		return;
	}

	QAction* settings = ctx->m_settings->addAction(mod->objectName());
	settings->setData(QVariant(plugin->settingsPath()));
	
	QObject::connect(settings, &QAction::triggered, [gtx, mod, ctx] {
		if (gtx->m_win->toggleWindow(mod->objectName() + "/Settings")) {
			return;
		}

		auto dialog = new D(gtx->m_win, mod);
		QObject::connect(dialog, &D::settingsUpdated, mod, &T::settingsChanged);
		gtx->m_win->addSubWindow(dialog, mod->objectName() + "/Settings");
	});
}

API_EXPORT void windowAddInstanceSettings(QMenu *parent, QMenu** settings, QAction** newInstance, const QString& name, QCoreApplication *app, Logger* log);

#endif // WINDOW_H
