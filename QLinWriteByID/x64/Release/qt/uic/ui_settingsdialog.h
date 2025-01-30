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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
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
    QLineEdit *nadEdit;
    QLabel *label_2;
    QSpinBox *interval;
    QLabel *label_3;
    QSpinBox *tries;
    QLabel *label_4;
    QLineEdit *title;
    QLabel *label_5;
    QLineEdit *dataSource;
    QLabel *label_6;
    QLineEdit *previousEdit;
    QLabel *label_7;
    QLineEdit *linEdit;
    QLabel *label_8;
    QLineEdit *didhEdit;
    QLabel *label_9;
    QLineEdit *didlEdit;
    QLabel *label_10;
    QLabel *label_11;
    QSpinBox *reschedulesEdit;
    QSpinBox *rescheduleIntervalEdit;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *applyButton;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(MdiChild *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(400, 392);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        nadEdit = new QLineEdit(SettingsDialog);
        nadEdit->setObjectName("nadEdit");

        formLayout->setWidget(0, QFormLayout::FieldRole, nadEdit);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_2);

        interval = new QSpinBox(SettingsDialog);
        interval->setObjectName("interval");
        interval->setMaximum(500);

        formLayout->setWidget(3, QFormLayout::FieldRole, interval);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_3);

        tries = new QSpinBox(SettingsDialog);
        tries->setObjectName("tries");

        formLayout->setWidget(6, QFormLayout::FieldRole, tries);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_4);

        title = new QLineEdit(SettingsDialog);
        title->setObjectName("title");

        formLayout->setWidget(7, QFormLayout::FieldRole, title);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_5);

        dataSource = new QLineEdit(SettingsDialog);
        dataSource->setObjectName("dataSource");

        formLayout->setWidget(8, QFormLayout::FieldRole, dataSource);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_6);

        previousEdit = new QLineEdit(SettingsDialog);
        previousEdit->setObjectName("previousEdit");

        formLayout->setWidget(4, QFormLayout::FieldRole, previousEdit);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_7);

        linEdit = new QLineEdit(SettingsDialog);
        linEdit->setObjectName("linEdit");

        formLayout->setWidget(9, QFormLayout::FieldRole, linEdit);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_8);

        didhEdit = new QLineEdit(SettingsDialog);
        didhEdit->setObjectName("didhEdit");

        formLayout->setWidget(1, QFormLayout::FieldRole, didhEdit);

        label_9 = new QLabel(SettingsDialog);
        label_9->setObjectName("label_9");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_9);

        didlEdit = new QLineEdit(SettingsDialog);
        didlEdit->setObjectName("didlEdit");

        formLayout->setWidget(2, QFormLayout::FieldRole, didlEdit);

        label_10 = new QLabel(SettingsDialog);
        label_10->setObjectName("label_10");

        formLayout->setWidget(10, QFormLayout::LabelRole, label_10);

        label_11 = new QLabel(SettingsDialog);
        label_11->setObjectName("label_11");

        formLayout->setWidget(11, QFormLayout::LabelRole, label_11);

        reschedulesEdit = new QSpinBox(SettingsDialog);
        reschedulesEdit->setObjectName("reschedulesEdit");

        formLayout->setWidget(10, QFormLayout::FieldRole, reschedulesEdit);

        rescheduleIntervalEdit = new QSpinBox(SettingsDialog);
        rescheduleIntervalEdit->setObjectName("rescheduleIntervalEdit");
        rescheduleIntervalEdit->setMaximum(500);

        formLayout->setWidget(11, QFormLayout::FieldRole, rescheduleIntervalEdit);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        applyButton = new QPushButton(SettingsDialog);
        applyButton->setObjectName("applyButton");

        horizontalLayout->addWidget(applyButton);

        okButton = new QPushButton(SettingsDialog);
        okButton->setObjectName("okButton");

        horizontalLayout->addWidget(okButton);

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
        label->setText(QCoreApplication::translate("SettingsDialog", "NAD", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Interval [ms]", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Tries", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Title", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Data Source", nullptr));
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Previous", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Lin Device", nullptr));
        label_8->setText(QCoreApplication::translate("SettingsDialog", "DIDH", nullptr));
        label_9->setText(QCoreApplication::translate("SettingsDialog", "DIDL", nullptr));
        label_10->setText(QCoreApplication::translate("SettingsDialog", "Reschedules", nullptr));
        label_11->setText(QCoreApplication::translate("SettingsDialog", "Reschedule Interval", nullptr));
        applyButton->setText(QCoreApplication::translate("SettingsDialog", "Apply", nullptr));
        okButton->setText(QCoreApplication::translate("SettingsDialog", "OK", nullptr));
        cancelButton->setText(QCoreApplication::translate("SettingsDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
