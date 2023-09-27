// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "console.h"

#include <QScrollBar>

Console::Console(QWidget *parent, QWidget* wmin) 
    : MdiChild(parent){
    m_console = new QPlainTextEdit();
    m_console.document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    m_console.setPalette(p);
}

void Console::putData(const QByteArray &data){
    m_console.insertPlainText(data);

    QScrollBar *bar = m_console.verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set){
    m_localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (m_localEchoEnabled)
            m_console.keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e){
    Q_UNUSED(e);
    m_console.setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e){
    Q_UNUSED(e);
}

void Console::contextMenuEvent(QContextMenuEvent *e){
    Q_UNUSED(e);
}
