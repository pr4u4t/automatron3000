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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *titleEdit;
    QLabel *label;
    QTableView *tableView;
    QLineEdit *buttonEdit;
    QLabel *label_4;
    QLabel *label_5;
    QCheckBox *progressCheckbox;
    QCheckBox *verboseCheckbox;
    QLabel *label_6;
    QSpinBox *intervalSpin;
    QLabel *label_7;
    QCheckBox *retryOnFailure;
    QLabel *label_8;
    QSpinBox *retries;
    QLabel *label_9;
    QSpinBox *retryInterval;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;

    void setupUi(MdiChild *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(492, 369);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_2);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        titleEdit = new QLineEdit(SettingsDialog);
        titleEdit->setObjectName("titleEdit");

        formLayout->setWidget(0, QFormLayout::FieldRole, titleEdit);

        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        tableView = new QTableView(SettingsDialog);
        tableView->setObjectName("tableView");

        formLayout->setWidget(9, QFormLayout::FieldRole, tableView);

        buttonEdit = new QLineEdit(SettingsDialog);
        buttonEdit->setObjectName("buttonEdit");

        formLayout->setWidget(1, QFormLayout::FieldRole, buttonEdit);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        progressCheckbox = new QCheckBox(SettingsDialog);
        progressCheckbox->setObjectName("progressCheckbox");

        formLayout->setWidget(2, QFormLayout::FieldRole, progressCheckbox);

        verboseCheckbox = new QCheckBox(SettingsDialog);
        verboseCheckbox->setObjectName("verboseCheckbox");

        formLayout->setWidget(3, QFormLayout::FieldRole, verboseCheckbox);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_6);

        intervalSpin = new QSpinBox(SettingsDialog);
        intervalSpin->setObjectName("intervalSpin");
        intervalSpin->setMaximum(3000);

        formLayout->setWidget(8, QFormLayout::FieldRole, intervalSpin);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        retryOnFailure = new QCheckBox(SettingsDialog);
        retryOnFailure->setObjectName("retryOnFailure");

        formLayout->setWidget(6, QFormLayout::FieldRole, retryOnFailure);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_8);

        retries = new QSpinBox(SettingsDialog);
        retries->setObjectName("retries");
        retries->setMaximum(5);

        formLayout->setWidget(7, QFormLayout::FieldRole, retries);

        label_9 = new QLabel(SettingsDialog);
        label_9->setObjectName("label_9");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_9);

        retryInterval = new QSpinBox(SettingsDialog);
        retryInterval->setObjectName("retryInterval");
        retryInterval->setMaximum(5000);

        formLayout->setWidget(5, QFormLayout::FieldRole, retryInterval);


        verticalLayout->addLayout(formLayout);

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
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Actions", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Title", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Button text", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Enable Progress", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Enable Verbose", nullptr));
        progressCheckbox->setText(QString());
        verboseCheckbox->setText(QString());
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Interval", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Retry on failure", nullptr));
        retryOnFailure->setText(QString());
        label_8->setText(QCoreApplication::translate("SettingsDialog", "Retries", nullptr));
        label_9->setText(QCoreApplication::translate("SettingsDialog", "Retry interval", nullptr));
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
