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
#include <QtWidgets/QDoubleSpinBox>
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
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *titleEdit;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QDoubleSpinBox *minValueEdit;
    QDoubleSpinBox *maxValueEdit;
    QDoubleSpinBox *tresholdEdit;
    QSpinBox *precisionEdit;
    QSpinBox *stepsEdit;
    QLineEdit *labelEdit;
    QLineEdit *unitsEdit;
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
        label = new QLabel(SettingsDialog);
        label->setObjectName("label");

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        label_2 = new QLabel(SettingsDialog);
        label_2->setObjectName("label_2");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_2);

        label_3 = new QLabel(SettingsDialog);
        label_3->setObjectName("label_3");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        label_4 = new QLabel(SettingsDialog);
        label_4->setObjectName("label_4");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_4);

        label_5 = new QLabel(SettingsDialog);
        label_5->setObjectName("label_5");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        titleEdit = new QLineEdit(SettingsDialog);
        titleEdit->setObjectName("titleEdit");

        formLayout->setWidget(0, QFormLayout::FieldRole, titleEdit);

        label_6 = new QLabel(SettingsDialog);
        label_6->setObjectName("label_6");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(SettingsDialog);
        label_7->setObjectName("label_7");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        label_8 = new QLabel(SettingsDialog);
        label_8->setObjectName("label_8");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_8);

        minValueEdit = new QDoubleSpinBox(SettingsDialog);
        minValueEdit->setObjectName("minValueEdit");

        formLayout->setWidget(1, QFormLayout::FieldRole, minValueEdit);

        maxValueEdit = new QDoubleSpinBox(SettingsDialog);
        maxValueEdit->setObjectName("maxValueEdit");

        formLayout->setWidget(2, QFormLayout::FieldRole, maxValueEdit);

        tresholdEdit = new QDoubleSpinBox(SettingsDialog);
        tresholdEdit->setObjectName("tresholdEdit");

        formLayout->setWidget(3, QFormLayout::FieldRole, tresholdEdit);

        precisionEdit = new QSpinBox(SettingsDialog);
        precisionEdit->setObjectName("precisionEdit");

        formLayout->setWidget(4, QFormLayout::FieldRole, precisionEdit);

        stepsEdit = new QSpinBox(SettingsDialog);
        stepsEdit->setObjectName("stepsEdit");

        formLayout->setWidget(7, QFormLayout::FieldRole, stepsEdit);

        labelEdit = new QLineEdit(SettingsDialog);
        labelEdit->setObjectName("labelEdit");

        formLayout->setWidget(5, QFormLayout::FieldRole, labelEdit);

        unitsEdit = new QLineEdit(SettingsDialog);
        unitsEdit->setObjectName("unitsEdit");

        formLayout->setWidget(6, QFormLayout::FieldRole, unitsEdit);


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
        label->setText(QCoreApplication::translate("SettingsDialog", "Min Value", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Precision", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Title", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Label", nullptr));
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Max Value", nullptr));
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Treshold", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Units", nullptr));
        label_8->setText(QCoreApplication::translate("SettingsDialog", "Steps", nullptr));
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
