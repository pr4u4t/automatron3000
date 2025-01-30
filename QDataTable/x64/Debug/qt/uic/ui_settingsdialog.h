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
#include <QtWidgets/QComboBox>
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
    QLabel *label_3;
    QLineEdit *titleEdit;
    QLabel *label;
    QComboBox *mode;
    QLabel *label_2;
    QLineEdit *separator;
    QLabel *label_4;
    QLineEdit *newLine;
    QTableView *columns;
    QLabel *label_5;
    QLineEdit *input;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *magic;
    QLabel *label_8;
    QSpinBox *maxRows;
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
        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        titleEdit = new QLineEdit(SettingsDialog);
        titleEdit->setObjectName("titleEdit");

        formLayout->setWidget(0, QFormLayout::FieldRole, titleEdit);

        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        mode = new QComboBox(SettingsDialog);
        mode->addItem(QString());
        mode->addItem(QString());
        mode->setObjectName("mode");

        formLayout->setWidget(1, QFormLayout::FieldRole, mode);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_2);

        separator = new QLineEdit(SettingsDialog);
        separator->setObjectName("separator");

        formLayout->setWidget(3, QFormLayout::FieldRole, separator);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        newLine = new QLineEdit(SettingsDialog);
        newLine->setObjectName("newLine");

        formLayout->setWidget(4, QFormLayout::FieldRole, newLine);

        columns = new QTableView(SettingsDialog);
        columns->setObjectName("columns");

        formLayout->setWidget(6, QFormLayout::FieldRole, columns);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_5);

        input = new QLineEdit(SettingsDialog);
        input->setObjectName("input");

        formLayout->setWidget(7, QFormLayout::FieldRole, input);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_7);

        magic = new QLineEdit(SettingsDialog);
        magic->setObjectName("magic");

        formLayout->setWidget(2, QFormLayout::FieldRole, magic);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_8);

        maxRows = new QSpinBox(SettingsDialog);
        maxRows->setObjectName("maxRows");
        maxRows->setMinimum(100);
        maxRows->setMaximum(4000000);

        formLayout->setWidget(5, QFormLayout::FieldRole, maxRows);


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
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Title", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Mode", nullptr));
        mode->setItemText(0, QCoreApplication::translate("SettingsDialog", "Binary", nullptr));
        mode->setItemText(1, QCoreApplication::translate("SettingsDialog", "Text", nullptr));

        label_2->setText(QCoreApplication::translate("SettingsDialog", "Separator", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "New Line", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Columns", nullptr));
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Input", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Magic", nullptr));
        label_8->setText(QCoreApplication::translate("SettingsDialog", "Max Rows", nullptr));
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
