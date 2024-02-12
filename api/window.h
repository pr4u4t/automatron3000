#ifndef WINDOW_H
#define WINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

#include "logger.h"
#include "api_global.h"

class API_EXPORT Window : public QMainWindow{

	Q_OBJECT

public:
	
	Window() = default;

	virtual QSettings& settings() = 0;

	virtual Logger* logger() = 0;

	virtual ~Window() {}

	virtual bool addSubWindow(QWidget* widget) = 0;

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

};

#endif
