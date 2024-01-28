#ifndef PLUGIN_H
#define PLUGIN_H

#include <functional>

#include <QObject>
#include <QSharedPointer>
#include <QUuid>
#include <QRegularExpression>

#include "api_global.h"
#include "window.h"
#include "mdichild.h"

class Loader;
class PluginsLoader;

class API_EXPORT Plugin {

public:

	enum class Type {
		INVALID		= 0,
		WIDGET		= 1 << 0 
	};

	static constexpr qint64 InvalidUUID = -1;
	static constexpr const char* const settings_path = "plugins/";

	Plugin(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString())
		: m_loader(ld),
		m_plugins(plugins),
		m_settingsPath(path),
		/*m_parentWidget(parent)*/ {
		int idx;

		if (settingsPath().isEmpty()) {
			m_uuid = QUuid::createUuid().toString();
			m_settingsPath = QString(settings_path) + name() + "-" + m_uuid;
		} else if ((idx = settingsPath().indexOf(QRegularExpression(name() + "-[0-9A-F]{8}-[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{12}$"))) != -1) {
			m_uuid = settingsPath().right(idx);
		}
	}

	QString name() const;

	QString version() const;

	QString author() const;

	QString description() const;

	Plugin::Type type() const;

	QString uuid() const;

	//virtual bool saveSettings() const = 0;

	virtual ~Plugin() = default;

	QString settingsPath() const {
		return m_settingsPath;
	}

	virtual QSharedPointer<MdiChild> widget() = 0;

protected:

	//QWidget* parentWidget() const;

	PluginsLoader* plugins() const;

private:
	const Loader* const m_loader = nullptr;
	PluginsLoader* const m_plugins = nullptr;
	QString m_uuid = 0;
	QString m_settingsPath;
	//QWidget* m_parentWidget = nullptr;
};

class API_EXPORT PluginWidget : public MdiChild, Plugin {
	Q_OBJECT

public:

	PluginWidget(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString()) {
		: 
	}

};

class API_EXPORT Loader {
public:
	using RegHandler = std::function<bool(Window*)>;

	Loader(const QString& name, Plugin::Type type, const QString& version,
		const QString& author, const QString& description, RegHandler reg,
		RegHandler unreg)
		: m_name(name), m_type(type), m_version(version),
		m_author(author), m_description(description),
		m_register(reg), m_unregister(unreg){}

	QString name() const {
		return m_name;
	}

	Plugin::Type type() const {
		return m_type;
	}

	QString version() const {
		return m_version;
	}

	QString author() const {
		return m_author;
	}

	QString description() const {
		return m_description;
	}

	virtual QSharedPointer<Plugin> load(PluginsLoader* plugins, QWidget* parent, const QString& settingsPath = QString()) const = 0;

	bool registerPlugin(Window* win) {
		return (m_register) ? m_register(win) : false;
	}

	bool unregisterPlugin(Window* win) {
		return (m_unregister) ? m_unregister(win) : false;
	}

	virtual ~Loader() = default;

private:

	QString m_name;
	Plugin::Type m_type;
	QString m_version;
	QString m_author;
	QString m_description;
	RegHandler m_register;
	RegHandler m_unregister;

};

template<typename T>
class PluginLoader : public Loader {
public:
	PluginLoader(const QString& name, Plugin::Type type, const QString& version, const QString& author, const QString& description,
		Loader::RegHandler reg, Loader::RegHandler unreg)
		: Loader(name, type, version, author, description, reg, unreg) {}

	QSharedPointer<Plugin> load(PluginsLoader* plugins, QWidget* parent, const QString& settingsPath = QString()) const {
		return QSharedPointer<Plugin>(new T(this, plugins, parent, settingsPath));
	}
};

class API_EXPORT PluginsLoader : public QObject{

	Q_OBJECT

public:
	PluginsLoader() = default;

	virtual auto instance(const QString& name, QWidget* parent) -> decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr)) = 0;

	virtual auto newInstance(const QString& name, QWidget* parent) -> decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr)) = 0;

	virtual ~PluginsLoader() = default;

signals:
	void loaded(const QString& name);
};

#define REGISTER_PLUGIN(name, type, version, author, description, reg, unreg) extern "C" { __declspec(dllexport) PluginLoader<name> name##Loader(#name, type, version, author, description, reg, unreg); }

#endif
