#ifndef PLUGIN_H
#define PLUGIN_H

#include <functional>
#include <type_traits>

#include <QObject>
#include <QSharedPointer>
#include <QUuid>
#include <QRegExp>
#include <QCloseEvent>
#include <QLocale>
#include <QCoreApplication>
#include "api_global.h"

#include <QGuiApplication>
#include <QApplication>
#include "window.h"
#include "mdichild.h"

class Loader;
class PluginsLoader;

struct PluginPrivate {

	PluginPrivate(Loader* ld, PluginsLoader* plugins, const QString& path = QString());

	Loader* m_loader = nullptr;
	PluginsLoader* m_plugins = nullptr;
	QString m_uuid = 0;
	QString m_settingsPath;
};

class API_EXPORT Plugin {

public:

	enum class Type {
		INVALID		= 0,
		EXTENSION	= 1 << 0,
		WIDGET		= 1 << 1,
		IODEVICE	= 1 << 2,
		MODEL		= 1 << 3
	};

	static constexpr qint64 InvalidUUID = -1;
	static constexpr const char* const settings_path = "plugins/";

	Plugin(Loader* ld, PluginsLoader* plugins, const QString& path = QString());

	QString name() const;

	QString version() const;

	QString author() const;

	QString description() const;

	QStringList depends() const;

	Plugin::Type type() const;

	QString uuid() const;

	virtual ~Plugin() = default;

	QString settingsPath() const;

	bool multipleInstances() const;

protected:

	PluginsLoader* plugins() const;

	Loader* loader() const;

private:
	PluginPrivate m_d;
};

class API_EXPORT Extension	: public QObject
							, public Plugin {
	Q_OBJECT

public:
	Extension(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString());

	virtual ~Extension() = default;

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

	Widget(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path = QString());

	virtual ~Widget() = default;

signals:

	void status(const QString& msg) const;

public slots:

	virtual void settingsChanged() = 0;
};

class PluginsLoader;

struct API_EXPORT ModuleLoaderContext {

	ModuleLoaderContext();
	virtual ~ModuleLoaderContext();

	template<typename T>
	T* to() {
		return dynamic_cast<T*>(this);
	}
};

struct LoaderPrivate {

	LoaderPrivate(const QString& name, Plugin::Type type, const QString& version,
		const QString& author, const QString& description, const QStringList& depends = QStringList(), bool multiple = false);

	QString m_name;
	Plugin::Type m_type;
	QString m_version;
	QString m_author;
	QString m_description;
	QStringList m_depends;
	bool m_enabled;
	bool m_multiple;
};

class API_EXPORT Loader {

public:
	Loader(const QString& name, Plugin::Type type, const QString& version,
		const QString& author, const QString& description, const QStringList& depends = QStringList(), bool multiple = false);

	QString name() const;

	Plugin::Type type() const;

	QString version() const;

	QString author() const;

	QString description() const;

	QStringList depends() const;

	bool enabled() const;

	bool multipleInstances() const;

	virtual QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath = QString()) const = 0;

	virtual bool registerPlugin(ModuleLoaderContext* ctx, PluginsLoader* ld, Logger* log) = 0;

	virtual bool unregisterPlugin(ModuleLoaderContext* ctx, PluginsLoader* ld, Logger* log) = 0;

	virtual ~Loader() = default;

	void setEnabled(bool enabled);

private:

	LoaderPrivate m_d;
};

template<typename D>
struct PluginLoaderPrivate {
	using RegHandler = std::function<bool(ModuleLoaderContext* ldctx, PluginsLoader* ld, D* ctx, Logger* log)>;

	PluginLoaderPrivate(RegHandler reg, RegHandler unreg, const D& ctx)
		: m_register(reg)
		, m_unregister(unreg)
		, m_data(ctx) {
	}

	RegHandler m_register;
	RegHandler m_unregister;
	D m_data;
};

template<typename T, typename D>
class PluginLoader : public Loader {
	
	using RegHandler = decltype(static_cast<PluginLoaderPrivate<D>*>(nullptr)->m_register);

public:
	PluginLoader(const QString& name, Plugin::Type type, const QString& version, const QString& author, const QString& description,
		RegHandler reg, RegHandler unreg, const QStringList& depends, bool multiple)
		: Loader(name, type, version, author, description, depends, multiple)
		, m_d(reg, unreg, D(QCoreApplication::instance())) {
	}

	QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath) const {
		return QSharedPointer<T>(new T(
			const_cast<PluginLoader<T, D>*>(this),
			plugins,
			qobject_cast<std::conditional<std::is_base_of<Widget, T>::value, QWidget*, QObject*>::type>(parent),
			settingsPath
		), &QObject::deleteLater).staticCast<Plugin>();
	}

	bool registerPlugin(ModuleLoaderContext* ldctx, PluginsLoader* ld, Logger* log) {
		return (m_d.m_register) ? m_d.m_register(ldctx, ld, &m_d.m_data, log) : false;
	}

	bool unregisterPlugin(ModuleLoaderContext* ldctx, PluginsLoader* ld, Logger* log) {
		return (m_d.m_unregister) ? m_d.m_unregister(ldctx, ld, &m_d.m_data, log) : false;
	}

	D* context() {
		return &m_d.m_data;
	}

private:
	
	PluginLoaderPrivate<D> m_d;
};

class API_EXPORT PluginsLoader : public QObject{

	Q_OBJECT

public:

	using PluginType = decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr));

	PluginsLoader() = default;
	
	virtual bool hasInstance(const QString& name, const QString& settingsPath = QString()) = 0;

	virtual auto instance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> PluginType = 0;

	virtual auto newInstance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> PluginType = 0;

	virtual ~PluginsLoader() = default;

signals:
	void loaded(const Plugin* plugin);
};

#define REGISTER_PLUGIN(name, type, version, author, description, reg, unreg, data, depends, multiple) \
extern "C" { __declspec(dllexport) PluginLoader<name, data> name##Loader(#name, type, version, author, description, reg, unreg, depends, multiple); }

class API_EXPORT Settings {

public:
	static QSettings get() {
		return QSettings("configuration.ini", QSettings::Format::IniFormat);
	}

	static bool localeNeeded() {
		QLocale locale = QLocale::system();
		QString lcName = locale.name();
		bool lcEnabled = (Settings::get().value("translations").toString() == "true") ? true : false;

		return (lcName.startsWith("en") == false && lcEnabled == true);
	}

	static bool isGui() {
		return qobject_cast<QGuiApplication*>(QCoreApplication::instance()) != nullptr;
	}
};

struct API_EXPORT PluginSettings {

public:

	PluginSettings() = default;

	virtual void save(QSettings& settings, const QString& settingsPath) const = 0;

	virtual ~PluginSettings() = default;
};

#endif
