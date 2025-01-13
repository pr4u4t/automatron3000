/********************************************************************************
** Form generated from reading UI file 'qdataui.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QDATAUI_H
#define UI_QDATAUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QDataUI
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QTableView *dbView;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *barcodeEdit;
    QPushButton *searchButton;
    QPushButton *clearButton;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QListView *historyView;
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *sideLabel;
    QLabel *label_3;
    QLabel *shelfLabel;
    QLabel *label_5;
    QLabel *rackLabel;
    QHBoxLayout *horizontalLayout_5;
    QLabel *left;
    QLabel *right;
    QHBoxLayout *horizontalLayout;
    QGroupBox *exportImportGroup;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *importCsvButton;
    QPushButton *importDirButton;
    QPushButton *exportCSVButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *unlockButton;

    void setupUi(QWidget *QDataUI)
    {
        if (QDataUI->objectName().isEmpty())
            QDataUI->setObjectName("QDataUI");
        QDataUI->resize(1005, 609);
        verticalLayout = new QVBoxLayout(QDataUI);
        verticalLayout->setObjectName("verticalLayout");
        splitter = new QSplitter(QDataUI);
        splitter->setObjectName("splitter");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Horizontal);
        groupBox = new QGroupBox(splitter);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        dbView = new QTableView(groupBox);
        dbView->setObjectName("dbView");
        dbView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        dbView->setAlternatingRowColors(true);
        dbView->setSelectionMode(QAbstractItemView::SingleSelection);
        dbView->setSelectionBehavior(QAbstractItemView::SelectRows);
        dbView->setSortingEnabled(true);

        verticalLayout_2->addWidget(dbView);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        horizontalLayout_3->addWidget(label_2);

        barcodeEdit = new QLineEdit(groupBox);
        barcodeEdit->setObjectName("barcodeEdit");

        horizontalLayout_3->addWidget(barcodeEdit);

        searchButton = new QPushButton(groupBox);
        searchButton->setObjectName("searchButton");

        horizontalLayout_3->addWidget(searchButton);

        clearButton = new QPushButton(groupBox);
        clearButton->setObjectName("clearButton");

        horizontalLayout_3->addWidget(clearButton);


        verticalLayout_2->addLayout(horizontalLayout_3);

        splitter->addWidget(groupBox);
        groupBox_2 = new QGroupBox(splitter);
        groupBox_2->setObjectName("groupBox_2");
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        historyView = new QListView(groupBox_2);
        historyView->setObjectName("historyView");

        verticalLayout_3->addWidget(historyView);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox_2);
        label->setObjectName("label");

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        sideLabel = new QLabel(groupBox_2);
        sideLabel->setObjectName("sideLabel");

        formLayout->setWidget(2, QFormLayout::FieldRole, sideLabel);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        shelfLabel = new QLabel(groupBox_2);
        shelfLabel->setObjectName("shelfLabel");

        formLayout->setWidget(1, QFormLayout::FieldRole, shelfLabel);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_5);

        rackLabel = new QLabel(groupBox_2);
        rackLabel->setObjectName("rackLabel");

        formLayout->setWidget(0, QFormLayout::FieldRole, rackLabel);


        verticalLayout_3->addLayout(formLayout);

        splitter->addWidget(groupBox_2);

        verticalLayout->addWidget(splitter);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        left = new QLabel(QDataUI);
        left->setObjectName("left");
        left->setEnabled(true);
        left->setPixmap(QPixmap(QString::fromUtf8("left-arrow.png")));
        left->setScaledContents(false);
        left->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(left);

        right = new QLabel(QDataUI);
        right->setObjectName("right");
        right->setEnabled(true);
        right->setPixmap(QPixmap(QString::fromUtf8("right-arrow.png")));
        right->setScaledContents(false);
        right->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(right);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        exportImportGroup = new QGroupBox(QDataUI);
        exportImportGroup->setObjectName("exportImportGroup");
        horizontalLayout_2 = new QHBoxLayout(exportImportGroup);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        importCsvButton = new QPushButton(exportImportGroup);
        importCsvButton->setObjectName("importCsvButton");
        importCsvButton->setEnabled(false);

        horizontalLayout_2->addWidget(importCsvButton);

        importDirButton = new QPushButton(exportImportGroup);
        importDirButton->setObjectName("importDirButton");
        importDirButton->setEnabled(false);

        horizontalLayout_2->addWidget(importDirButton);

        exportCSVButton = new QPushButton(exportImportGroup);
        exportCSVButton->setObjectName("exportCSVButton");
        exportCSVButton->setEnabled(false);

        horizontalLayout_2->addWidget(exportCSVButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        unlockButton = new QPushButton(exportImportGroup);
        unlockButton->setObjectName("unlockButton");

        horizontalLayout_2->addWidget(unlockButton);


        horizontalLayout->addWidget(exportImportGroup);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(QDataUI);

        QMetaObject::connectSlotsByName(QDataUI);
    } // setupUi

    void retranslateUi(QWidget *QDataUI)
    {
        QDataUI->setWindowTitle(QCoreApplication::translate("QDataUI", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("QDataUI", "Database", nullptr));
        label_2->setText(QCoreApplication::translate("QDataUI", "Barcode:", nullptr));
        searchButton->setText(QCoreApplication::translate("QDataUI", "Search", nullptr));
        clearButton->setText(QCoreApplication::translate("QDataUI", "Clear", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("QDataUI", "History", nullptr));
        label->setText(QCoreApplication::translate("QDataUI", "Side:", nullptr));
        sideLabel->setText(QString());
        label_3->setText(QCoreApplication::translate("QDataUI", "Shelf:", nullptr));
        shelfLabel->setText(QString());
        label_5->setText(QCoreApplication::translate("QDataUI", "Rack:", nullptr));
        rackLabel->setText(QString());
        left->setText(QString());
        right->setText(QString());
        exportImportGroup->setTitle(QCoreApplication::translate("QDataUI", "Import/Export", nullptr));
        importCsvButton->setText(QCoreApplication::translate("QDataUI", "Import CSV", nullptr));
        importDirButton->setText(QCoreApplication::translate("QDataUI", "Import Dir", nullptr));
        exportCSVButton->setText(QCoreApplication::translate("QDataUI", "Export CSV", nullptr));
        unlockButton->setText(QCoreApplication::translate("QDataUI", "Unlock", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QDataUI: public Ui_QDataUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDATAUI_H
