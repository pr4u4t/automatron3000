#include "logger.h"
#include <QDateTime>

Logger::Logger(const QString& path,QObject* parent)
    : m_log(path,parent){
    m_log.open(QIODeviceBase::WriteOnly | QIODeviceBase::Append | QIODeviceBase::Unbuffered);
}

Logger::~Logger(){
    m_log.flush();
    m_log.close();
}

QString Logger::severityName(LoggerSeverity severity){
    switch(severity){
        case LoggerSeverity::NOTICE:
            return "NOTICE";
        
        case LoggerSeverity::WARNING:
            return "WARNING";
            
        case LoggerSeverity::ERROR:
            return "ERROR";
            
        case LoggerSeverity::DEBUG:
            return "DEBUG";
    }
    
    return "UNKNOWN";
}

void Logger::message(const QString& msg, LoggerSeverity severity){
    QString line = QDateTime::currentDateTime().toString()+" "+severityName(severity)+" "+msg;
    emit echo(line); 
    line += LE;
    m_log.write(line.toLocal8Bit());
    
}
