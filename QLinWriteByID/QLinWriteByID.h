#ifndef QLINWRITEBYID_H
#define QLINWRITEBYID_H

#include "qlinwritebyid_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_linwritebyid.h"
#include <QTimer>


QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinWriteByIDUI;
}

//class QIntValidator;
QT_END_NAMESPACE

enum class QLinWriteByIDState {
    INITIAL,
    WRITE,
    WAIT,
    PAUSE,
    STOP,
    ERROR
};

class QLINWRITEBYID_EXPORT QLinWriteByID : public Widget
{
    Q_OBJECT

public:
    QLinWriteByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    ~QLinWriteByID();

    SettingsMdi* settingsWindow() const override;

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startRead();

    void init();

private:

    Ui::QLinWriteByIDUI* m_ui = nullptr;
    uint16_t m_test = 0;
    QSharedPointer<IODevice> m_lin;
    QLinWriteByIDState m_state;
    int m_responses = 0;
    QTimer m_timer;
    int m_try = 0;
};

#endif