// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

#include "mdichild.h"
#include "mainwindow.h"

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
    MainWindow* m_mwin = nullptr;
    
};

class Console : public MdiChild{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);

public:
    Q_INVOKABLE Console(QWidget *parent = nullptr, QWidget* mwin = nullptr);

    void putData(const QByteArray &data);

public slots:
    void settingsChanged();
    
private:
    Terminal* m_terminal = nullptr;
};

#endif // CONSOLE_H
