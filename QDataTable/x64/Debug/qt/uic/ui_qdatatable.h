/********************************************************************************
** Form generated from reading UI file 'qdatatable.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QDATATABLE_H
#define UI_QDATATABLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QDataTableUI
{
public:
    QVBoxLayout *verticalLayout;
    QTableView *tableView;

    void setupUi(MdiChild *QDataTableUI)
    {
        if (QDataTableUI->objectName().isEmpty())
            QDataTableUI->setObjectName("QDataTableUI");
        QDataTableUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QDataTableUI);
        verticalLayout->setObjectName("verticalLayout");
        tableView = new QTableView(QDataTableUI);
        tableView->setObjectName("tableView");

        verticalLayout->addWidget(tableView);


        retranslateUi(QDataTableUI);

        QMetaObject::connectSlotsByName(QDataTableUI);
    } // setupUi

    void retranslateUi(MdiChild *QDataTableUI)
    {
        QDataTableUI->setWindowTitle(QCoreApplication::translate("QDataTableUI", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QDataTableUI: public Ui_QDataTableUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDATATABLE_H
