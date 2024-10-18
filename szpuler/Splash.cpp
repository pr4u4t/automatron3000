#include "Splash.h"
#include <QStyleOptionProgressBar>
#include <QStyle>

Splash::Splash(const QString& path) {
    QPixmap pixmap(path);
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int screenWidth = screenGeometry.width() / 2;
    int screenHeight = screenGeometry.height() / 2;
    QPixmap splashPix = pixmap.scaled(screenWidth, screenHeight, Qt::KeepAspectRatio);
    setPixmap(splashPix);
}


void Splash::setProgress(int value) {
    m_progress = value;

    if (m_progress > 100) {
        m_progress = 100;
    }

    if (m_progress < 0) {
        m_progress = 0;
    }

    //update();
}

void Splash::drawContents(QPainter* painter) {
    // Set style for progressbar...
    QStyleOptionProgressBar pbstyle;
    //pbstyle.initFrom(this);
    pbstyle.state = QStyle::State_Enabled | QStyle::State_Horizontal;
    pbstyle.textVisible = true;
    pbstyle.minimum = 0;
    pbstyle.maximum = 100;
    pbstyle.progress = m_progress;
    pbstyle.invertedAppearance = false;
    pbstyle.rect = QRect(0, height()-20, width(), 20); // Where is it

    // Draw it...
    style()->drawControl(QStyle::CE_ProgressBar, &pbstyle, painter, this);

    QSplashScreen::drawContents(painter);
}
