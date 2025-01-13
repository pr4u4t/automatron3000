#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>

#include "../api/api.h"
#include "qmlvisual_global.h"
#include <QMetaObject>

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

class MLVisualSettings : public PluginSettings {
    static constexpr const char* viewerPathKey = "viewerPath";
    static constexpr const char* viewerPathValue = nullptr;

    static constexpr const char* searchPathKey = "searchPath";
    static constexpr const char* searchPathValue = nullptr;

    Q_GADGET

    Q_PROPERTY(QString viewerPath READ viewerPath WRITE setViewerPath)
    Q_PROPERTY(QString searchPath READ searchPath WRITE setSearchPath)
    
public:
    MLVisualSettings()
        : m_viewerPath(viewerPathValue)
        , m_searchPath(searchPathValue) {
        registerMetaObject(&staticMetaObject);
    }

    virtual ~MLVisualSettings() = default;

    MLVisualSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_viewerPath(settings.value(settingsPath + "/" + viewerPathKey, viewerPathValue).toString())
        , m_searchPath(settings.value(settingsPath + "/" + searchPathKey, searchPathValue).toString()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + viewerPathKey, m_viewerPath);
        settings.setValue(settingsPath + "/" + searchPathKey, m_searchPath);

        PluginSettings::save(settings, settingsPath);
    }

    // Getter and Setter for viewerPath
    QString viewerPath() const { return m_viewerPath; }
    void setViewerPath(const QString& viewerPath) { m_viewerPath = viewerPath; }

    // Getter and Setter for searchPath
    QString searchPath() const { return m_searchPath; }
    void setSearchPath(const QString& searchPath) { m_searchPath = searchPath; }

    bool operator==(const MLVisualSettings& other) const {
        return m_viewerPath == other.m_viewerPath &&
            m_searchPath == other.m_searchPath;
    }

    bool operator!=(const MLVisualSettings& other) const {
        return !(*this == other);
    }

private:

    QString m_viewerPath;
    QString m_searchPath;
};

class QMLVisual;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QMLVisual* visual);

    ~SettingsDialog();

    operator MLVisualSettings() const;

private slots:
    void ok();

    void apply();

    void cancel();

    void selectViewPath();

    void selectSearchPath();

private:
    void updateSettings();

    void fillFromSettings();

    bool verifySettings() const;

    void setup();

private:
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
};

#endif