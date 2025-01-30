#ifndef INSTANCES_H
#define INSTANCES_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPair>

#include "../api/api.h"
#include "../core/core.h"

class MainWindow;

class Instances : public Widget {

	Q_OBJECT

public:

	enum Columns : qint32 {
		NAME,
		OBJECT,
		VERSION,
		UUID
	};

	Instances(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, PluginSettings* setts, const QString& uuid);

	//bool saveSettings();

	void settingsChanged();

	SettingsMdi* settingsWindow() const override;

	bool reset(Reset type = Reset::SOFT) {
		return true;
	}

	bool initialize() override;

	bool deinitialize() override;

public slots:

	void loaded(const Plugin* plugin);

	void unloaded(const Plugin* plugin);

	QVariant exec() {
		//TODO: return Instances list 
		return QVariant();
	}

protected slots:

	void activated(const QModelIndex& index);

	void customMenuRequested(QPoint point);

	void settingsRequested();

	void deleteRequested();

	void copyRequested();

protected:

	MainWindow* mainWindow() const;

private:

	std::optional<QPair<QString, QString>> instanceInfo() const;

	QTableView* m_view = nullptr;
	QStandardItemModel* m_model = nullptr;

};

#endif