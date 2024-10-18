#ifndef PLUGINS_H
#define PLUGINS_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include "../api/api.h"
#include "../core/core.h"

class Plugins : public Widget {

	Q_OBJECT

public:

	Plugins(Loader* ld, PluginsLoader* plugins, QWidget* parent = nullptr, const QString& settingsPath = QString());

	~Plugins() {
	
	}

	bool saveSettings() {
		return true;
	}

	SettingsMdi* settingsWindow() const override;

	bool reset(Reset type = Reset::SOFT) {
		return true;
	}

	bool initialize() override;

	bool deinitialize() override;

public slots:

	void settingsChanged();

	QVariant exec() {
		return QVariant();
	}
};

#endif