/********************************************************************************
** Form generated from reading UI file 'linreadbyid.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINREADBYID_H
#define UI_LINREADBYID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
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
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QLabel *failedLabel;
    QLabel *progressLabel;
    QLabel *successLabel;
    QSpacerItem *horizontalSpacer_4;
    QFormLayout *formLayout_2;
    QHBoxLayout *horizontalLayout_8;
    QLabel *hexLabel;
    QLabel *lenLabel;
    QLabel *asciiLabel;
    QLabel *resultAscii;
    QLineEdit *result;
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
        title->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(title);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        failedLabel = new QLabel(QLinReadByIDUI);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        failedLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(failedLabel);

        progressLabel = new QLabel(QLinReadByIDUI);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setEnabled(false);
        progressLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        progressLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(progressLabel);

        successLabel = new QLabel(QLinReadByIDUI);
        successLabel->setObjectName("successLabel");
        successLabel->setEnabled(false);
        successLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        successLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(successLabel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_3);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName("formLayout_2");
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        hexLabel = new QLabel(QLinReadByIDUI);
        hexLabel->setObjectName("hexLabel");

        horizontalLayout_8->addWidget(hexLabel);

        lenLabel = new QLabel(QLinReadByIDUI);
        lenLabel->setObjectName("lenLabel");

        horizontalLayout_8->addWidget(lenLabel);


        formLayout_2->setLayout(0, QFormLayout::LabelRole, horizontalLayout_8);

        asciiLabel = new QLabel(QLinReadByIDUI);
        asciiLabel->setObjectName("asciiLabel");

        formLayout_2->setWidget(1, QFormLayout::LabelRole, asciiLabel);

        resultAscii = new QLabel(QLinReadByIDUI);
        resultAscii->setObjectName("resultAscii");

        formLayout_2->setWidget(1, QFormLayout::FieldRole, resultAscii);

        result = new QLineEdit(QLinReadByIDUI);
        result->setObjectName("result");

        formLayout_2->setWidget(0, QFormLayout::FieldRole, result);


        verticalLayout->addLayout(formLayout_2);

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
        failedLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "FAILED", nullptr));
        progressLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "IN PROGRESS", nullptr));
        successLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "SUCCESS", nullptr));
        hexLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "HEX", nullptr));
        lenLabel->setText(QCoreApplication::translate("QLinReadByIDUI", ":", nullptr));
        asciiLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "ASCII: ", nullptr));
        resultAscii->setText(QString());
        pushButton->setText(QCoreApplication::translate("QLinReadByIDUI", "Read", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinReadByIDUI: public Ui_QLinReadByIDUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINREADBYID_H
