#ifndef PLUGIN_LIST_H
#define PLUGIN_LIST_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include "api/api.h"
#include "ModuleLoader.h"

class PluginList : public MdiChild {

	Q_OBJECT

public:

	PluginList(QWidget* parent, QWidget* mwin, MLoader* loader) 
		: MdiChild(nullptr, mwin){
		QStandardItemModel* model = new QStandardItemModel(0, 4);
		
		if (loader != nullptr) {
			auto list = loader->loaders();
			for (auto it = list.begin(), end = list.end(); it != end; ++it) {
				QList<QStandardItem*> row;
				row << new QStandardItem((*it)->name());
				row << new QStandardItem((*it)->version());
				row << new QStandardItem((*it)->description());
				row << new QStandardItem((*it)->author());
				model->appendRow(row);
			}
		}

		model->setHeaderData(0, Qt::Horizontal, "name");
		model->setHeaderData(1, Qt::Horizontal, "version");
		model->setHeaderData(2, Qt::Horizontal, "description");
		model->setHeaderData(3, Qt::Horizontal, "author");

		QTableView* view = new QTableView();
		view->setModel(model);

		QBoxLayout* lay = new QVBoxLayout();
		setLayout(lay);
		lay->addWidget(view);
	}

	bool saveSettings() {
		return true;
	}

public slots:
	void settingsChanged() override {}

};

#endif