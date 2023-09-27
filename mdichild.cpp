// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include "mdichild.h"
#include "mainwindow.h"

MdiChild::MdiChild(QWidget *parent)
    : QWidget(parent){
    setAttribute(Qt::WA_DeleteOnClose);
}

QSettings& MdiChild::settings(){
    return qobject_cast<MainWindow*>(parent())->settings();
}

void MdiChild::closeEvent(QCloseEvent *event){
    /*if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }*/
}
