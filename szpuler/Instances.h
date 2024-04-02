#ifndef INSTANCES_H
#define INSTANCES_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include "../api/api.h"
#include "../core/core.h"

class Instances : public Widget {

	Q_OBJECT

public:

	Instances(Loader* ld, PluginsLoader* plugins, QWidget* parent = nullptr, const QString& settingsPath = QString());

	bool saveSettings();

	void settingsChanged();

public slots:

	void loaded(const Plugin* plugin);

private:

	QTableView* m_view = nullptr;
	QStandardItemModel* m_model = nullptr;

};

#endif