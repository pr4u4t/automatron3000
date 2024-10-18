#ifndef QBADGE_H
#define QBADGE_H

#include "qbadge_global.h"
#include "settingsdialog.h"
#include "../api/api.h"
#include "ImageDisplay.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QBadgeUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QBADGE_EXPORT QBadge : public Widget {

    Q_OBJECT

public:
    QBadge(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~QBadge();
    
    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QBadge::reset(Reset type)");
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:

    void settingsChanged();

    QVariant exec() override {
        return QVariant();
    }

private:
    Ui::QBadgeUI* m_ui = nullptr;
};

#endif