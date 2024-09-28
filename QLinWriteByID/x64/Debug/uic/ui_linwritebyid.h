/********************************************************************************
** Form generated from reading UI file 'linwritebyid.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINWRITEBYID_H
#define UI_LINWRITEBYID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLinWriteByIDUI
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
    QLineEdit *valueEdit;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *QLinWriteByIDUI)
    {
        if (QLinWriteByIDUI->objectName().isEmpty())
            QLinWriteByIDUI->setObjectName("QLinWriteByIDUI");
        QLinWriteByIDUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QLinWriteByIDUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QLinWriteByIDUI);
        title->setObjectName("title");
        title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(title);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        failedLabel = new QLabel(QLinWriteByIDUI);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        failedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(failedLabel);

        progressLabel = new QLabel(QLinWriteByIDUI);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setEnabled(false);
        progressLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        progressLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(progressLabel);

        successLabel = new QLabel(QLinWriteByIDUI);
        successLabel->setObjectName("successLabel");
        successLabel->setEnabled(false);
        successLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        successLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(successLabel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_2);

        valueEdit = new QLineEdit(QLinWriteByIDUI);
        valueEdit->setObjectName("valueEdit");

        verticalLayout->addWidget(valueEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(QLinWriteByIDUI);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(QLinWriteByIDUI);

        QMetaObject::connectSlotsByName(QLinWriteByIDUI);
    } // setupUi

    void retranslateUi(QWidget *QLinWriteByIDUI)
    {
        QLinWriteByIDUI->setWindowTitle(QCoreApplication::translate("QLinWriteByIDUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QLinWriteByIDUI", "TextLabel", nullptr));
        failedLabel->setText(QCoreApplication::translate("QLinWriteByIDUI", "FAILED", nullptr));
        progressLabel->setText(QCoreApplication::translate("QLinWriteByIDUI", "IN PROGRESS", nullptr));
        successLabel->setText(QCoreApplication::translate("QLinWriteByIDUI", "SUCCESS", nullptr));
        pushButton->setText(QCoreApplication::translate("QLinWriteByIDUI", "Write", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinWriteByIDUI: public Ui_QLinWriteByIDUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINWRITEBYID_H
