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
#include <QtGui/QIcon>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_UsbCamUI
{
public:
    QHBoxLayout *horizontalLayout;
    QStackedWidget *stackedWidget;
    QWidget *viewfinderPage;
    QGridLayout *gridLayout_5;
    QVideoWidget *viewfinder;
    QWidget *previewPage;
    QGridLayout *gridLayout_4;
    QLabel *lastImagePreviewLabel;
    QTabWidget *captureWidget;
    QWidget *tab_2;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer_2;
    QPushButton *takeImageButton;
    QSlider *exposureCompensation;
    QLabel *label;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QPushButton *recordButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QSpacerItem *verticalSpacer;
    QPushButton *muteButton;
    QPushButton *metaDataButton;

    void setupUi(MdiChild *UsbCamUI)
    {
        if (UsbCamUI->objectName().isEmpty())
            UsbCamUI->setObjectName("UsbCamUI");
        UsbCamUI->resize(745, 379);
        horizontalLayout = new QHBoxLayout(UsbCamUI);
        horizontalLayout->setObjectName("horizontalLayout");
        stackedWidget = new QStackedWidget(UsbCamUI);
        stackedWidget->setObjectName("stackedWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(145, 145, 145, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        stackedWidget->setPalette(palette);
        viewfinderPage = new QWidget();
        viewfinderPage->setObjectName("viewfinderPage");
        gridLayout_5 = new QGridLayout(viewfinderPage);
        gridLayout_5->setObjectName("gridLayout_5");
        viewfinder = new QVideoWidget(viewfinderPage);
        viewfinder->setObjectName("viewfinder");

        gridLayout_5->addWidget(viewfinder, 0, 0, 1, 1);

        stackedWidget->addWidget(viewfinderPage);
        previewPage = new QWidget();
        previewPage->setObjectName("previewPage");
        gridLayout_4 = new QGridLayout(previewPage);
        gridLayout_4->setObjectName("gridLayout_4");
        lastImagePreviewLabel = new QLabel(previewPage);
        lastImagePreviewLabel->setObjectName("lastImagePreviewLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lastImagePreviewLabel->sizePolicy().hasHeightForWidth());
        lastImagePreviewLabel->setSizePolicy(sizePolicy1);
        lastImagePreviewLabel->setFrameShape(QFrame::Shape::Box);

        gridLayout_4->addWidget(lastImagePreviewLabel, 0, 0, 1, 1);

        stackedWidget->addWidget(previewPage);

        horizontalLayout->addWidget(stackedWidget);

        captureWidget = new QTabWidget(UsbCamUI);
        captureWidget->setObjectName("captureWidget");
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        gridLayout = new QGridLayout(tab_2);
        gridLayout->setObjectName("gridLayout");
        verticalSpacer_2 = new QSpacerItem(20, 161, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 3, 0, 1, 1);

        takeImageButton = new QPushButton(tab_2);
        takeImageButton->setObjectName("takeImageButton");
        takeImageButton->setEnabled(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/shutter.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        takeImageButton->setIcon(icon);

        gridLayout->addWidget(takeImageButton, 0, 0, 1, 1);

        exposureCompensation = new QSlider(tab_2);
        exposureCompensation->setObjectName("exposureCompensation");
        exposureCompensation->setMinimum(-4);
        exposureCompensation->setMaximum(4);
        exposureCompensation->setPageStep(2);
        exposureCompensation->setOrientation(Qt::Orientation::Horizontal);
        exposureCompensation->setTickPosition(QSlider::TickPosition::TicksAbove);

        gridLayout->addWidget(exposureCompensation, 5, 0, 1, 1);

        label = new QLabel(tab_2);
        label->setObjectName("label");

        gridLayout->addWidget(label, 4, 0, 1, 1);

        captureWidget->addTab(tab_2, QString());
        tab = new QWidget();
        tab->setObjectName("tab");
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName("gridLayout_2");
        recordButton = new QPushButton(tab);
        recordButton->setObjectName("recordButton");

        gridLayout_2->addWidget(recordButton, 0, 0, 1, 1);

        pauseButton = new QPushButton(tab);
        pauseButton->setObjectName("pauseButton");

        gridLayout_2->addWidget(pauseButton, 1, 0, 1, 1);

        stopButton = new QPushButton(tab);
        stopButton->setObjectName("stopButton");

        gridLayout_2->addWidget(stopButton, 2, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 76, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 3, 0, 1, 1);

        muteButton = new QPushButton(tab);
        muteButton->setObjectName("muteButton");
        muteButton->setCheckable(true);

        gridLayout_2->addWidget(muteButton, 4, 0, 1, 1);

        metaDataButton = new QPushButton(tab);
        metaDataButton->setObjectName("metaDataButton");
        metaDataButton->setCheckable(true);

        gridLayout_2->addWidget(metaDataButton, 5, 0, 1, 1);

        captureWidget->addTab(tab, QString());

        horizontalLayout->addWidget(captureWidget);


        retranslateUi(UsbCamUI);

        stackedWidget->setCurrentIndex(0);
        captureWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(UsbCamUI);
    } // setupUi

    void retranslateUi(MdiChild *UsbCamUI)
    {
        UsbCamUI->setWindowTitle(QCoreApplication::translate("UsbCamUI", "Form", nullptr));
        lastImagePreviewLabel->setText(QString());
        takeImageButton->setText(QCoreApplication::translate("UsbCamUI", "Capture Photo", nullptr));
        label->setText(QCoreApplication::translate("UsbCamUI", "Exposure Compensation:", nullptr));
        captureWidget->setTabText(captureWidget->indexOf(tab_2), QCoreApplication::translate("UsbCamUI", "Image", nullptr));
        recordButton->setText(QCoreApplication::translate("UsbCamUI", "Record", nullptr));
        pauseButton->setText(QCoreApplication::translate("UsbCamUI", "Pause", nullptr));
        stopButton->setText(QCoreApplication::translate("UsbCamUI", "Stop", nullptr));
        muteButton->setText(QCoreApplication::translate("UsbCamUI", "Mute", nullptr));
        metaDataButton->setText(QCoreApplication::translate("UsbCamUI", "Set metadata", nullptr));
        captureWidget->setTabText(captureWidget->indexOf(tab), QCoreApplication::translate("UsbCamUI", "Video", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UsbCamUI: public Ui_UsbCamUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USBCAM_H
