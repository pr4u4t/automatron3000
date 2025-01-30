/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
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
#include <QtWidgets/QLineEdit>
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
    QLabel *label_10;
    QComboBox *channelCombo;
    QLabel *label_14;
    QComboBox *checksumMethod;
    QLabel *label_12;
    QLineEdit *configName;
    QLabel *label_13;
    QSpinBox *queueSize;
    QLabel *label_7;
    QSpinBox *baudrate;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *masterCheck;
    QRadioButton *slaveCheck;
    QRadioButton *masterNslave;
    QSpacerItem *horizontalSpacer_2;
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
    QLabel *label_11;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *vectorConfig;
    QSpacerItem *horizontalSpacer_3;
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
        SettingsDialog->resize(650, 566);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_10 = new QLabel(SettingsDialog);
        label_10->setObjectName("label_10");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_10);

        channelCombo = new QComboBox(SettingsDialog);
        channelCombo->setObjectName("channelCombo");

        formLayout->setWidget(0, QFormLayout::FieldRole, channelCombo);

        label_14 = new QLabel(SettingsDialog);
        label_14->setObjectName("label_14");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_14);

        checksumMethod = new QComboBox(SettingsDialog);
        checksumMethod->addItem(QString());
        checksumMethod->addItem(QString());
        checksumMethod->setObjectName("checksumMethod");

        formLayout->setWidget(2, QFormLayout::FieldRole, checksumMethod);

        label_12 = new QLabel(SettingsDialog);
        label_12->setObjectName("label_12");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_12);

        configName = new QLineEdit(SettingsDialog);
        configName->setObjectName("configName");

        formLayout->setWidget(3, QFormLayout::FieldRole, configName);

        label_13 = new QLabel(SettingsDialog);
        label_13->setObjectName("label_13");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_13);

        queueSize = new QSpinBox(SettingsDialog);
        queueSize->setObjectName("queueSize");
        queueSize->setMaximum(512);
        queueSize->setValue(256);

        formLayout->setWidget(4, QFormLayout::FieldRole, queueSize);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_7);

        baudrate = new QSpinBox(SettingsDialog);
        baudrate->setObjectName("baudrate");
        baudrate->setMinimum(10400);
        baudrate->setMaximum(20000);
        baudrate->setValue(16500);

        formLayout->setWidget(5, QFormLayout::FieldRole, baudrate);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_3);

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

        masterNslave = new QRadioButton(SettingsDialog);
        modeGroup->addButton(masterNslave);
        masterNslave->setObjectName("masterNslave");

        horizontalLayout_2->addWidget(masterNslave);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        formLayout->setLayout(6, QFormLayout::FieldRole, horizontalLayout_2);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_4);

        slaveID = new QSpinBox(SettingsDialog);
        slaveID->setObjectName("slaveID");

        formLayout->setWidget(7, QFormLayout::FieldRole, slaveID);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_2);

        linVersion = new QComboBox(SettingsDialog);
        linVersion->addItem(QString());
        linVersion->addItem(QString());
        linVersion->addItem(QString());
        linVersion->setObjectName("linVersion");

        formLayout->setWidget(8, QFormLayout::FieldRole, linVersion);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_5);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        slaveInitialData = new QTableView(SettingsDialog);
        slaveInitialData->setObjectName("slaveInitialData");
        slaveInitialData->setAlternatingRowColors(true);
        slaveInitialData->setSelectionMode(QAbstractItemView::SingleSelection);

        gridLayout->addWidget(slaveInitialData, 0, 0, 1, 1);


        formLayout->setLayout(9, QFormLayout::FieldRole, gridLayout);

        label_9 = new QLabel(SettingsDialog);
        label_9->setObjectName("label_9");

        formLayout->setWidget(11, QFormLayout::LabelRole, label_9);

        SlaveDLC = new QSpinBox(SettingsDialog);
        SlaveDLC->setObjectName("SlaveDLC");
        SlaveDLC->setMinimum(1);
        SlaveDLC->setMaximum(8);

        formLayout->setWidget(11, QFormLayout::FieldRole, SlaveDLC);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(13, QFormLayout::LabelRole, label_6);

        autoConnect = new QCheckBox(SettingsDialog);
        autoConnect->setObjectName("autoConnect");

        formLayout->setWidget(13, QFormLayout::FieldRole, autoConnect);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(14, QFormLayout::LabelRole, label_8);

        DLC = new QTableView(SettingsDialog);
        DLC->setObjectName("DLC");
        DLC->setAlternatingRowColors(true);

        formLayout->setWidget(14, QFormLayout::FieldRole, DLC);

        label_11 = new QLabel(SettingsDialog);
        label_11->setObjectName("label_11");

        formLayout->setWidget(16, QFormLayout::LabelRole, label_11);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        vectorConfig = new QPushButton(SettingsDialog);
        vectorConfig->setObjectName("vectorConfig");

        horizontalLayout_3->addWidget(vectorConfig);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        formLayout->setLayout(16, QFormLayout::FieldRole, horizontalLayout_3);


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
        label_10->setText(QCoreApplication::translate("SettingsDialog", "Channel", nullptr));
        label_14->setText(QCoreApplication::translate("SettingsDialog", "Checksum method", nullptr));
        checksumMethod->setItemText(0, QCoreApplication::translate("SettingsDialog", "Classic", nullptr));
        checksumMethod->setItemText(1, QCoreApplication::translate("SettingsDialog", "Enhanced", nullptr));

        label_12->setText(QCoreApplication::translate("SettingsDialog", "Configuration Name", nullptr));
        label_13->setText(QCoreApplication::translate("SettingsDialog", "Queue Size", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Baudrate", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Mode", nullptr));
        masterCheck->setText(QCoreApplication::translate("SettingsDialog", "Master", nullptr));
        slaveCheck->setText(QCoreApplication::translate("SettingsDialog", "Slave", nullptr));
        masterNslave->setText(QCoreApplication::translate("SettingsDialog", "Master+Slave", nullptr));
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
        label_11->setText(QCoreApplication::translate("SettingsDialog", "Vector Cofiguration", nullptr));
        vectorConfig->setText(QCoreApplication::translate("SettingsDialog", "Open", nullptr));
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
