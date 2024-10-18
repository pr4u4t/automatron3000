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

    bool reset(Reset type = Reset::SOFT) {
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:
    void settingsChanged();
    
    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE);

    QVariant exec() {
        return QVariant();
    }
    
private:
    QTextEdit* m_text;
};

#endif
