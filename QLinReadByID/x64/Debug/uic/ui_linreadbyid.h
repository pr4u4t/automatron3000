/********************************************************************************
** Form generated from reading UI file 'linreadbyid.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINREADBYID_H
#define UI_LINREADBYID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QBadgeUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QLabel *result;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *QLinReadByIDUI)
    {
        if (QLinReadByIDUI->objectName().isEmpty())
            QLinReadByIDUI->setObjectName("QLinReadByIDUI");
        QLinReadByIDUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QLinReadByIDUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QLinReadByIDUI);
        title->setObjectName("title");
        title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(title);

        result = new QLabel(QLinReadByIDUI);
        result->setObjectName("result");
        result->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(result);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(QLinReadByIDUI);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(QLinReadByIDUI);

        QMetaObject::connectSlotsByName(QLinReadByIDUI);
    } // setupUi

    void retranslateUi(QWidget *QLinReadByIDUI)
    {
        QLinReadByIDUI->setWindowTitle(QCoreApplication::translate("QBadgeUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QBadgeUI", "TextLabel", nullptr));
        result->setText(QCoreApplication::translate("QBadgeUI", "TextLabel", nullptr));
        pushButton->setText(QCoreApplication::translate("QBadgeUI", "Read", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QBadgeUI: public Ui_QBadgeUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINREADBYID_H
