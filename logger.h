#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>

enum LoggerSeverity{
    NOTICE = 0,
    WARNING,
    ERROR,
    DEBUG
};

class Logger : public QObject{
    Q_OBJECT
    
public:
    static constexpr const char* LE = "\r\n";
    
    Logger(const QString& path,QObject* parent = nullptr);
    
    ~Logger();
    
public slots:

    void message(const QString& msg,LoggerSeverity severity = LoggerSeverity::NOTICE);

protected:
    QString severityName(LoggerSeverity severity);
    
private:

    QFile m_log;
};

#endif
