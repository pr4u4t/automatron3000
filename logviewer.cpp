#include "logviewer.h"
#include "api/api.h"
#include "mainwindow.h"

#include <QVBoxLayout>

LogViewer::LogViewer(QWidget* mwin)
    : MdiChild(mwin), m_text(new QPlainTextEdit()){
    m_text->document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    m_text->setPalette(p);
    m_text->setReadOnly(false);
    connect(qobject_cast<MainWindow*>(mwin)->logger(),&Logger::echo,this,&LogViewer::message);
    
    QBoxLayout *l = new QVBoxLayout();
    l->addWidget(m_text);
    setLayout(l);
}

void LogViewer::settingsChanged(){}
    
void LogViewer::message(const QString& msg){
    m_text->appendPlainText(msg);
}
