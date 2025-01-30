/********************************************************************************
** Form generated from reading UI file 'preferences.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCES_H
#define UI_PREFERENCES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Preferences
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QTreeView *treeView;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *ok;
    QPushButton *apply;
    QPushButton *cancel;

    void setupUi(QWidget *Preferences)
    {
        if (Preferences->objectName().isEmpty())
            Preferences->setObjectName("Preferences");
        Preferences->resize(698, 426);
        verticalLayout = new QVBoxLayout(Preferences);
        verticalLayout->setObjectName("verticalLayout");
        splitter = new QSplitter(Preferences);
        splitter->setObjectName("splitter");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Horizontal);
        treeView = new QTreeView(splitter);
        treeView->setObjectName("treeView");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(treeView->sizePolicy().hasHeightForWidth());
        treeView->setSizePolicy(sizePolicy1);
        treeView->setMinimumSize(QSize(150, 258));
        splitter->addWidget(treeView);
        scrollArea = new QScrollArea(splitter);
        scrollArea->setObjectName("scrollArea");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(3);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy2);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 416, 374));
        scrollArea->setWidget(scrollAreaWidgetContents);
        splitter->addWidget(scrollArea);

        verticalLayout->addWidget(splitter);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        ok = new QPushButton(Preferences);
        ok->setObjectName("ok");

        horizontalLayout_2->addWidget(ok);

        apply = new QPushButton(Preferences);
        apply->setObjectName("apply");

        horizontalLayout_2->addWidget(apply);

        cancel = new QPushButton(Preferences);
        cancel->setObjectName("cancel");

        horizontalLayout_2->addWidget(cancel);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(Preferences);

        QMetaObject::connectSlotsByName(Preferences);
    } // setupUi

    void retranslateUi(QWidget *Preferences)
    {
        Preferences->setWindowTitle(QCoreApplication::translate("Preferences", "Form", nullptr));
        ok->setText(QCoreApplication::translate("Preferences", "OK", nullptr));
        apply->setText(QCoreApplication::translate("Preferences", "Apply", nullptr));
        cancel->setText(QCoreApplication::translate("Preferences", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Preferences: public Ui_Preferences {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCES_H
