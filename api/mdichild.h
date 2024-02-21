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
    
    void logMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;
    
    void serialMessage(const QString& msg);

public:
    MdiChild(/*Plugin* parent,*/ QWidget* mwin);

    QSettings& settings();

    virtual bool saveSettings() = 0;

public slots:
    virtual void settingsChanged() = 0;
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
    QWidget *mainWindow();
    
private:
    QWidget* m_mainWindow = nullptr;
    Plugin* m_plugin = nullptr;
};

#endif
