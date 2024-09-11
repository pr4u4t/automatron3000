// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QWidget>
#include <QSettings>

#include "logger.h"
#include "api_global.h"

class Plugin;

class API_EXPORT MdiChild : public QWidget{
    
    Q_OBJECT

signals:
    
    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

public:
    
    MdiChild(QWidget* parent = nullptr);
    
    virtual ~MdiChild() = default;

protected:

    void closeEvent(QCloseEvent *event) override;
};

#endif // MDICHILD_H
