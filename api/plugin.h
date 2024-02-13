#ifndef PLUGIN_H
#define PLUGIN_H

#include <functional>
#include <type_traits>

#include <QObject>
#include <QSharedPointer>
#include <QUuid>
#include <QRegExp>
#include <QCloseEvent>

#include "api_global.h"
#include "window.h"
#include "mdichild.h"

class Loader;
class PluginsLoader;

class API_EXPORT Plugin {

public:

	enum class Type {
		INVALID		= 0,
		EXTENSION	= 1 << 0,
		WIDGET		= 1 << 1,
		IODEVICE	= 1 << 2
	};

	static constexpr qint64 InvalidUUID = -1;
	static constexpr const char* const settings_path = "plugins/";

	Plugin(const Loader* ld, PluginsLoader* plugins/*, QObject* parent*/, const QString& path = QString())
		: m_loader(ld),
		m_plugins(plugins),
		m_settingsPath(path)//,
		/*m_parentWidget(parent)*/ {
		QRegExp rx("([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})");
		int idx;

		if (settingsPath().isEmpty()) {
			m_uuid = QUuid::createUuid().toString();
			m_settingsPath = QString(settings_path) + name() + "-" + m_uuid;
		} else if ((idx = rx.indexIn(path)) != -1) {
			m_uuid = "{"+rx.cap(1)+"}";
			m_settingsPath = QString(settings_path) + path;
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

class API_EXPORT Extension : public QObject, public Plugin {
	Q_OBJECT

public:
	Extension(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString())
		: Plugin(ld, plugins/*, QObject* parent*/, path) {
	}

};

class API_EXPORT Widget : public MdiChild, public Plugin {
	Q_OBJECT

public:

	Widget(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path = QString())
		: Plugin(ld, plugins/*, QObject* parent*/, path), MdiChild(parent){
	}
};

class PluginsLoader;

class API_EXPORT Loader {
public:
	using RegHandler = std::function<bool(Window*, PluginsLoader* ld)>;

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

	virtual QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath = QString()) const = 0;

	bool registerPlugin(Window* win, PluginsLoader* ld) {
		return (m_register) ? m_register(win, ld) : false;
	}

	bool unregisterPlugin(Window* win, PluginsLoader *ld) {
		return (m_unregister) ? m_unregister(win, ld) : false;
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

	QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath = QString()) const {
		return QSharedPointer<T>(new T(
			this,
			plugins, 
			qobject_cast<std::conditional<std::is_base_of<Widget, T>::value, QWidget*, QObject*>::type>(parent),
			settingsPath
		)).staticCast<Plugin>();
	}
};

class API_EXPORT PluginsLoader : public QObject{

	Q_OBJECT

public:
	PluginsLoader() = default;

	virtual auto instance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr)) = 0;

	virtual auto newInstance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr)) = 0;

	virtual ~PluginsLoader() = default;

signals:
	void loaded(const QString& name);
};

#define REGISTER_PLUGIN(name, type, version, author, description, reg, unreg) extern "C" { __declspec(dllexport) PluginLoader<name> name##Loader(#name, type, version, author, description, reg, unreg); }

#endif
