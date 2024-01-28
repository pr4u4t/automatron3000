// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include "mdichild.h"
#include "window.h"

MdiChild::MdiChild(Plugin *parent, QWidget *mwin)
    : m_plugin(parent),
    QWidget(mwin),
    m_mainWindow(mwin){
    setAttribute(Qt::WA_DeleteOnClose);
}

QSettings& MdiChild::settings(){
    return dynamic_cast<Window*>(mainWindow())->settings();
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
