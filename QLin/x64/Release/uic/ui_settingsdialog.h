/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_7;
    QSpinBox *baudrate;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *masterCheck;
    QRadioButton *slaveCheck;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label;
    QSpinBox *masterID;
    QLabel *label_4;
    QSpinBox *slaveID;
    QLabel *label_2;
    QComboBox *linVersion;
    QLabel *label_5;
    QGridLayout *gridLayout;
    QTableView *slaveInitialData;
    QLabel *label_9;
    QSpinBox *SlaveDLC;
    QLabel *label_6;
    QCheckBox *autoConnect;
    QLabel *label_8;
    QTableView *DLC;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QButtonGroup *modeGroup;

    void setupUi(QWidget *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(650, 470);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_7);

        baudrate = new QSpinBox(SettingsDialog);
        baudrate->setObjectName("baudrate");
        baudrate->setMinimum(10400);
        baudrate->setMaximum(20000);
        baudrate->setValue(16500);

        formLayout->setWidget(0, QFormLayout::FieldRole, baudrate);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        masterCheck = new QRadioButton(SettingsDialog);
        modeGroup = new QButtonGroup(SettingsDialog);
        modeGroup->setObjectName("modeGroup");
        modeGroup->addButton(masterCheck);
        masterCheck->setObjectName("masterCheck");

        horizontalLayout_2->addWidget(masterCheck);

        slaveCheck = new QRadioButton(SettingsDialog);
        modeGroup->addButton(slaveCheck);
        slaveCheck->setObjectName("slaveCheck");

        horizontalLayout_2->addWidget(slaveCheck);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        formLayout->setLayout(1, QFormLayout::FieldRole, horizontalLayout_2);

        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        masterID = new QSpinBox(SettingsDialog);
        masterID->setObjectName("masterID");
        masterID->setMaximum(63);

        formLayout->setWidget(2, QFormLayout::FieldRole, masterID);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        slaveID = new QSpinBox(SettingsDialog);
        slaveID->setObjectName("slaveID");

        formLayout->setWidget(3, QFormLayout::FieldRole, slaveID);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_2);

        linVersion = new QComboBox(SettingsDialog);
        linVersion->addItem(QString());
        linVersion->addItem(QString());
        linVersion->addItem(QString());
        linVersion->setObjectName("linVersion");

        formLayout->setWidget(4, QFormLayout::FieldRole, linVersion);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_5);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        slaveInitialData = new QTableView(SettingsDialog);
        slaveInitialData->setObjectName("slaveInitialData");
        slaveInitialData->setAlternatingRowColors(true);
        slaveInitialData->setSelectionMode(QAbstractItemView::SingleSelection);

        gridLayout->addWidget(slaveInitialData, 0, 0, 1, 1);


        formLayout->setLayout(5, QFormLayout::FieldRole, gridLayout);

        label_9 = new QLabel(SettingsDialog);
        label_9->setObjectName("label_9");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_9);

        SlaveDLC = new QSpinBox(SettingsDialog);
        SlaveDLC->setObjectName("SlaveDLC");
        SlaveDLC->setMinimum(1);
        SlaveDLC->setMaximum(8);

        formLayout->setWidget(7, QFormLayout::FieldRole, SlaveDLC);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_6);

        autoConnect = new QCheckBox(SettingsDialog);
        autoConnect->setObjectName("autoConnect");

        formLayout->setWidget(9, QFormLayout::FieldRole, autoConnect);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(10, QFormLayout::LabelRole, label_8);

        DLC = new QTableView(SettingsDialog);
        DLC->setObjectName("DLC");

        formLayout->setWidget(10, QFormLayout::FieldRole, DLC);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(SettingsDialog);
        okButton->setObjectName("okButton");

        horizontalLayout->addWidget(okButton);

        applyButton = new QPushButton(SettingsDialog);
        applyButton->setObjectName("applyButton");

        horizontalLayout->addWidget(applyButton);

        cancelButton = new QPushButton(SettingsDialog);
        cancelButton->setObjectName("cancelButton");

        horizontalLayout->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QWidget *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Form", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Baudrate", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Mode", nullptr));
        masterCheck->setText(QCoreApplication::translate("SettingsDialog", "Master", nullptr));
        slaveCheck->setText(QCoreApplication::translate("SettingsDialog", "Slave", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Master ID", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Slave ID", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "LIN Version", nullptr));
        linVersion->setItemText(0, QCoreApplication::translate("SettingsDialog", "1.3", nullptr));
        linVersion->setItemText(1, QCoreApplication::translate("SettingsDialog", "2.0", nullptr));
        linVersion->setItemText(2, QCoreApplication::translate("SettingsDialog", "2.1", nullptr));

        label_5->setText(QCoreApplication::translate("SettingsDialog", "Slave initial data", nullptr));
        label_9->setText(QCoreApplication::translate("SettingsDialog", "Slave DLC", nullptr));
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Auto connect", nullptr));
        autoConnect->setText(QString());
        label_8->setText(QCoreApplication::translate("SettingsDialog", "DLC", nullptr));
        okButton->setText(QCoreApplication::translate("SettingsDialog", "OK", nullptr));
        applyButton->setText(QCoreApplication::translate("SettingsDialog", "Apply", nullptr));
        cancelButton->setText(QCoreApplication::translate("SettingsDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
