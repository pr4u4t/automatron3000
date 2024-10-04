/********************************************************************************
** Form generated from reading UI file 'qcustomaction.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QCUSTOMACTION_H
#define UI_QCUSTOMACTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QCustomActionUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;

    void setupUi(MdiChild *QCustomActionUI)
    {
        if (QCustomActionUI->objectName().isEmpty())
            QCustomActionUI->setObjectName("QCustomActionUI");
        QCustomActionUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QCustomActionUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QCustomActionUI);
        title->setObjectName("title");
        title->setTextFormat(Qt::MarkdownText);

        verticalLayout->addWidget(title);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton = new QPushButton(QCustomActionUI);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(QCustomActionUI);

        QMetaObject::connectSlotsByName(QCustomActionUI);
    } // setupUi

    void retranslateUi(MdiChild *QCustomActionUI)
    {
        QCustomActionUI->setWindowTitle(QCoreApplication::translate("QCustomActionUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QCustomActionUI", "TextLabel", nullptr));
        pushButton->setText(QCoreApplication::translate("QCustomActionUI", "Action", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QCustomActionUI: public Ui_QCustomActionUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QCUSTOMACTION_H
