#ifndef QWEB_H
#define QWEB_H

#include "qweb_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QWebUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QWEB_EXPORT QWeb : public Widget {

    Q_OBJECT

public:
    QWeb(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, WebSettings* set, const QString& uuid);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~QWeb();

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
    Ui::QWebUI* m_ui = nullptr;
};

#endif