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

class Ui_QLinReadByIDUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
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

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(QLinReadByIDUI);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        result = new QLabel(QLinReadByIDUI);
        result->setObjectName("result");
        result->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(result);


        verticalLayout->addLayout(horizontalLayout_2);

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
        QLinReadByIDUI->setWindowTitle(QCoreApplication::translate("QLinReadByIDUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QLinReadByIDUI", "Read By ID", nullptr));
        label->setText(QCoreApplication::translate("QLinReadByIDUI", "Result:", nullptr));
        result->setText(QString());
        pushButton->setText(QCoreApplication::translate("QLinReadByIDUI", "Read", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinReadByIDUI: public Ui_QLinReadByIDUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINREADBYID_H
