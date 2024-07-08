/********************************************************************************
** Form generated from reading UI file 'qlintester.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QLINTESTER_H
#define UI_QLINTESTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLinTesterUI
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *failedLabel;
    QLabel *passedLabel;
    QProgressBar *testProgress;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *testButton;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *QLinTesterUI)
    {
        if (QLinTesterUI->objectName().isEmpty())
            QLinTesterUI->setObjectName("QLinTesterUI");
        QLinTesterUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QLinTesterUI);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(QLinTesterUI);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        failedLabel = new QLabel(groupBox);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(failedLabel);

        passedLabel = new QLabel(groupBox);
        passedLabel->setObjectName("passedLabel");
        passedLabel->setEnabled(false);
        passedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(passedLabel);


        verticalLayout_2->addLayout(horizontalLayout);

        testProgress = new QProgressBar(groupBox);
        testProgress->setObjectName("testProgress");
        testProgress->setValue(0);

        verticalLayout_2->addWidget(testProgress);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        testButton = new QPushButton(groupBox);
        testButton->setObjectName("testButton");

        horizontalLayout_2->addWidget(testButton);

        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName("pushButton");

        horizontalLayout_2->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(groupBox);


        retranslateUi(QLinTesterUI);

        QMetaObject::connectSlotsByName(QLinTesterUI);
    } // setupUi

    void retranslateUi(QWidget *QLinTesterUI)
    {
        QLinTesterUI->setWindowTitle(QCoreApplication::translate("QLinTesterUI", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("QLinTesterUI", "Lin Tester", nullptr));
        failedLabel->setText(QCoreApplication::translate("QLinTesterUI", "FAILED", nullptr));
        passedLabel->setText(QCoreApplication::translate("QLinTesterUI", "PASSED", nullptr));
        testButton->setText(QCoreApplication::translate("QLinTesterUI", "Test", nullptr));
        pushButton->setText(QCoreApplication::translate("QLinTesterUI", "Stop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinTesterUI: public Ui_QLinTesterUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLINTESTER_H
