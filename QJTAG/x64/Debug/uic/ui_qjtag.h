/********************************************************************************
** Form generated from reading UI file 'qjtag.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QJTAG_H
#define UI_QJTAG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QJTAGUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QSpacerItem *verticalSpacer;
    QPushButton *execButton;

    void setupUi(MdiChild *QJTAGUI)
    {
        if (QJTAGUI->objectName().isEmpty())
            QJTAGUI->setObjectName("QJTAGUI");
        QJTAGUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QJTAGUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QJTAGUI);
        title->setObjectName("title");

        verticalLayout->addWidget(title, 0, Qt::AlignHCenter);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        execButton = new QPushButton(QJTAGUI);
        execButton->setObjectName("execButton");

        verticalLayout->addWidget(execButton, 0, Qt::AlignHCenter);


        retranslateUi(QJTAGUI);

        QMetaObject::connectSlotsByName(QJTAGUI);
    } // setupUi

    void retranslateUi(MdiChild *QJTAGUI)
    {
        QJTAGUI->setWindowTitle(QCoreApplication::translate("QJTAGUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QJTAGUI", "title", nullptr));
        execButton->setText(QCoreApplication::translate("QJTAGUI", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QJTAGUI: public Ui_QJTAGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QJTAG_H
