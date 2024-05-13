#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QContiguousCache>

#include "api_global.h"

enum class API_EXPORT LoggerSeverity : qint32 {
    LOG_NONE    = 0,
    LOG_INFO    = 1 << 0,
    LOG_NOTICE  = 1 << 1,
    LOG_WARNING = 1 << 2,
    LOG_ERROR   = 1 << 3,
    LOG_DEBUG   = 1 << 4,


    LOG_ALL     = 0x1F
};



class API_EXPORT Logger : public QObject{
    Q_OBJECT
    
signals:
    void echo(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE);
    
public:
    static constexpr const char* LE = "\r\n";
    static constexpr int BufferSize = 100;

    Logger(const QString& path, QObject* parent = nullptr, qint32 severity = static_cast<qint32>(LoggerSeverity::LOG_ALL));
    
    ~Logger();
    
    QContiguousCache<QString> cache() const {
        return m_buffer;
    }

    QString logPath() const {
        return m_log.fileName();
    }

    qint32 severity() const {
        return m_severity;
    }

public slots:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE);

protected:
    QString severityName(LoggerSeverity severity);
    
    QString severityString() const {
        QString ret;

        severityMerge(ret, LoggerSeverity::LOG_DEBUG, "DEBUG");
        severityMerge(ret, LoggerSeverity::LOG_ERROR, "ERROR");
        severityMerge(ret, LoggerSeverity::LOG_WARNING, "WARNING");
        severityMerge(ret, LoggerSeverity::LOG_NOTICE, "NOTICE");
        severityMerge(ret, LoggerSeverity::LOG_INFO, "INFO");

        return ret;
    }

    inline void severityMerge(QString& str, const LoggerSeverity& severity, const QString& name) const {
        if (m_severity & static_cast<qint32>(severity)) {
            if (str.size() > 0) {
                str += "|";
            }
            str += name;
        }
    }

private:

    QFile m_log;
    QContiguousCache<QString> m_buffer;
    qint32 m_severity = 0;
};

#endif
