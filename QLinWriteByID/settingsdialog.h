#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>

#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:


    static constexpr const char* const frameDataKey = "frameData";
    static constexpr const char* const intervalKey = "Interval";
    static constexpr const char* const triesKey = "tries";
    static constexpr const char* const titleKey = "title";
    static constexpr const char* const dataSourceKey = "dataSource";

    static constexpr const char* const frameDataValue = "0x6522f18c";
    static constexpr const qint64 intervalValue = 0;
    static constexpr const uint8_t triesValue = 3;
    static constexpr const char* const titleValue = "Write By Identifier";
    static constexpr const char* const dataSourceValue = "";

    struct LinWriteByIDSettings : public PluginSettings {
        LinWriteByIDSettings()
            : frameData(frameDataValue)
            , interval(intervalValue)
            , tries(triesValue)
            , dataSource(dataSourceValue){
        }

        LinWriteByIDSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , frameData(settings.value(settingsPath + "/" + frameDataKey, frameDataValue).toByteArray())
            , interval(settings.value(settingsPath + "/" + intervalKey, intervalValue).toInt())
            , tries(settings.value(settingsPath + "/" + triesKey, triesValue).toUInt())
            , title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
            , dataSource(settings.value(settingsPath + "/" + dataSourceKey, dataSourceValue).toString()){
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + frameDataKey, frameData);
            settings.setValue(settingsPath + "/" + intervalKey, interval);
            settings.setValue(settingsPath + "/" + triesKey, tries);
            settings.setValue(settingsPath + "/" + titleKey, title);
            settings.setValue(settingsPath + "/" + dataSourceKey, dataSource);

            PluginSettings::save(settings, settingsPath);
        }

        QByteArray frameData;
        qint64 interval;
        uint8_t tries;
        QString title;
        QString dataSource;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    LinWriteByIDSettings lintesterSettings() const;

    QString settingsPath() const {
        return m_settingsPath;
    }

private slots:
    void ok();
    void apply();
    void cancel();

private:
    void updateSettings();
    void fillFromSettings();

private:
    LinWriteByIDSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif
