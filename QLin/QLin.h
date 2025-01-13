#ifndef QLIN_H
#define QLIN_H

#include "qlin_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "CLin.h"

struct QLinData {
	QLinData(CLin* clin, bool open = false) 
		: m_lin(clin)
		, m_open(open){
	}

	CLin* m_lin = nullptr;
	bool m_open = false;
};

class QLIN_EXPORT QLin : public IODevice {

    Q_OBJECT

public:
    QLin(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path, LinSettings* set, const QString& uuid);

	bool open(const QString& url = QString());

	qint64 write(const QString& data);

	qint64 write(const QByteArray& data);

	QString read(qint64 maxLen);

	qint64 bytesAvailable();

	void close();

	bool isOpen() const;

	bool flush();

	SettingsMdi* settingsWindow() const override;

	Q_INVOKABLE int slaveID() const;

	Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
		emit message("QLin::reset(Reset type)");
		close();
		return open();
	}

	bool initialize() override;

	bool deinitialize() override;

public slots:

	void settingsChanged();

protected slots:

	//void readData();

private:

	QLinData m_data;
	
};

#endif