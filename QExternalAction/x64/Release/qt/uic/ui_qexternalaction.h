/********************************************************************************
** Form generated from reading UI file 'qexternalaction.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QEXTERNALACTION_H
#define UI_QEXTERNALACTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QExternalActionUI
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

    void setupUi(MdiChild *QExternalActionUI)
    {
        if (QExternalActionUI->objectName().isEmpty())
            QExternalActionUI->setObjectName("QExternalActionUI");
        QExternalActionUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QExternalActionUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QExternalActionUI);
        title->setObjectName("title");
        title->setEnabled(true);
        title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(title, 0, Qt::AlignHCenter);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        failedLabel = new QLabel(QExternalActionUI);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        failedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(failedLabel);

        progressLabel = new QLabel(QExternalActionUI);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setEnabled(false);
        progressLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        progressLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(progressLabel);

        successLabel = new QLabel(QExternalActionUI);
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

        execButton = new QPushButton(QExternalActionUI);
        execButton->setObjectName("execButton");
        execButton->setEnabled(true);

        verticalLayout->addWidget(execButton, 0, Qt::AlignHCenter);


        retranslateUi(QExternalActionUI);

        QMetaObject::connectSlotsByName(QExternalActionUI);
    } // setupUi

    void retranslateUi(MdiChild *QExternalActionUI)
    {
        QExternalActionUI->setWindowTitle(QCoreApplication::translate("QExternalActionUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QExternalActionUI", "title", nullptr));
        failedLabel->setText(QCoreApplication::translate("QExternalActionUI", "FAILED", nullptr));
        progressLabel->setText(QCoreApplication::translate("QExternalActionUI", "IN PROGRESS", nullptr));
        successLabel->setText(QCoreApplication::translate("QExternalActionUI", "SUCCESS", nullptr));
        execButton->setText(QCoreApplication::translate("QExternalActionUI", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QExternalActionUI: public Ui_QExternalActionUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QEXTERNALACTION_H
