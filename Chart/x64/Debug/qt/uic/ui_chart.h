/********************************************************************************
** Form generated from reading UI file 'chart.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHART_H
#define UI_CHART_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_ChartUI
{
public:
    QVBoxLayout *verticalLayout;

    void setupUi(MdiChild *ChartUI)
    {
        if (ChartUI->objectName().isEmpty())
            ChartUI->setObjectName("ChartUI");
        ChartUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(ChartUI);
        verticalLayout->setObjectName("verticalLayout");

        retranslateUi(ChartUI);

        QMetaObject::connectSlotsByName(ChartUI);
    } // setupUi

    void retranslateUi(MdiChild *ChartUI)
    {
        ChartUI->setWindowTitle(QCoreApplication::translate("ChartUI", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChartUI: public Ui_ChartUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHART_H
