#ifndef IO_DEVICE_H
#define IO_DEVICE_H

#include "plugin.h"
#include "logger.h"
#include "api_global.h"

class API_EXPORT IODevice : public Extension {

	Q_OBJECT

public:

	IODevice(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString())
		: Extension(ld, plugins, parent, path) {}

	virtual bool open(const QString& url) = 0;

	virtual qint64 write(const QString& data) = 0;

	virtual QString read(qint64 maxLen) = 0;

	virtual qint64 bytesAvailable() = 0;

	virtual void close() = 0;
};

#endif
