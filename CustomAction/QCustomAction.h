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

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        return true;
    }

public slots:

    void settingsChanged();

    //protected:

    //    void resizeEvent(QResizeEvent* event);

    void execClicked(bool checked = false);

private:
    Ui::QCustomActionUI* m_ui = nullptr;
};

#endif