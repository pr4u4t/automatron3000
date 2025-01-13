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
        SettingsDialog->resize(492, 369);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_2);

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

        formLayout->setWidget(5, QFormLayout::FieldRole, tableView);

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

        formLayout->setWidget(4, QFormLayout::LabelRole, label_6);

        intervalSpin = new QSpinBox(SettingsDialog);
        intervalSpin->setObjectName("intervalSpin");

        formLayout->setWidget(4, QFormLayout::FieldRole, intervalSpin);


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
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Actions", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Title", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Button text", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Enable Progress", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Enable Verbose", nullptr));
        progressCheckbox->setText(QString());
        verboseCheckbox->setText(QString());
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Interval", nullptr));
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
