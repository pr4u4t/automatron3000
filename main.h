#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

class Main : public QApplication{
    
    Q_OBJECT

public:  
    Main(int& argc, char* argv[]);
    
    static constexpr const char* appName = "Szpuler";
    static constexpr const char* org = "Pawel Ciejka";
    static constexpr const char* description = "Szpuler";
    
protected:
    
    void parseArgumnets();
};

#endif
