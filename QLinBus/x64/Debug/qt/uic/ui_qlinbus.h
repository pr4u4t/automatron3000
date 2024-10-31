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
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *sniffEnable;
    QPushButton *sniffDisable;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *clearButton;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *exportJson;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *QLinBusUI)
    {
        if (QLinBusUI->objectName().isEmpty())
            QLinBusUI->setObjectName("QLinBusUI");
        QLinBusUI->resize(718, 537);
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
        groupBox_3 = new QGroupBox(groupBox_2);
        groupBox_3->setObjectName("groupBox_3");
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        sniffEnable = new QPushButton(groupBox_3);
        sniffEnable->setObjectName("sniffEnable");

        horizontalLayout_3->addWidget(sniffEnable);

        sniffDisable = new QPushButton(groupBox_3);
        sniffDisable->setObjectName("sniffDisable");

        horizontalLayout_3->addWidget(sniffDisable);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        horizontalLayout->addWidget(groupBox_3);

        groupBox = new QGroupBox(groupBox_2);
        groupBox->setObjectName("groupBox");
        horizontalLayout_2 = new QHBoxLayout(groupBox);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        startButton = new QPushButton(groupBox);
        startButton->setObjectName("startButton");

        horizontalLayout_2->addWidget(startButton);

        pauseButton = new QPushButton(groupBox);
        pauseButton->setObjectName("pauseButton");

        horizontalLayout_2->addWidget(pauseButton);

        stopButton = new QPushButton(groupBox);
        stopButton->setObjectName("stopButton");

        horizontalLayout_2->addWidget(stopButton);

        clearButton = new QPushButton(groupBox);
        clearButton->setObjectName("clearButton");

        horizontalLayout_2->addWidget(clearButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        horizontalLayout->addWidget(groupBox);


        verticalLayout_2->addLayout(horizontalLayout);

        groupBox_4 = new QGroupBox(groupBox_2);
        groupBox_4->setObjectName("groupBox_4");
        horizontalLayout_4 = new QHBoxLayout(groupBox_4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        exportJson = new QPushButton(groupBox_4);
        exportJson->setObjectName("exportJson");

        horizontalLayout_4->addWidget(exportJson);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        verticalLayout_2->addWidget(groupBox_4);


        verticalLayout->addWidget(groupBox_2);


        retranslateUi(QLinBusUI);

        QMetaObject::connectSlotsByName(QLinBusUI);
    } // setupUi

    void retranslateUi(QWidget *QLinBusUI)
    {
        QLinBusUI->setWindowTitle(QCoreApplication::translate("QLinBusUI", "Form", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("QLinBusUI", "Scan/Sniff", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("QLinBusUI", "Sniff", nullptr));
        sniffEnable->setText(QCoreApplication::translate("QLinBusUI", "Enable", nullptr));
        sniffDisable->setText(QCoreApplication::translate("QLinBusUI", "Disable", nullptr));
        groupBox->setTitle(QCoreApplication::translate("QLinBusUI", "Scan", nullptr));
        startButton->setText(QCoreApplication::translate("QLinBusUI", "Start", nullptr));
        pauseButton->setText(QCoreApplication::translate("QLinBusUI", "Pause", nullptr));
        stopButton->setText(QCoreApplication::translate("QLinBusUI", "Stop", nullptr));
        clearButton->setText(QCoreApplication::translate("QLinBusUI", "Clear", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("QLinBusUI", "Import/Export", nullptr));
        exportJson->setText(QCoreApplication::translate("QLinBusUI", "Export", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinBusUI: public Ui_QLinBusUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLINBUS_H
