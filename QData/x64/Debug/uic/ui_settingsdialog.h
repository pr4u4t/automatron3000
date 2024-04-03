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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *dbDriverLabel;
    QComboBox *dbDriver;
    QLabel *dbURILabel;
    QLineEdit *dbUri;
    QLabel *label;
    QLineEdit *dbName;
    QLabel *label_8;
    QLineEdit *dbTable;
    QLabel *label_3;
    QComboBox *dbLock;
    QLabel *label_4;
    QLineEdit *dbLockPass;
    QLabel *label_2;
    QLineEdit *dbLockPassConfirm;
    QLabel *label_5;
    QLineEdit *codeRegexp;
    QLabel *label_9;
    QLineEdit *removeChars;
    QLabel *label_6;
    QLineEdit *serialPrefix;
    QLabel *label_7;
    QLineEdit *serialInterval;
    QLabel *label_10;
    QComboBox *omitZeros;
    QLabel *label_11;
    QLineEdit *clearCode;
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
        SettingsDialog->resize(778, 492);
        verticalLayout_2 = new QVBoxLayout(SettingsDialog);
        verticalLayout_2->setObjectName("verticalLayout_2");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        dbDriverLabel = new QLabel(SettingsDialog);
        dbDriverLabel->setObjectName("dbDriverLabel");

        formLayout->setWidget(0, QFormLayout::LabelRole, dbDriverLabel);

        dbDriver = new QComboBox(SettingsDialog);
        dbDriver->addItem(QString());
        dbDriver->addItem(QString());
        dbDriver->setObjectName("dbDriver");

        formLayout->setWidget(0, QFormLayout::FieldRole, dbDriver);

        dbURILabel = new QLabel(SettingsDialog);
        dbURILabel->setObjectName("dbURILabel");

        formLayout->setWidget(1, QFormLayout::LabelRole, dbURILabel);

        dbUri = new QLineEdit(SettingsDialog);
        dbUri->setObjectName("dbUri");

        formLayout->setWidget(1, QFormLayout::FieldRole, dbUri);

        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        dbName = new QLineEdit(SettingsDialog);
        dbName->setObjectName("dbName");

        formLayout->setWidget(2, QFormLayout::FieldRole, dbName);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_8);

        dbTable = new QLineEdit(SettingsDialog);
        dbTable->setObjectName("dbTable");

        formLayout->setWidget(3, QFormLayout::FieldRole, dbTable);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_3);

        dbLock = new QComboBox(SettingsDialog);
        dbLock->addItem(QString());
        dbLock->addItem(QString());
        dbLock->setObjectName("dbLock");

        formLayout->setWidget(4, QFormLayout::FieldRole, dbLock);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_4);

        dbLockPass = new QLineEdit(SettingsDialog);
        dbLockPass->setObjectName("dbLockPass");
        dbLockPass->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(5, QFormLayout::FieldRole, dbLockPass);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_2);

        dbLockPassConfirm = new QLineEdit(SettingsDialog);
        dbLockPassConfirm->setObjectName("dbLockPassConfirm");
        dbLockPassConfirm->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(6, QFormLayout::FieldRole, dbLockPassConfirm);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_5);

        codeRegexp = new QLineEdit(SettingsDialog);
        codeRegexp->setObjectName("codeRegexp");

        formLayout->setWidget(7, QFormLayout::FieldRole, codeRegexp);

        label_9 = new QLabel(SettingsDialog);
        label_9->setObjectName("label_9");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_9);

        removeChars = new QLineEdit(SettingsDialog);
        removeChars->setObjectName("removeChars");

        formLayout->setWidget(8, QFormLayout::FieldRole, removeChars);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_6);

        serialPrefix = new QLineEdit(SettingsDialog);
        serialPrefix->setObjectName("serialPrefix");

        formLayout->setWidget(9, QFormLayout::FieldRole, serialPrefix);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(10, QFormLayout::LabelRole, label_7);

        serialInterval = new QLineEdit(SettingsDialog);
        serialInterval->setObjectName("serialInterval");

        formLayout->setWidget(10, QFormLayout::FieldRole, serialInterval);

        label_10 = new QLabel(SettingsDialog);
        label_10->setObjectName("label_10");

        formLayout->setWidget(11, QFormLayout::LabelRole, label_10);

        omitZeros = new QComboBox(SettingsDialog);
        omitZeros->addItem(QString());
        omitZeros->addItem(QString());
        omitZeros->setObjectName("omitZeros");

        formLayout->setWidget(11, QFormLayout::FieldRole, omitZeros);

        label_11 = new QLabel(SettingsDialog);
        label_11->setObjectName("label_11");

        formLayout->setWidget(12, QFormLayout::LabelRole, label_11);

        clearCode = new QLineEdit(SettingsDialog);
        clearCode->setObjectName("clearCode");

        formLayout->setWidget(12, QFormLayout::FieldRole, clearCode);


        verticalLayout_2->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
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


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(MdiChild *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Form", nullptr));
        dbDriverLabel->setText(QCoreApplication::translate("SettingsDialog", "Database Driver", nullptr));
        dbDriver->setItemText(0, QCoreApplication::translate("SettingsDialog", "QSQLITE", nullptr));
        dbDriver->setItemText(1, QCoreApplication::translate("SettingsDialog", "QPSQL", nullptr));

        dbURILabel->setText(QCoreApplication::translate("SettingsDialog", "Database URI", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Database Name", nullptr));
        label_8->setText(QCoreApplication::translate("SettingsDialog", "Database Table", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Lock Database", nullptr));
        dbLock->setItemText(0, QCoreApplication::translate("SettingsDialog", "No", nullptr));
        dbLock->setItemText(1, QCoreApplication::translate("SettingsDialog", "Yes", nullptr));

        label_4->setText(QCoreApplication::translate("SettingsDialog", "Database Password", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Confirm Password", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Barcode Regexp", nullptr));
        label_9->setText(QCoreApplication::translate("SettingsDialog", "Barcode Remove Chars", nullptr));
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Serial Prefix", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Interval", nullptr));
        label_10->setText(QCoreApplication::translate("SettingsDialog", "Omit Zeros", nullptr));
        omitZeros->setItemText(0, QCoreApplication::translate("SettingsDialog", "Yes", nullptr));
        omitZeros->setItemText(1, QCoreApplication::translate("SettingsDialog", "No", nullptr));

        label_11->setText(QCoreApplication::translate("SettingsDialog", "Clear code", nullptr));
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
