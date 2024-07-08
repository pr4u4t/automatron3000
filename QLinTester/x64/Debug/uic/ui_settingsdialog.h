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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *interval;
    QSpinBox *tries;
    QSpinBox *scanStartID;
    QSpinBox *scanStopID;
    QLabel *label_5;
    QCheckBox *enableColors;
    QLabel *label_6;
    QSpinBox *startID;
    QLabel *label_7;
    QSpinBox *stopID;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;

    void setupUi(MdiChild *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        interval = new QSpinBox(SettingsDialog);
        interval->setObjectName("interval");

        formLayout->setWidget(0, QFormLayout::FieldRole, interval);

        tries = new QSpinBox(SettingsDialog);
        tries->setObjectName("tries");

        formLayout->setWidget(1, QFormLayout::FieldRole, tries);

        scanStartID = new QSpinBox(SettingsDialog);
        scanStartID->setObjectName("scanStartID");

        formLayout->setWidget(2, QFormLayout::FieldRole, scanStartID);

        scanStopID = new QSpinBox(SettingsDialog);
        scanStopID->setObjectName("scanStopID");

        formLayout->setWidget(3, QFormLayout::FieldRole, scanStopID);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        enableColors = new QCheckBox(SettingsDialog);
        enableColors->setObjectName("enableColors");

        formLayout->setWidget(4, QFormLayout::FieldRole, enableColors);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        startID = new QSpinBox(SettingsDialog);
        startID->setObjectName("startID");

        formLayout->setWidget(5, QFormLayout::FieldRole, startID);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        stopID = new QSpinBox(SettingsDialog);
        stopID->setObjectName("stopID");

        formLayout->setWidget(6, QFormLayout::FieldRole, stopID);


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

    void retranslateUi(MdiChild *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Form", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Iterval [ms]", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Tries", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Start ID", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Stop ID", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Enable colors", nullptr));
        enableColors->setText(QString());
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Test start ID", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Test stop ID", nullptr));
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
