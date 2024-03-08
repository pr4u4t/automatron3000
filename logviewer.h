#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QPlainTextEdit>

#include "api/api.h"
#include "ModuleLoader.h"

class LogViewer : public Widget{

    Q_OBJECT
    
public:
    
    LogViewer(Loader* ld, PluginsLoader* plugins, QWidget* parent = nullptr, const QString& settingsPath = QString());

public slots:
    void settingsChanged();
    
    void message(const QString& msg);
    
private:
    QPlainTextEdit* m_text;
};

#endif
