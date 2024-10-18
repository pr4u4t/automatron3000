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

class QCUSTOMACTION_EXPORT QCustomAction : public Widget {

    Q_OBJECT

public:
    QCustomAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

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

private:
    Ui::QCustomActionUI* m_ui = nullptr;
};

#endif