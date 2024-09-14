#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QTextEdit>

#include "../api/api.h"
#include "../core/core.h"

class LogViewer : public Widget{

    Q_OBJECT
    
public:
    
    LogViewer(Loader* ld, PluginsLoader* plugins, QWidget* parent = nullptr, const QString& settingsPath = QString());

    SettingsMdi* settingsWindow() const override;

public slots:
    void settingsChanged();
    
    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE);
    
private:
    QTextEdit* m_text;
};

#endif
