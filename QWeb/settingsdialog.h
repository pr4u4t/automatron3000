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

    static constexpr const char* urlKey = "url";
    static constexpr const char* urlValue = nullptr;
    static constexpr const char* refreshKey = "refresh";
    static constexpr qint32 refreshValue = -1;

    struct WebSettings : public PluginSettings {
        WebSettings() 
            : url(urlValue)
            , refresh(refreshValue){
        }

        virtual ~WebSettings() = default;

        WebSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , url(settings.value(settingsPath + "/" + urlKey, urlValue).toString())
            , refresh(settings.value(settingsPath + "/" + refreshKey, refreshValue).toInt()) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + urlKey, url);
            settings.setValue(settingsPath + "/" + refreshKey, refresh);

            PluginSettings::save(settings, settingsPath);
        }

        QString url;
        qint32 refresh;
    };

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    WebSettings dataSettings() const;

    QString settingsPath() const;

private slots:
    void ok();

    void apply();
    
    void cancel();

private:
    void updateSettings();
    
    void fillFromSettings();

private:
    WebSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif