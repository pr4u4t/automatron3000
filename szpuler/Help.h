#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QHelpEngineCore>

class HelpBrowser : public QTextBrowser {
    
    Q_OBJECT

public:
    
    HelpBrowser(QWidget* parent = nullptr);
    
    void showHelpForKeyword(const QString& id);

private:
    QVariant loadResource(int type, const QUrl& name) override;

    QHelpEngineCore* m_helpEngine;
};

#endif
