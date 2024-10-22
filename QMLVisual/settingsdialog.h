#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>

#include "../api/api.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    static constexpr const char* viewerPathKey = "viewerPath";
    static constexpr const char* viewerPathValue = nullptr;

    static constexpr const char* searchPathKey = "searchPath";
    static constexpr const char* searchPathValue = nullptr;

    struct MLVisualSettings : public PluginSettings {
        MLVisualSettings()
            : viewerPath(viewerPathValue)
            , searchPath(searchPathValue){
        }

        virtual ~MLVisualSettings() = default;

        MLVisualSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , viewerPath(settings.value(settingsPath + "/" + viewerPathKey, viewerPathValue).toString())
            , searchPath(settings.value(settingsPath + "/" + searchPathKey, searchPathValue).toString()) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + viewerPathKey, viewerPath);
            settings.setValue(settingsPath + "/" + searchPathKey, searchPath);

            PluginSettings::save(settings, settingsPath);
        }

        QString viewerPath;
        QString searchPath;
    };

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    MLVisualSettings visualSettings() const;

    QString settingsPath() const;

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

private:
    MLVisualSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif