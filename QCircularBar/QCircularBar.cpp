#include "QCircularBar.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>
#include <QLCDNumber>

#include "../core/core.h"

struct QCircularBarMenu {
    QCircularBarMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QCircularBar", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_QCircularBarMenu = new QMenu(m_app->translate("MainWindow", "Visualization"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_QCircularBarMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QCircularBarMenu);
        m_newInstance->setData(QVariant("QCircularBar"));
        m_QCircularBarMenu->addAction(m_newInstance);
    }

    QMenu* m_QCircularBarMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QCircularBar_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCircularBarMenu* ctx, Logger* log) {
    log->message("QCircularBar_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QCircularBar_register(): application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QCircularBarMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QCircularBar") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QCircularBar*>(plugin)->objectName(), ctx->m_QCircularBarMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QCircularBar*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QCircularBar*>(plugin), &QCircularBar::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QCircularBar*>(plugin)->objectName() + "/Settings"); //ctx->m_app->translate("MainWindow", "QJTAG-Settings"));
            });
        });

    return true;
}

static bool QCircularBar_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCircularBarMenu* ctx, Logger* log) {
    log->message("QJTAG_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QCircularBar,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QCircularBar_register,
    QCircularBar_unregister,
    QCircularBarMenu,
    {},
    true,
    200
)

QCircularBar::QCircularBar(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
    : Widget(ld, plugins, parent, path, new SettingsDialog::CircularBarSettings(Settings::get(), path)) {
    m_lcd = new QLCDNumber(this);
    setPrecision(0);
    setSteps(20);
    setBarSize(5);
    setMinimumSize(QSize(80, 80));
    setMaximumSize(QSize(1024, 1024));
    setStartAngle(225);
    setEndAngle(-45);
    setForeground(QColor(0, 166, 8));
    setBackground(Qt::white);
    setThresholdEnabled(false);
    setNumericIndicatorEnabled(true);
    int r, g, b;
    r = foreground().red();
    g = foreground().green();
    b = foreground().blue();
    QString style = QString("background-color: transparent; color: rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
    m_lcd->setStyleSheet(style);
    m_lcd->setSegmentStyle(QLCDNumber::Flat);
    m_autodigits = false;
    setMinValue(0);
    setMaxValue(100);
    setDigitCount(5);
    setValue(0);
    setLabel("Label");
    //setUnits("");
    setThreshold(80);
    setCircularBarEnabled(true);
    //setCoverGlassEnabled(true);
    setEnabled(true);
}


QCircularBar::~QCircularBar() {
    delete m_lcd;
}

SettingsMdi* QCircularBar::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QCircularBar::settingsChanged);
    return ret;
}

int QCircularBar::digits(int val) {
    int digits = 0;
    if (val <= 0) { // remove this line if '-' counts as a digit
        digits = 1;
    }
    
    while (val) {
        val /= 10;
        digits++;
    }
    return digits;
}

void QCircularBar::settingsChanged() {
    emit message("QCircularBar::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<SettingsDialog::CircularBarSettings>();
    *set = SettingsDialog::CircularBarSettings(Settings::get(), settingsPath());

    setMinValue(set->minValue);
    setMaxValue(set->maxValue);
    setThreshold(set->threshold);
    setPrecision(set->precision);
    setLabel(set->label);
    setUnits(set->units);
    setSteps(set->steps);

    update();
}

void QCircularBar::setBarSize(int barSize) {
    m_barSize = barSize;
    update();
}


void QCircularBar::setValue(double value) {
    if (!isEnabled()) {
        return;
    }

    if (value > m_maxValue){
        m_value = m_maxValue;
        emit errorSignal(OutOfRange);
    } else {
        if (value < m_minValue) {
            m_value = m_minValue;
            emit errorSignal(OutOfRange);
        }
        else {
            m_value = value;
            if (m_autodigits) {
                m_lcd->setDigitCount(digits(value));
            }

            if (m_lcd) {
                //QString val = QString( "%1" ).arg(m_value,0,'f',m_precision);
                m_lcd->display(m_value);
            }
        }
    }

    if (thresholdEnabled()) {
        thresholdManager();

        if (value >= threshold()) {
            QString style = QString("background-color: transparent; color: rgb(200,0,0);");
            m_lcd->setStyleSheet(style);
        } else {
            int r, g, b;
            r = foreground().red();
            g = foreground().green();
            b = foreground().blue();
            QString style = QString("background-color: transparent; color: rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
            m_lcd->setStyleSheet(style);
        }
    } else {
        int r, g, b;
        r = foreground().red();
        g = foreground().green();
        b = foreground().blue();
        QString style = QString("background-color: transparent; color: rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
        m_lcd->setStyleSheet(style);
    }

    update();
}


int QCircularBar::digitCount() const
{
    if (m_autodigits)
        return -1;

    if (m_lcd)
        return m_lcd->digitCount();
    else
        return 0;
}


void QCircularBar::setValue(int value)
{
    setValue((double)value);
    update();
}

void QCircularBar::setMinValue(double value)
{
    m_minValue = value;
    update();
}


void QCircularBar::setMinValue(int value)
{
    setMinValue((double)value);
}


void QCircularBar::setMaxValue(double value)
{
    if (value > m_minValue)
    {
        m_maxValue = value;
        update();
    }
    else
        emit errorSignal(MaxValueError);
}


void QCircularBar::setMaxValue(int value)
{
    setMaxValue((double)value);
}


void QCircularBar::setThreshold(double value)
{
    if (value > m_minValue && value < m_maxValue)
    {
        m_threshold = value;
        m_thresholdEnabled = true;
        update();
    }
    else
        emit errorSignal(ThresholdError);
}


void QCircularBar::setThreshold(int value)
{
    setThreshold((double)value);
}



void QCircularBar::setPrecision(int precision)
{
    m_precision = precision;
    update();
}


void QCircularBar::setLabel(QString label)
{
    m_label = label;
    update();
}

void QCircularBar::setUnits(QString units)
{
    m_units = units;
    update();
}

void QCircularBar::setDigitCount(int n_digits)
{
    if (n_digits > 0)
    {
        if (m_lcd)
            m_lcd->setDigitCount(n_digits);
    }
    else
        m_autodigits = true;
}


void QCircularBar::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    int side = qMin(width(), height());
    m_lcd->setGeometry(width() / 2 - side / 4, height() / 2 - side / 6, side / 2, side / 3);
    update();
}


void QCircularBar::changeEvent(QEvent* event)
{
    if (!isEnabled())
    {
        QString style = QString("background-color: transparent; color: rgb(200,200,200);");
        m_lcd->setStyleSheet(style);
        update();
    }
    else
        setValue(value());
}


void QCircularBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2, side, side);
    painter.setWindow(-50, -50, 100, 100);

    drawBackground(&painter);
    if (circularBarEnabled())
        drawCircularBar(&painter);

    drawTicks(&painter);
    if (coverGlassEnabled())
        drawCoverGlass(&painter);
    drawLabel(&painter);
    drawUnits(&painter);
    drawCrown(&painter);
    if (thresholdEnabled())
        drawThresholdLine(&painter);

}

void QCircularBar::setCoverGlassEnabled(bool enable)
{
    m_coverGlassEnabled = enable;
    update();
}

void QCircularBar::setSteps(int nSteps)
{
    if (nSteps > 1)
    {
        m_steps = nSteps;
        update();
    }
    else
        nSteps = 1;
}


void QCircularBar::setStartAngle(int value)
{
    m_startAngle = value;
    update();
}


void QCircularBar::setEndAngle(int value)
{
    m_endAngle = value;
    update();
}


void QCircularBar::setForeground(QColor newForeColor)
{
    m_foreground = newForeColor;


    int r, g, b;
    r = foreground().red();
    g = foreground().green();
    b = foreground().blue();
    QString style = QString("background-color: transparent; color: rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
    m_lcd->setStyleSheet(style);


    update();
}


void QCircularBar::setBackground(QColor newBackColor)
{
    m_background = newBackColor;


    int r, g, b;
    r = foreground().red();
    g = foreground().green();
    b = foreground().blue();
    QString style = QString("background-color: transparent; color: rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
    m_lcd->setStyleSheet(style);

    update();
}


void QCircularBar::thresholdManager()
{
    // m_thresholdFlag is used to avoid signals at each setValue
    if (m_value > m_threshold /*&& !m_thresholdFlag*/)
    {
        //     m_thresholdFlag=true;
        emit thresholdAlarm(true);
    }
    else
        if (m_value < m_threshold /*&& m_thresholdFlag*/)
        {
            //      m_thresholdFlag=false;
            emit thresholdAlarm(false);
        }
}


void QCircularBar::setThresholdEnabled(bool enable)
{
    m_thresholdEnabled = enable;
    update();
}


void QCircularBar::setNumericIndicatorEnabled(bool enable)
{
    m_numericIndicatorEnabled = enable;
    m_lcd->setVisible(enable);
    update();
}


void QCircularBar::setCircularBarEnabled(bool enable)
{
    m_circularBarEnabled = enable;
    update();
}


void QCircularBar::drawCrown(QPainter* painter)
{
    painter->save();
    QRectF rectangle(-47, -47, 94, 94);
    int startAngle = 30 * 16;
    int spanAngle = 390 * 16;
    QLinearGradient linearGrad(QPointF(-47, -47), QPointF(94, 94));
    linearGrad.setColorAt(0, Qt::white);
    linearGrad.setColorAt(0.3, QColor(60, 60, 60, 250));
    QPen pen;
    QBrush brush = QBrush(linearGrad);
    pen.setBrush(brush);
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawArc(rectangle, startAngle, spanAngle);
    painter->restore();

}


void QCircularBar::drawCoverGlass(QPainter* painter)
{
    painter->save();
    QLinearGradient linearGrad(QPointF(-15, -45), QPointF(90, 75));
    linearGrad.setColorAt(0, QColor(255, 255, 255, 30));
    linearGrad.setColorAt(0.3, QColor(120, 120, 120, 20));
    QBrush brush = QBrush(linearGrad);
    QPen pen;
    pen = QColor(0, 0, 0, 0);
    pen.setBrush(brush);
    pen.setWidth(0);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(-45, -45, 90, 90);
    painter->restore();
}


void QCircularBar::drawBackground(QPainter* painter)
{
    painter->save();
    painter->setBrush(m_background);
    painter->drawEllipse(-45, -45, 90, 90);
    painter->restore();
}


void QCircularBar::drawTicks(QPainter* painter)
{
    painter->save();
    QPen pen = QPen(background());
    pen.setWidth(1);
    painter->setPen(pen);
    int nSteps = m_steps;
    painter->rotate(-m_startAngle);
    double angleStep = (-m_endAngle + m_startAngle) / m_steps;
    for (int i = 0; i <= nSteps; i++)
    {
        painter->drawLine(30, 0, 40, 0);
        painter->rotate(angleStep);
    }
    painter->restore();

    painter->save();
    pen.setWidth(1);
    painter->setPen(pen);
    double spanAngle = m_endAngle - m_startAngle;
    painter->drawArc(-28, -28, 56, 56, m_startAngle * 16, spanAngle * 16);
    painter->restore();

}

void QCircularBar::drawThresholdLine(QPainter* painter)
{
    QPen pen;

    double thresholdAngle = (m_startAngle + (m_endAngle - m_startAngle) / (m_maxValue - m_minValue) * (m_threshold - m_minValue));
    pen.setWidth(2);

    //    pen.setColor(Qt::green);
    //    painter->setPen(pen);
    //    painter->drawArc(-40,-40,80,80,(int)m_startAngle*16,(int)(thresholdAngle-m_startAngle)*16);

    if (isEnabled())
        pen.setColor(Qt::red);
    else
        pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->drawArc(-40, -40, 80, 80, (int)thresholdAngle * 16, (int)(-thresholdAngle + m_endAngle) * 16);

}

void QCircularBar::drawCircularBar(QPainter* painter)
{
    painter->save();
    double valueAngle = (m_startAngle + ((m_endAngle - m_startAngle) / (m_maxValue - m_minValue)) * (m_value - m_minValue));
    QRadialGradient haloGradientw(0, 0, 49, 0, 0);
    QColor color;

    if (isEnabled())
        color = foreground();
    else
        color = Qt::gray;

    haloGradientw.setColorAt(0.7, color);
    haloGradientw.setColorAt(0.9, Qt::black);
    QBrush brush = QBrush(haloGradientw);
    QPen pen = QPen(brush, barSize());
    painter->setPen(pen);
    painter->drawArc(-35, -35, 70, 70, (int)(m_startAngle * 16), (int)((valueAngle - m_startAngle) * 16));

    painter->restore();

    if (thresholdEnabled())
    {
        //        if(value()>threshold())
        //        {
        //            painter->save();
        //            double thresholdAngle = ( m_startAngle+(m_endAngle-m_startAngle)/(m_maxValue-m_minValue)*(m_threshold-m_minValue) );
        //            QRadialGradient haloGradientT(0, 0, 49, 0, 0);
        //            haloGradientT.setColorAt(0.7, Qt::red);
        //            haloGradientT.setColorAt(0.9, Qt::black);
        //            brush=QBrush(haloGradientT);
        //            pen = QPen(brush,4);
        //            painter->setPen(pen);
        //            painter->drawArc(-35,-35,70,70,(int)(thresholdAngle*16),(int)((valueAngle-thresholdAngle)*16));
        //            //painter->drawArc(-35,-35,70,70,(int)(m_startAngle*16),(int)((valueAngle-m_startAngle)*16));
        //            painter->restore();
        //        }
    }




}


void QCircularBar::drawLabel(QPainter* painter)
{
    painter->save();

    QRectF labelRect = QRectF(-15, 20, 30, 20);

    if (isEnabled())
    {
        painter->setBrush(m_foreground);
        painter->setPen(m_foreground);
    }
    else
    {
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::gray);
    }

    painter->drawText(labelRect, Qt::AlignCenter, m_label);
    painter->restore();
}


void QCircularBar::drawUnits(QPainter* painter)
{
    painter->save();

    QRectF unitsRect = QRectF(-20, -30, 40, 20);

    if (isEnabled())
    {
        painter->setBrush(m_foreground);
        painter->setPen(m_foreground);
    }
    else
    {
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::gray);
    }

    painter->drawText(unitsRect, Qt::AlignCenter, m_units);
    painter->restore();
}
