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

struct LinWriteByIDSettings : public PluginSettings {
    static constexpr const char* const frameDataKey = "frameData";
    static constexpr const char* const intervalKey = "Interval";
    static constexpr const char* const triesKey = "tries";
    static constexpr const char* const titleKey = "title";
    static constexpr const char* const dataSourceKey = "dataSource";
    static constexpr const char* const previousKey = "previous";
    static constexpr const char* const linDeviceKey = "linDevice";

    static constexpr const char* const frameDataValue = "0x6522f18c";
    static constexpr const qint64 intervalValue = 0;
    static constexpr const uint8_t triesValue = 3;
    static constexpr const char* const titleValue = "Write By Identifier";
    static constexpr const char* const dataSourceValue = "";
    static constexpr const char* const previousValue = nullptr;
    static constexpr const char* const linDeviceValue = nullptr;

    Q_GADGET

    Q_PROPERTY(QByteArray frameData READ frameData WRITE setFrameData)
    Q_PROPERTY(qint64 interval READ interval WRITE setInterval)
    Q_PROPERTY(uint8_t tries READ tries WRITE setTries)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource)
    Q_PROPERTY(QString previous READ previous WRITE setPrevious)
    Q_PROPERTY(QString linDevice READ linDevice WRITE setLinDevice)

public:
    LinWriteByIDSettings()
        : m_frameData(frameDataValue)
        , m_interval(intervalValue)
        , m_tries(triesValue)
        , m_dataSource(dataSourceValue)
        , m_previous(previousValue)
        , m_linDevice(linDeviceValue) {
        registerMetaObject(&staticMetaObject);
    }

    LinWriteByIDSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_frameData(settings.value(settingsPath + "/" + frameDataKey, frameDataValue).toByteArray())
        , m_interval(settings.value(settingsPath + "/" + intervalKey, intervalValue).toInt())
        , m_tries(settings.value(settingsPath + "/" + triesKey, triesValue).toUInt())
        , m_title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
        , m_dataSource(settings.value(settingsPath + "/" + dataSourceKey, dataSourceValue).toString())
        , m_previous(settings.value(settingsPath + "/" + previousKey, previousValue).toString())
        , m_linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + frameDataKey, m_frameData);
        settings.setValue(settingsPath + "/" + intervalKey, m_interval);
        settings.setValue(settingsPath + "/" + triesKey, m_tries);
        settings.setValue(settingsPath + "/" + titleKey, m_title);
        settings.setValue(settingsPath + "/" + dataSourceKey, m_dataSource);
        settings.setValue(settingsPath + "/" + previousKey, m_previous);
        settings.setValue(settingsPath + "/" + linDeviceKey, m_linDevice);

        PluginSettings::save(settings, settingsPath);
    }

    QByteArray frameData() const { return m_frameData; }
    void setFrameData(const QByteArray& frameData) { m_frameData = frameData; }

    // Getter and Setter for interval
    qint64 interval() const { return m_interval; }
    void setInterval(qint64 interval) { m_interval = interval; }

    // Getter and Setter for tries
    uint8_t tries() const { return m_tries; }
    void setTries(uint8_t tries) { m_tries = tries; }

    // Getter and Setter for title
    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    // Getter and Setter for dataSource
    QString dataSource() const { return m_dataSource; }
    void setDataSource(const QString& dataSource) { m_dataSource = dataSource; }

    // Getter and Setter for previous
    QString previous() const { return m_previous; }
    void setPrevious(const QString& previous) { m_previous = previous; }

    // Getter and Setter for linDevice
    QString linDevice() const { return m_linDevice; }
    void setLinDevice(const QString& linDevice) { m_linDevice = linDevice; }

    bool operator==(const LinWriteByIDSettings& other) const {
        return m_frameData == other.m_frameData &&
            m_interval == other.m_interval &&
            m_tries == other.m_tries &&
            m_title == other.m_title &&
            m_dataSource == other.m_dataSource &&
            m_previous == other.m_previous &&
            m_linDevice == other.m_linDevice;
    }

    bool operator!=(const LinWriteByIDSettings& other) const {
        return !(*this == other);
    }

private:
    QByteArray m_frameData;
    qint64 m_interval;
    uint8_t m_tries;
    QString m_title;
    QString m_dataSource;
    QString m_previous;
    QString m_linDevice;
};

class QLinWriteByID;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QLinWriteByID* write);

    ~SettingsDialog();

    operator LinWriteByIDSettings() const;

private slots:
    void ok();

    void apply();
    
    void cancel();

private:
    void updateSettings();

    void fillFromSettings();

    void setup();

private:

    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
};

#endif
