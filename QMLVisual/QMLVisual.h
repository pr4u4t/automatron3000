#ifndef QMLVISUAL_H
#define QMLVISUAL_H

#include <QApplication>
#include <QQuickWidget>
#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>

#include "qmlvisual_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QMLVisualUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QMLVISUAL_EXPORT QMLVisual : public Widget {

    Q_OBJECT

public:
    QMLVisual(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, MLVisualSettings* set, const QString& uuid);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~QMLVisual();

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
    Ui::QMLVisualUI* m_ui = nullptr;
    QQuickWidget* m_viewer;
};

#endif