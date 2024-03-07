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

	Plugin(const Loader* ld, PluginsLoader* plugins, const QString& path = QString())
		: m_loader(ld)
		, m_plugins(plugins)
		, m_settingsPath(path) {
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
};

class API_EXPORT Extension	: public QObject
							, public Plugin {
	Q_OBJECT

public:
	Extension(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString())
		: Plugin(ld, plugins, path) {
	}

	virtual ~Extension() {}

signals:

	void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

	void status(const QString& msg) const;

public slots:

	virtual void settingsChanged() = 0;
};

class API_EXPORT Widget : public MdiChild
						, public Plugin {
	Q_OBJECT

public:

	Widget(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path = QString())
		: Plugin(ld, plugins, path)
		, MdiChild(){
	}

	virtual ~Widget() {}

signals:

	void status(const QString& msg) const;

public slots:

	virtual void settingsChanged() = 0;
};

class PluginsLoader;

class API_EXPORT Loader {

public:
	Loader(const QString& name, Plugin::Type type, const QString& version,
		const QString& author, const QString& description)
		: m_name(name)
		, m_type(type)
		, m_version(version)
		, m_author(author)
		, m_description(description){
	}

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

	virtual bool registerPlugin(Window* win, PluginsLoader* ld, Logger* log) = 0;

	virtual bool unregisterPlugin(Window* win, PluginsLoader* ld, Logger* log) = 0;

	virtual ~Loader() = default;

private:

	QString m_name;
	Plugin::Type m_type;
	QString m_version;
	QString m_author;
	QString m_description;
};

template<typename T, typename D>
class PluginLoader : public Loader {
	using RegHandler = std::function<bool(Window*, PluginsLoader* ld, D* ctx, Logger* log)>;

public:
	PluginLoader(const QString& name, Plugin::Type type, const QString& version, const QString& author, const QString& description,
		RegHandler reg, RegHandler unreg)
		: Loader(name, type, version, author, description)
		, m_register(reg)
		, m_unregister(unreg)
		, m_data(D(QCoreApplication::instance())){
	}

	QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath = QString()) const override {
		return QSharedPointer<T>(new T(
			this,
			plugins, 
			qobject_cast<std::conditional<std::is_base_of<Widget, T>::value, QWidget*, QObject*>::type>(parent),
			settingsPath
		),&QObject::deleteLater).staticCast<Plugin>();
	}

	bool registerPlugin(Window* win, PluginsLoader* ld, Logger* log) override {
		return (m_register) ? m_register(win, ld, &m_data, log) : false;
	}

	bool unregisterPlugin(Window* win, PluginsLoader* ld, Logger* log) override {
		return (m_unregister) ? m_unregister(win, ld, &m_data, log) : false;
	}

private:
	RegHandler m_register;
	RegHandler m_unregister;
	D m_data;
};

class API_EXPORT PluginsLoader : public QObject{

	Q_OBJECT

public:

	PluginsLoader() = default;
	
	virtual bool hasInstance(const QString& name, const QString& settingsPath = QString()) = 0;

	virtual auto instance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr)) = 0;

	virtual auto newInstance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr)) = 0;

	virtual ~PluginsLoader() = default;

signals:
	void loaded(const QString& name);
};

#define REGISTER_PLUGIN(name, type, version, author, description, reg, unreg, data) extern "C" { __declspec(dllexport) PluginLoader<name, data> name##Loader(#name, type, version, author, description, reg, unreg); }

class API_EXPORT Settings {

public:
	static QSettings get() {
		return QSettings("configuration.ini", QSettings::Format::IniFormat);
	}

};

#endif
