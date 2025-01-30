#ifndef QCUSTOM_ACTION_H
#define QCUSTOM_ACTION_H

#include "settingsdialog.h"
#include "../api/api.h"

#include "qcustomaction_global.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QCustomActionUI;
}

//class QIntValidator;
QT_END_NAMESPACE

enum class QCustomActionState {
    INITIAL,
    INPROGRESS,
    PAUSE,
    STOP
};

struct QCustomActionData {
    QCustomActionData(Ui::QCustomActionUI* ui, const QCustomActionState& state = QCustomActionState::INITIAL)
    : m_ui(ui)
    , m_state(state){}

    Ui::QCustomActionUI* m_ui = nullptr;
    QCustomActionState m_state = QCustomActionState::INITIAL;
};

class QCUSTOMACTION_EXPORT QCustomAction : public Widget {

    Q_OBJECT

public:
    QCustomAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, CustomActionSettings* set, const QString& uuid);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~QCustomAction();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT);

    bool initialize() override;

    bool deinitialize() override;

public slots:

    void settingsChanged();

    void jobMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE);

    void execClicked(bool checked = false);

    QVariant exec() override {
        return QVariant();
    }

private slots:

    void exportLog() const;

    void clearLog() const;

    void stopClicked() {
        m_data.m_state = QCustomActionState::STOP;
    }

    void pauseClicked() {
        m_data.m_state = QCustomActionState::PAUSE;
    }

private:
    QCustomActionData m_data;
};

#endif