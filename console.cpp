// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "console.h"

#include <QScrollBar>
#include <QHBoxLayout>

Terminal::Terminal(QWidget* parent)
    : QPlainTextEdit(parent){
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
}

void Terminal::setLocalEchoEnabled(bool set){
    m_localEchoEnabled = set;
}

void Terminal::keyPressEvent(QKeyEvent *e){
    switch (e->key()) {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;
        default:
            if (m_localEchoEnabled){
                keyPressEvent(e);
            }
            emit getData(e->text().toLocal8Bit());
    }
}
    
void Terminal::mousePressEvent(QMouseEvent *e){
    Q_UNUSED(e);
    setFocus();
}
    
void Terminal::mouseDoubleClickEvent(QMouseEvent *e){
    Q_UNUSED(e);
}
    
void Terminal::contextMenuEvent(QContextMenuEvent *e){
    Q_UNUSED(e);
}
    
Console::Console(QWidget *parent, QWidget* wmin) 
    : MdiChild(parent),m_terminal(new Terminal(this)){
    QBoxLayout *l = new QVBoxLayout();
    l->addWidget(m_terminal);
    setLayout(l);
}

void Console::putData(const QByteArray &data){
    m_terminal->insertPlainText(data);
    QScrollBar *bar = m_terminal->verticalScrollBar();
    bar->setValue(bar->maximum());
}
