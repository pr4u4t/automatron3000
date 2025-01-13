// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include "mdichild.h"
#include "window.h"
#include "plugin.h"

MdiChild::MdiChild(QWidget* parent)
    : QWidget(){//(parent){
    //setAttribute(Qt::WA_DeleteOnClose);
}

void MdiChild::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);
    emit message("MdiChild::closeEvent");
}
