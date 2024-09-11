#ifndef WINDOW_H
#define WINDOW_H

//#ifndef DAEMON

#include <QSettings>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

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
		QList<QMenu*> menus = menuBar()->findChildren<QMenu*>(Qt::FindDirectChildrenOnly);

		for (auto it = menus.begin(), end = menus.end(); it != end; ++it) {
			if ((*it)->title() == title) {
				return *it;
			}
		}

		return nullptr;
	}



public slots:

	virtual void createOrActivate() = 0;

	virtual void create() = 0;

	virtual void showStatusMessage(const QString& msg, int timeout = 0) = 0;

};

//#endif // DAEMON
#endif // WINDOW_H
