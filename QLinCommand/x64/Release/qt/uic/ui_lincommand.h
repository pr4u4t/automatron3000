/********************************************************************************
** Form generated from reading UI file 'lincommand.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINCOMMAND_H
#define UI_LINCOMMAND_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLinCommandUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QLabel *failedLabel;
    QLabel *progressLabel;
    QLabel *successLabel;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *QLinCommandUI)
    {
        if (QLinCommandUI->objectName().isEmpty())
            QLinCommandUI->setObjectName("QLinCommandUI");
        QLinCommandUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QLinCommandUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QLinCommandUI);
        title->setObjectName("title");
        title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(title);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        failedLabel = new QLabel(QLinCommandUI);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setStyleSheet(QString::fromUtf8("font-weight: bold;"));
        failedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(failedLabel);

        progressLabel = new QLabel(QLinCommandUI);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setEnabled(false);
        progressLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        progressLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(progressLabel);

        successLabel = new QLabel(QLinCommandUI);
        successLabel->setObjectName("successLabel");
        successLabel->setEnabled(false);
        successLabel->setStyleSheet(QString::fromUtf8("font-weight: bold;"));
        successLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(successLabel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(QLinCommandUI);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(QLinCommandUI);

        QMetaObject::connectSlotsByName(QLinCommandUI);
    } // setupUi

    void retranslateUi(QWidget *QLinCommandUI)
    {
        QLinCommandUI->setWindowTitle(QCoreApplication::translate("QLinCommandUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QLinCommandUI", "Command", nullptr));
        failedLabel->setText(QCoreApplication::translate("QLinCommandUI", "FAILED", nullptr));
        progressLabel->setText(QCoreApplication::translate("QLinCommandUI", "IN PROGRESS", nullptr));
        successLabel->setText(QCoreApplication::translate("QLinCommandUI", "SUCCESS", nullptr));
        pushButton->setText(QCoreApplication::translate("QLinCommandUI", "Execute", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinCommandUI: public Ui_QLinCommandUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINCOMMAND_H
