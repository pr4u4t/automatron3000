/********************************************************************************
** Form generated from reading UI file 'qcustomaction.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QCUSTOMACTION_H
#define UI_QCUSTOMACTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QCustomActionUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QProgressBar *progressBar;
    QHBoxLayout *horizontalLayout;
    QPushButton *pauseButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *stopButton;
    QTextEdit *textEdit;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *exportButton;
    QPushButton *clearButton;

    void setupUi(MdiChild *QCustomActionUI)
    {
        if (QCustomActionUI->objectName().isEmpty())
            QCustomActionUI->setObjectName("QCustomActionUI");
        QCustomActionUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QCustomActionUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QCustomActionUI);
        title->setObjectName("title");
        title->setTextFormat(Qt::TextFormat::MarkdownText);

        verticalLayout->addWidget(title);

        progressBar = new QProgressBar(QCustomActionUI);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pauseButton = new QPushButton(QCustomActionUI);
        pauseButton->setObjectName("pauseButton");

        horizontalLayout->addWidget(pauseButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton = new QPushButton(QCustomActionUI);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        stopButton = new QPushButton(QCustomActionUI);
        stopButton->setObjectName("stopButton");

        horizontalLayout->addWidget(stopButton);


        verticalLayout->addLayout(horizontalLayout);

        textEdit = new QTextEdit(QCustomActionUI);
        textEdit->setObjectName("textEdit");

        verticalLayout->addWidget(textEdit);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        exportButton = new QPushButton(QCustomActionUI);
        exportButton->setObjectName("exportButton");

        horizontalLayout_2->addWidget(exportButton);

        clearButton = new QPushButton(QCustomActionUI);
        clearButton->setObjectName("clearButton");

        horizontalLayout_2->addWidget(clearButton);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(QCustomActionUI);

        QMetaObject::connectSlotsByName(QCustomActionUI);
    } // setupUi

    void retranslateUi(MdiChild *QCustomActionUI)
    {
        QCustomActionUI->setWindowTitle(QCoreApplication::translate("QCustomActionUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QCustomActionUI", "TextLabel", nullptr));
        pauseButton->setText(QCoreApplication::translate("QCustomActionUI", "Pause", nullptr));
        pushButton->setText(QCoreApplication::translate("QCustomActionUI", "Action", nullptr));
        stopButton->setText(QCoreApplication::translate("QCustomActionUI", "Stop", nullptr));
        exportButton->setText(QCoreApplication::translate("QCustomActionUI", "Export", nullptr));
        clearButton->setText(QCoreApplication::translate("QCustomActionUI", "Clear", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QCustomActionUI: public Ui_QCustomActionUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QCUSTOMACTION_H
