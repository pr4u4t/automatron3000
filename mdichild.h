// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QWidget>
#include <QSettings>

#include "logger.h"

class MdiChild : public QWidget{
    
    Q_OBJECT

signals:
    
    void logMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::NOTICE) const;
    
public:
    MdiChild(QWidget *parent,QWidget *mwin);

    QSettings& settings();
    
public slots:
    virtual void settingsChanged() = 0;
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
    QWidget *mainWindow();
    
private:
    QWidget* m_mainWindow = nullptr;
};

#endif
