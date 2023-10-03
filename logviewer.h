#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QPlainTextEdit>

#include "mdichild.h"

class LogViewer : public MdiChild{

    Q_OBJECT
    
public:
    
    Q_INVOKABLE LogViewer(QWidget* parent = nullptr, QWidget* mwin = nullptr);
    
public slots:
    void settingsChanged();
    
    void message(const QString& msg);
    
private:
    QPlainTextEdit* m_text;
};

#endif
