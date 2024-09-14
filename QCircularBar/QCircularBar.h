#ifndef QCIRCULAR_BAR_H
#define QCIRCULAR_BAR_H

#include "qcircularbar_global.h"

#include <Qt>
#include <QWidget>
#include <QLCDNumber>
#include <QPropertyAnimation>
#include <QColor>

#include "../api/api.h"

class QCIRCULARBAR_EXPORT QCircularBar : public Widget
{
    Q_OBJECT
    Q_ENUMS(errorCode);
    Q_PROPERTY(double value READ value WRITE setValue);
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue);
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue);
    Q_PROPERTY(double threshold READ threshold WRITE setThreshold);
    Q_PROPERTY(int precision READ precision WRITE setPrecision);
    Q_PROPERTY(QString label READ label WRITE setLabel);
    Q_PROPERTY(QString units READ units WRITE setUnits);
    Q_PROPERTY(int steps READ steps WRITE setSteps);

    Q_PROPERTY(int startAngle READ startAngle WRITE setStartAngle);
    Q_PROPERTY(int endAngle READ endAngle WRITE setEndAngle);

    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(bool enableThreshold READ thresholdEnabled WRITE setThresholdEnabled);
    Q_PROPERTY(bool enableNumericIndicator READ enableNumericIndicator WRITE setNumericIndicatorEnabled);
    Q_PROPERTY(bool circularBarEnabled READ circularBarEnabled WRITE setCircularBarEnabled);
    Q_PROPERTY(bool coverGlassEnabled READ coverGlassEnabled WRITE setCoverGlassEnabled);
    Q_PROPERTY(int digitCount READ digitCount WRITE setDigitCount)
    Q_PROPERTY(int barSize READ barSize WRITE setBarSize)


