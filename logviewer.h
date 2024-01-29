#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QPlainTextEdit>

#include "api/api.h"

class LogViewer : public MdiChild{

    Q_OBJECT
    
public:
    
    LogViewer(QWidget* mwin = nullptr);
    
public slots:
    void settingsChanged();
    
    void message(const QString& msg);
    
private:
    QPlainTextEdit* m_text;
};

#endif
