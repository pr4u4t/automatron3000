#ifndef INSTANCES_H
#define INSTANCES_H

#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include "api/api.h"
#include "ModuleLoader.h"

class Instances : public MdiChild {

	Q_OBJECT

public:

	Instances(QWidget* parent, QWidget* mwin, MLoader* loader)
		: MdiChild(mwin) {
		QStandardItemModel* model = new QStandardItemModel(0, 2);

		if (loader != nullptr) {
			auto list = loader->instances();
			for (auto it = list.begin(), end = list.end(); it != end; ++it) {
				QList<QStandardItem*> row;
				row << new QStandardItem((*it)->name());
				row << new QStandardItem((*it)->version());
				model->appendRow(row);
			}
		}

		model->setHeaderData(0, Qt::Horizontal, tr("name"));
		model->setHeaderData(1, Qt::Horizontal, tr("version"));

		QTableView* view = new QTableView();
		view->setShowGrid(true);
		view->setModel(model);
		view->setEditTriggers(QAbstractItemView::NoEditTriggers);
		view->setAlternatingRowColors(true);
		view->setSelectionMode(QAbstractItemView::SingleSelection);
		view->setSelectionBehavior(QAbstractItemView::SelectRows);
		view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		QBoxLayout* lay = new QVBoxLayout();
		setLayout(lay);
		lay->addWidget(view);
	}

	bool saveSettings() {
		return true;
	}

};

#endif