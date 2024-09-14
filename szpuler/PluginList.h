#ifndef PLUGIN_LIST_H
#define PLUGIN_LIST_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include "../api/api.h"
#include "../core/core.h"

class PluginList : public Widget {

	Q_OBJECT

public:

	PluginList(Loader* ld, PluginsLoader* plugins, QWidget* parent = nullptr, const QString& settingsPath = QString());

	~PluginList() {
	
	}

	bool saveSettings() {
		return true;
	}

	SettingsMdi* settingsWindow() const override;

public slots:

	void settingsChanged();

};

#endif