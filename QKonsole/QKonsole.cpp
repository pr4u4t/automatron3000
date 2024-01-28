#include "QKonsole.h"

bool QKonsole_register(Window* win) {
	QAction* console = new QAction(Window::tr("Console"), win);
	console->setData(QVariant("QKonsole"));
	QObject::connect(console, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu("&File");
	if (menu == nullptr) {
		if ((menu = win->menuBar()->addMenu("&File")) == nullptr) {
			return false;
		}
	}

	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions[1], console);
	return true;
}

bool QKonsole_unregister(Window* win) {
	return true;
}

REGISTER_PLUGIN(
	QKonsole, 
	Plugin::Type::WIDGET, 
	"0.0.1", 
	"pawel.ciejka@gmail.com", 
	"example plugin",
	QKonsole_register,
	QKonsole_unregister
)
