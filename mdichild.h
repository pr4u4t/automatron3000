// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QWidget>
#include <QSettings>

class MdiChild : public QWidget{
    Q_OBJECT

public:
    MdiChild(QWidget *parent);

    QSettings& settings();
    
protected:
    void closeEvent(QCloseEvent *event) override;

};

#endif
