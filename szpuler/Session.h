#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include "../api/api.h"
#include "../core/core.h"

class MainWindow;

class Session : public QObject {
    Q_OBJECT

public:

    Session(MLoader* plugins, Logger* log, const QString& path, MainWindow* win);

    QSettings& settings();

    MLoader* plugins() const;

    Logger* logger() const;

    MainWindow* window() const;

public slots:

    qint64 store();

    qint64 restore();

protected:



signals:
    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

private:

    Logger* m_logger = nullptr;
    MLoader* m_plugins = nullptr;
    MainWindow* m_win = nullptr;
    QSettings m_settings;
};

#endif