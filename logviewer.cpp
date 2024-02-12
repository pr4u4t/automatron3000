#include "logviewer.h"
#include "api/api.h"
#include "mainwindow.h"

#include <QVBoxLayout>

LogViewer::LogViewer(QWidget* parent, QWidget* mwin)
    : MdiChild(mwin), m_text(new QPlainTextEdit()){
    m_text->document()->setMaximumBlockCount(100);
    m_text->setReadOnly(true);
    connect(qobject_cast<MainWindow*>(mwin)->logger(),&Logger::echo,this,&LogViewer::message);
    
    QBoxLayout *l = new QVBoxLayout();
    l->addWidget(m_text);
    setLayout(l);
}

void LogViewer::settingsChanged(){}
    
void LogViewer::message(const QString& msg){
    m_text->appendPlainText(msg);
}
