/********************************************************************************
** Form generated from reading UI file 'qweb.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QWEB_H
#define UI_QWEB_H

#include <QtCore/QVariant>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include "../api/api.h"

QT_BEGIN_NAMESPACE

class Ui_QWebUI
{
public:
    QVBoxLayout *verticalLayout;
    QWebEngineView *webView;

    void setupUi(MdiChild *QWebUI)
    {
        if (QWebUI->objectName().isEmpty())
            QWebUI->setObjectName("QWebUI");
        QWebUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QWebUI);
        verticalLayout->setObjectName("verticalLayout");
        webView = new QWebEngineView(QWebUI);
        webView->setObjectName("webView");
        webView->setUrl(QUrl(QString::fromUtf8("about:blank")));

        verticalLayout->addWidget(webView);


        retranslateUi(QWebUI);

        QMetaObject::connectSlotsByName(QWebUI);
    } // setupUi

    void retranslateUi(MdiChild *QWebUI)
    {
        QWebUI->setWindowTitle(QCoreApplication::translate("QWebUI", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QWebUI: public Ui_QWebUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QWEB_H