public:
    /*!
       \brief ErrorCode : This enum represents the code returned by the error signal
    */
    enum ErrorCode { MaxValueError = 1, MinValueError, ThresholdError, TargetError, PrecisionError, ColorError, UnitsEmpty, OutOfRange };

    /*!
      \brief Constructor
      \param parent Parent Widget
    */

    QCircularBar(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    bool saveSettings() {
        return true;
    }

    SettingsMdi* settingsWindow() const override;

    /*!
      \brief Destructor
      \param none
    */
    ~QCircularBar();

    /*!
  \return The current displayed value
*/
    double value() const { return m_value; }

    /*!
      \return The minimum widget scale value
    */
    double minValue() const { return m_minValue; }

    /*!
      \return The maximum widget scale value
    */
    double maxValue() const { return m_maxValue; }

    /*!
      \return The threshold
    */
    double threshold() const { return m_threshold; }

    /*!
      \return The number of decimals displayed in the widget scale
    */
    int precision() const { return m_precision; }

    /*!
      \return The Label
    */
    QString label()const { return m_label; }

    /*!
      \return The Label
    */
    QString units()const { return m_units; }



    /*!
      \return The number of steps in the widget scale
    */
    int steps() const { return m_steps; }

    /*!
      \return The starting angle associated to the minimum value for the widget scale
    */

    int startAngle() { return m_startAngle; }

    /*!
      \return The end angle associated to the minimum value for the widget scale
    */
    int endAngle() { return m_endAngle; }

    /*!
      \return The foreground color used for the widget
    */
    QColor foreground() const { return m_foreground; }

    /*!
      \return The background color used for the widget
    */
    QColor background() const { return m_background; }

    /*!
      \return The enableThreshold flag
    */
    bool thresholdEnabled() const { return m_thresholdEnabled; }

    /*!
      \return The enableNumericIndicator flag
    */
    bool enableNumericIndicator() const { return m_numericIndicatorEnabled; }


    /*!
      \return The m_circularBarEnabled flag
    */
    bool circularBarEnabled() const { return m_circularBarEnabled; };


    /*!
      \return The m_circularBarEnabled flag
    */
    bool coverGlassEnabled() const { return m_coverGlassEnabled; };


    /*!
      \return nnumber of digit for LCD indicator
    */

    int digitCount() const;

    int barSize() const { return m_barSize; };

signals:

    /*!
      \signals This signal is emitted to report an error condition.
    */
    void errorSignal(int);
    /*!
      \signals This signal reports a value true when the input value exceeds for the
               first time the threshould limit and reports a value false when the input
               value decreases under the threshold limit. That means errorSignal is emitted
               only one time (with true argument) when the input passes from a value < threshold to a value > threshold
               and only one time (with false argument) when the input decreases under the
               threshold limit.
    */
    void thresholdAlarm(bool);


public slots:
    void settingsChanged();

    /*!
      \slots This slot is used to set QMeter value
    */
    void setValue(double);
    /*!
      \slots This is an overloaded member function, provided for convenience.
    */
    void setValue(int);
    /*!
      \slots This slot is used to set the lower limit of the QMeter scale.
    */
    void setMinValue(double);
    /*!
      \slots This is an overloaded member function, provided for convenience.
    */
    void setMinValue(int);
    /*!
      \slots This slot is used to set the upper limit of the QMeter scale.
    */
    void setMaxValue(double);
    /*!
      \slots This is an overloaded member function, provided for convenience.
    */
    void setMaxValue(int);
    /*!
      \slots This slot is used to set the threshold limit. See
      \ref thresholdAlarm for more info.
    */
    void setThreshold(double);
    /*!
      \slots This is an overloaded member function, provided for convenience.
    */
    void setThreshold(int);
    /*!
      \slots This slot is used to set the number of decimal digits for the QMeter scale
      and QMeter numerical value.
    */
    void setPrecision(int);

    /*!
     \slots This slot is used to set the label in the QMeter widget.
    */
    void setLabel(QString);

    /*!
      \slots This slot is used to set the units in the QMeter widget.
    */
    void setUnits(QString);

    /*!
      \slots This slot is used to set the number of spets in the QMeter scale.
    */
    void setSteps(int);
    /*!
      \slots This represents the starting angle correspondign to the lower limit for the QMeter scale.
      Positive and negative angles can be used.
    */
    void setStartAngle(int);
    /*!
      \slots This represents the end angle corresponding to the upper limit for the QMeter scale.
      Positive and negative angles can be used.
    */
    void setEndAngle(int);
    /*!
      \slots This slot is used to set the widget foreground color.
    */
    void setForeground(QColor);
    /*!
      \slots This slot is used to set the widget background color.
    */
    void setBackground(QColor);
    /*!
      \slots This slot allows to enable the threshold management. See
      \ref thresholdAlarm for more info.
    */
    void setThresholdEnabled(bool);

    /*!
      \slots This slot allows to enable the numerical value display
    */
    void setNumericIndicatorEnabled(bool);

    /*!
          \slots This slot allows to enable the circular bar indicator
    */
    void setCircularBarEnabled(bool);

    /*!
          \slots This slot allows to enable the circular bar indicator
    */
    void setCoverGlassEnabled(bool);

    /*!
          \slots This slot sets LCD number of digits
    */
    void setDigitCount(int n_digits);

    /*!
                  \slots This slot sets the bar size in pixels
    */
    void setBarSize(int barSize);

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void changeEvent(QEvent* event);

private:

    void thresholdManager();
    int digits(int val);
    void drawCrown(QPainter* painter);
    void drawBackground(QPainter* painter);
    void drawTicks(QPainter* painter);
    void drawCircularBar(QPainter* painter);
    void drawCoverGlass(QPainter* painter);
    void drawLabel(QPainter* painter);
    void drawUnits(QPainter* painter);
    void drawThresholdLine(QPainter* painter);

    double m_value;
    double m_maxValue, m_minValue;
    int m_precision;
    QString m_label;
    QString m_units;
    int m_steps;
    int m_barSize;
    int m_startAngle, m_endAngle;
    double m_threshold;
    bool m_autodigits;
    bool m_thresholdEnabled;
    bool m_numericIndicatorEnabled;
    bool m_circularBarEnabled;
    bool m_coverGlassEnabled;
    QColor m_foreground;
    QColor m_background;
    QLCDNumber* m_lcd;
};


#endif // QCIRCULAR_BAR_H