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
        pushButton->setText(QCoreApplication::translate("QLinWriteByIDUI", "Write", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinWriteByIDUI: public Ui_QLinWriteByIDUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINWRITEBYID_H
