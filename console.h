// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

#include "mdichild.h"

class Console : public MdiChild{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);

public:
    Q_INVOKABLE Console(QWidget *parent = nullptr, QWidget* mwin = nullptr);

    void putData(const QByteArray &data);
    void setLocalEchoEnabled(bool set);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    bool m_localEchoEnabled = false;
    QPlainTextEdit* m_console;
};

#endif // CONSOLE_H
