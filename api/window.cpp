#include "window.h"
#include <QStyle>

void windowAddInstanceSettings(QMenu* parent, QMenu** settings, QAction** newInstance, const QString& name, QCoreApplication* app, Logger* log) {
	
	parent->addSection(app->translate("MainWindow", name.toLocal8Bit()));
	if (parent->style()->styleHint(QStyle::SH_Menu_SupportsSections) == 0) {
		parent->addAction(app->translate("MainWindow", name.toLocal8Bit()));
	}

	*settings = parent->addMenu(app->translate("MainWindow", "Settings"));
	(*settings)->addAction(app->translate("MainWindow", "empty"))->setEnabled(false);

	*newInstance = parent->addAction(app->translate("MainWindow", "New instance"));
	(*newInstance)->setData(QVariant(name));
}