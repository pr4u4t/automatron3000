/********************************************************************************
** Form generated from reading UI file 'qbadge.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QBADGE_H
#define UI_QBADGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"
#include "ImageDisplay.h"

QT_BEGIN_NAMESPACE

class Ui_QBadgeUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QLabel *label;
    ImageDisplay *image;

    void setupUi(MdiChild *QBadgeUI)
    {
        if (QBadgeUI->objectName().isEmpty())
            QBadgeUI->setObjectName("QBadgeUI");
        QBadgeUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QBadgeUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QBadgeUI);
        title->setObjectName("title");

        verticalLayout->addWidget(title);

        label = new QLabel(QBadgeUI);
        label->setObjectName("label");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        verticalLayout->addWidget(label);

        image = new ImageDisplay(QBadgeUI);
        image->setObjectName("image");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(image->sizePolicy().hasHeightForWidth());
        image->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(image);


        retranslateUi(QBadgeUI);

        QMetaObject::connectSlotsByName(QBadgeUI);
    } // setupUi

    void retranslateUi(MdiChild *QBadgeUI)
    {
        QBadgeUI->setWindowTitle(QCoreApplication::translate("QBadgeUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QBadgeUI", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("QBadgeUI", "TextLabel", nullptr));
        image->setProperty("text", QVariant(QString()));
    } // retranslateUi

};

namespace Ui {
    class QBadgeUI: public Ui_QBadgeUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QBADGE_H
