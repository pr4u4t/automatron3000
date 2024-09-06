#ifndef QLINREADBYID_H
#define QLINREADBYID_H

#include "qlinreadbyid_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_linreadbyid.h"
#include <QTimer>

QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinReadByIDUI;
}

//class QIntValidator;
QT_END_NAMESPACE

enum class QLinReadByIDState {
    INITIAL,
    READ,
    PAUSE,
    STOP,
    ERROR
};

class QLINREADBYID_EXPORT QLinReadByID : public Widget
{
    Q_OBJECT

public:
    QLinReadByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    ~QLinReadByID();

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startRead();

    void init();

private:
    SettingsDialog::LinReadByIDSettings m_settings;
    Ui::QLinReadByIDUI* m_ui = nullptr;
    uint16_t m_test = 0;
    QSharedPointer<IODevice> m_lin;
    QLinReadByIDState m_state;
    int m_responses = 0;
    QTimer m_timer;
    int m_try = 0;
};

#endif