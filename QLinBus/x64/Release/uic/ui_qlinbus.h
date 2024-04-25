/********************************************************************************
** Form generated from reading UI file 'qlinbus.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QLINBUS_H
#define UI_QLINBUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLinBusUI
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QTableView *scanTable;
    QProgressBar *scanProgress;
    QHBoxLayout *horizontalLayout;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *clearButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *QLinBusUI)
    {
        if (QLinBusUI->objectName().isEmpty())
            QLinBusUI->setObjectName("QLinBusUI");
        QLinBusUI->resize(409, 412);
        verticalLayout = new QVBoxLayout(QLinBusUI);
        verticalLayout->setObjectName("verticalLayout");
        groupBox_2 = new QGroupBox(QLinBusUI);
        groupBox_2->setObjectName("groupBox_2");
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        scanTable = new QTableView(groupBox_2);
        scanTable->setObjectName("scanTable");
        scanTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        scanTable->setAlternatingRowColors(true);
        scanTable->setSelectionMode(QAbstractItemView::SingleSelection);
        scanTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_2->addWidget(scanTable);

        scanProgress = new QProgressBar(groupBox_2);
        scanProgress->setObjectName("scanProgress");
        scanProgress->setAutoFillBackground(false);
        scanProgress->setMaximum(63);
        scanProgress->setValue(0);
        scanProgress->setInvertedAppearance(false);
        scanProgress->setTextDirection(QProgressBar::TopToBottom);

        verticalLayout_2->addWidget(scanProgress);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        startButton = new QPushButton(groupBox_2);
        startButton->setObjectName("startButton");

        horizontalLayout->addWidget(startButton);

        stopButton = new QPushButton(groupBox_2);
        stopButton->setObjectName("stopButton");

        horizontalLayout->addWidget(stopButton);

        clearButton = new QPushButton(groupBox_2);
        clearButton->setObjectName("clearButton");

        horizontalLayout->addWidget(clearButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(groupBox_2);


        retranslateUi(QLinBusUI);

        QMetaObject::connectSlotsByName(QLinBusUI);
    } // setupUi

    void retranslateUi(QWidget *QLinBusUI)
    {
        QLinBusUI->setWindowTitle(QCoreApplication::translate("QLinBusUI", "Form", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("QLinBusUI", "Scan", nullptr));
        startButton->setText(QCoreApplication::translate("QLinBusUI", "Start", nullptr));
        stopButton->setText(QCoreApplication::translate("QLinBusUI", "Stop", nullptr));
        clearButton->setText(QCoreApplication::translate("QLinBusUI", "Clear", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinBusUI: public Ui_QLinBusUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLINBUS_H
