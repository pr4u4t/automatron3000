/********************************************************************************
** Form generated from reading UI file 'qjtag.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QJTAG_H
#define UI_QJTAG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QJTAGUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QLabel *failedLabel;
    QLabel *progressLabel;
    QLabel *successLabel;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QPushButton *execButton;

    void setupUi(MdiChild *QJTAGUI)
    {
        if (QJTAGUI->objectName().isEmpty())
            QJTAGUI->setObjectName("QJTAGUI");
        QJTAGUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QJTAGUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QJTAGUI);
        title->setObjectName("title");
        title->setEnabled(true);
        title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(title, 0, Qt::AlignHCenter);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        failedLabel = new QLabel(QJTAGUI);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        failedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(failedLabel);

        progressLabel = new QLabel(QJTAGUI);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setEnabled(false);
        progressLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        progressLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(progressLabel);

        successLabel = new QLabel(QJTAGUI);
        successLabel->setObjectName("successLabel");
        successLabel->setEnabled(false);
        successLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        successLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(successLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        execButton = new QPushButton(QJTAGUI);
        execButton->setObjectName("execButton");
        execButton->setEnabled(true);

        verticalLayout->addWidget(execButton, 0, Qt::AlignHCenter);


        retranslateUi(QJTAGUI);

        QMetaObject::connectSlotsByName(QJTAGUI);
    } // setupUi

    void retranslateUi(MdiChild *QJTAGUI)
    {
        QJTAGUI->setWindowTitle(QCoreApplication::translate("QJTAGUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QJTAGUI", "title", nullptr));
        failedLabel->setText(QCoreApplication::translate("QJTAGUI", "FAILED", nullptr));
        progressLabel->setText(QCoreApplication::translate("QJTAGUI", "IN PROGRESS", nullptr));
        successLabel->setText(QCoreApplication::translate("QJTAGUI", "SUCCESS", nullptr));
        execButton->setText(QCoreApplication::translate("QJTAGUI", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QJTAGUI: public Ui_QJTAGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QJTAG_H
