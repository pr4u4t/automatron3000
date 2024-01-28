#include "QData.h"
#include "../api/api.h"

bool QData_register(Window* win) {
	if (win == nullptr) {
		return false;
	}

	QAction* database = new QAction(Window::tr("Database"), win);
	database->setData(QVariant("QData"));
	QObject::connect(database, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu("&File");
	if (menu == nullptr) {
		if ((menu = win->menuBar()->addMenu("&File")) == nullptr) {
			return false;
		}
	}

	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions[1], database);
	return true;
}

bool QData_unregister(Window* win) {
	return true;
}

REGISTER_PLUGIN(
	QData, 
	Plugin::Type::WIDGET, 
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"example plugin",
	QData_register,
	QData_unregister
)
