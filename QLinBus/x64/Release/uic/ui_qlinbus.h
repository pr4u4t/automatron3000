/********************************************************************************
** Form generated from reading UI file 'qlinbus.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QLINBUS_H
#define UI_QLINBUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLinBusUI
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineEdit;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lineEdit_2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QListView *listView;
    QProgressBar *scanProgress;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *QLinBusUI)
    {
        if (QLinBusUI->objectName().isEmpty())
            QLinBusUI->setObjectName("QLinBusUI");
        QLinBusUI->resize(409, 412);
        verticalLayout = new QVBoxLayout(QLinBusUI);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(QLinBusUI);
        groupBox->setObjectName("groupBox");
        horizontalLayout_2 = new QHBoxLayout(groupBox);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName("groupBox_3");
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lineEdit = new QLineEdit(groupBox_3);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout_3->addWidget(lineEdit);


        horizontalLayout_2->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName("groupBox_4");
        horizontalLayout_4 = new QHBoxLayout(groupBox_4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        lineEdit_2 = new QLineEdit(groupBox_4);
        lineEdit_2->setObjectName("lineEdit_2");

        horizontalLayout_4->addWidget(lineEdit_2);


        horizontalLayout_2->addWidget(groupBox_4);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(QLinBusUI);
        groupBox_2->setObjectName("groupBox_2");
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        listView = new QListView(groupBox_2);
        listView->setObjectName("listView");

        verticalLayout_2->addWidget(listView);

        scanProgress = new QProgressBar(groupBox_2);
        scanProgress->setObjectName("scanProgress");
        scanProgress->setAutoFillBackground(false);
        scanProgress->setValue(24);
        scanProgress->setInvertedAppearance(false);
        scanProgress->setTextDirection(QProgressBar::TopToBottom);

        verticalLayout_2->addWidget(scanProgress);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton_2 = new QPushButton(groupBox_2);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout->addWidget(pushButton_2);

        pushButton = new QPushButton(groupBox_2);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(groupBox_2);


        retranslateUi(QLinBusUI);

        QMetaObject::connectSlotsByName(QLinBusUI);
    } // setupUi

    void retranslateUi(QWidget *QLinBusUI)
    {
        QLinBusUI->setWindowTitle(QCoreApplication::translate("QLinBusUI", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("QLinBusUI", "Test", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("QLinBusUI", "Master", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("QLinBusUI", "Slave", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("QLinBusUI", "Scan", nullptr));
        pushButton_2->setText(QCoreApplication::translate("QLinBusUI", "Start", nullptr));
        pushButton->setText(QCoreApplication::translate("QLinBusUI", "Stop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinBusUI: public Ui_QLinBusUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLINBUS_H
