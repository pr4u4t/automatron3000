#include "logger.h"
#include <QDateTime>
#include <QDir>

Logger::Logger(const QString& path, QObject* parent, qint32 severity)
    : m_log(path,parent)
    , m_buffer(BufferSize)
    , m_severity(severity){
    
    QDir().mkpath(QFileInfo(path).absolutePath());

    if (m_log.open(QIODeviceBase::WriteOnly | QIODeviceBase::Append | QIODeviceBase::Unbuffered) == false) {
        qDebug().noquote() << tr("Failed to open log file: %1").arg(path);
        return;
    }

    message("Started!");
    message(QString("Logger::Logger: using log file: %1, severity: %2 %3").arg(path).arg(severity).arg(severityString()));
}

Logger::~Logger(){
    if (m_log.isOpen()) {
        m_log.flush();
        m_log.close();
    }
}

QString Logger::severityName(LoggerSeverity severity){
    switch(severity){
        case LoggerSeverity::LOG_INFO:
            return "INFO";

        case LoggerSeverity::LOG_NOTICE:
            return "NOTICE";
        
        case LoggerSeverity::LOG_WARNING:
            return "WARNING";
            
        case LoggerSeverity::LOG_ERROR:
            return "ERROR";
            
        case LoggerSeverity::LOG_DEBUG:
            return "DEBUG";
    }
    
    return "UNKNOWN";
}

void Logger::message(const QString& msg, LoggerSeverity severity){
    if (m_severity & static_cast<qint32>(severity)) {
        QString line = "["+QDateTime::currentDateTime().toString() + " " + severityName(severity) + "] " + msg;
        m_buffer.append(line);
        emit echo(line, severity);
        line += LE;
        m_log.write(line.toLocal8Bit());
    }
}
