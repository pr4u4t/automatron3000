// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include "mdichild.h"
#include "mainwindow.h"

MdiChild::MdiChild(QWidget *parent, QWidget *mwin)
    : QWidget(parent),
    m_mainWindow(mwin){
    setAttribute(Qt::WA_DeleteOnClose);
}

QSettings& MdiChild::settings(){
    return qobject_cast<MainWindow*>(mainWindow())->settings();
}

QWidget *MdiChild::mainWindow(){
    return m_mainWindow;
}

void MdiChild::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);
    qDebug() << "MdiChild::closeEvent";
    /*if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }*/
}
