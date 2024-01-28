// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

#include "../api/api.h"

class Terminal : public QPlainTextEdit{
    Q_OBJECT
    
signals:
    void getData(const QByteArray &data);
    
    void logMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::NOTICE);
    
public:
    Terminal(QWidget* parent = nullptr, QWidget* mwin = nullptr);
    
    void setLocalEchoEnabled(bool set);
        
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    
private:
    bool m_localEchoEnabled = false;  
};

class Console : public MdiChild{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);

public:
    Console(Plugin *parent = nullptr, QWidget* mwin = nullptr);

    void putData(const QByteArray &data);

    ~Console() {
    
    }

    bool saveSettings() {
        return true;
    }

public slots:
    void settingsChanged();
    
private:
    Terminal* m_terminal = nullptr;
};

#endif // CONSOLE_H
