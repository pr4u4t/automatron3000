#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProxyStyle>
#include <QTranslator>

class ProxyStyle : public QProxyStyle{
public:
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override {
        if (hint == QStyle::SH_UnderlineShortcut) {
            return 1;
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

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
