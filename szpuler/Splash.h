#ifndef SPLASH_H
#define SPLASH_H

#include <QScreen>
#include <QPixmap>
#include <QSplashScreen>
#include <QGuiApplication>



class Splash : public QSplashScreen {

    Q_OBJECT

public:

    Splash(const QString& path);

    Splash& operator<<(const QString& msg) {
        showMessage(msg, Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        return *this;
    }

public slots:

    void setProgress(int value);

protected:

    void drawContents(QPainter* painter);

private:
    int m_progress = 0;
};

#endif
