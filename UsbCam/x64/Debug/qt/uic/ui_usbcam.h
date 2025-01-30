/********************************************************************************
** Form generated from reading UI file 'usbcam.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USBCAM_H
#define UI_USBCAM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"
#include "ImageDisplay.h"

QT_BEGIN_NAMESPACE

class Ui_UsbCamUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QLabel *label;
    ImageDisplay *image;

    void setupUi(MdiChild *UsbCamUI)
    {
        if (UsbCamUI->objectName().isEmpty())
            UsbCamUI->setObjectName("UsbCamUI");
        UsbCamUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(UsbCamUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(UsbCamUI);
        title->setObjectName("title");
        title->setTextFormat(Qt::MarkdownText);

        verticalLayout->addWidget(title);

        label = new QLabel(UsbCamUI);
        label->setObjectName("label");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setTextFormat(Qt::MarkdownText);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        verticalLayout->addWidget(label);

        image = new ImageDisplay(UsbCamUI);
        image->setObjectName("image");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(image->sizePolicy().hasHeightForWidth());
        image->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(image);


        retranslateUi(UsbCamUI);

        QMetaObject::connectSlotsByName(UsbCamUI);
    } // setupUi

    void retranslateUi(MdiChild *UsbCamUI)
    {
        UsbCamUI->setWindowTitle(QCoreApplication::translate("UsbCamUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("UsbCamUI", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("UsbCamUI", "TextLabel", nullptr));
        image->setProperty("text", QVariant(QString()));
    } // retranslateUi

};

namespace Ui {
    class UsbCamUI: public Ui_UsbCamUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USBCAM_H
