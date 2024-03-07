#ifndef QSERIAL_H
#define QSERIAL_H

#include <QSerialPort>
#include <QMessageBox>
#include <QString>

#include "qserial_global.h"
#include "../api/api.h"
#include "settingsdialog.h"

class QSERIAL_EXPORT QSerial : public IODevice {

	Q_OBJECT

public:
	QSerial(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString());

	bool open(const QString& url);

	qint64 write(const QString& data);

	QString read(qint64 maxLen);

	qint64 bytesAvailable();

	void close();

	bool isOpen() const;

public slots:

	void settingsChanged();

private:

	QSerialPort* m_serial = nullptr;
	SettingsDialog::SerialSettings m_settings;
};

#endif