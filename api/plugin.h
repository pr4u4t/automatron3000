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

#include <QStack>
#include <QGuiApplication>
#include <QApplication>
#include "window.h"
#include "mdichild.h"

class Loader;
class PluginsLoader;
class SettingsMdi;

struct API_EXPORT PluginSettings {
	
	Q_GADGET
	Q_PROPERTY(QString objectName READ objectName WRITE setObjectName)
public:

	PluginSettings() = default;

	PluginSettings(const QSettings& settings, const QString& settingsPath)
		: m_objectName(settings.value(settingsPath + "/objectName").toString()) {
		registerMetaObject(&staticMetaObject);
	}

	virtual void save(QSettings& settings, const QString& settingsPath) const {
		settings.setValue(settingsPath + "/objectName", m_objectName);
	}

	virtual ~PluginSettings() = default;

	void setObjectName(const QString& objectName) {
		m_objectName = objectName;
	}

	QString objectName() const {
		return m_objectName;
	}

	const QStack<const QMetaObject*>& metaStack() const {
		return m_metaStack;
	}

protected:

	void registerMetaObject(const QMetaObject* o) {
		if (!m_metaStack.contains(o)) {
			m_metaStack.push(o);
		}
	}

private:

	QString m_objectName;
	QStack<const QMetaObject*> m_metaStack;
};

struct PluginPrivate {

	PluginPrivate(Loader* ld, PluginsLoader* plugins, const QString& path = QString(), PluginSettings* set = nullptr, const QString& uuid = QString());

	Loader* m_loader = nullptr;
	PluginsLoader* m_plugins = nullptr;
	QString m_uuid;
	QString m_settingsPath;
	PluginSettings* m_settings = nullptr;
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

	enum class Reset {
		SOFT = 0,
		HARD
	};

	static constexpr qint64 InvalidUUID = -1;

	Plugin(Loader* ld, PluginsLoader* plugins, const QString& path = QString(), PluginSettings* set = nullptr, const QString &uuid = QString());

	virtual ~Plugin() = default;

	QString name() const;

	QString version() const;

	QString author() const;

	QString description() const;

	QStringList depends() const;

	Plugin::Type type() const;

	QString uuid() const;

	QString settingsPath() const;

	bool multipleInstances() const;

	virtual SettingsMdi* settingsWindow() const = 0;

	Q_INVOKABLE virtual bool reset(Reset type = Reset::SOFT) = 0;

	virtual bool initialize() = 0;

	virtual bool deinitialize() = 0;

	template<typename T>
	T* settings() const {
		return dynamic_cast<T*>(m_d.m_settings);
	}

	template<>
	PluginSettings* settings() const {
		return m_d.m_settings;
	}

protected:

	PluginsLoader* plugins() const;

	Loader* loader() const;

	//void updateSettings(const PluginSettings& settings);

private:
	PluginPrivate m_d;
};

class API_EXPORT Extension	: public QObject
							, public Plugin {
	Q_OBJECT

public:
	Extension(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path = QString(), PluginSettings* set = nullptr, const QString& uuid = QString());

	virtual ~Extension() = default;

signals:

	void settingsApplied();

	void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

	void status(const QString& msg) const;

public slots:

	virtual void settingsChanged() = 0;

protected slots:

	void nameChanged(const QString& name);
};

class API_EXPORT Widget : public MdiChild
						, public Plugin {
	Q_OBJECT

public:

	Widget(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path = QString(), PluginSettings* set = nullptr, const QString& uuid = QString());

	virtual ~Widget() = default;

signals:

	void settingsApplied();

	void status(const QString& msg) const;

public slots:

	virtual void settingsChanged() = 0;

	virtual QVariant exec() = 0;

protected slots:

	void nameChanged(const QString& name);
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
		const QString& author, const QString& description, const QStringList& depends = QStringList(), bool multiple = false, qint32 weight = 100);

	QString m_name;
	Plugin::Type m_type;
	QString m_version;
	QString m_author;
	QString m_description;
	QStringList m_depends;
	bool m_enabled;
	bool m_multiple;
	qint32 m_weight;
};

enum class ModuleHint {
	INITIALIZE = 0,
	DONT_INITIALIZE = 1
};

class API_EXPORT Loader {

public:
	Loader(const QString& name, Plugin::Type type, const QString& version,
		const QString& author, const QString& description, const QStringList& depends = QStringList(), bool multiple = false, qint32 weight = 100);

	QString name() const;

	Plugin::Type type() const;

	QString version() const;

	QString author() const;

	QString description() const;

	QStringList depends() const;

	bool enabled() const;

	bool multipleInstances() const;

	virtual QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath = QString(), PluginSettings* srcset = nullptr) const = 0;

	virtual PluginSettings* settings(const QString& path) const = 0;

	virtual PluginSettings* settings() const = 0;

	virtual bool registerPlugin(ModuleLoaderContext* ctx, PluginsLoader* ld, Logger* log) = 0;

	virtual bool unregisterPlugin(ModuleLoaderContext* ctx, PluginsLoader* ld, Logger* log) = 0;

	virtual ~Loader() = default;

	void setEnabled(bool enabled);

	qint32 weight() const;

private:

	LoaderPrivate m_d;
};

class API_EXPORT Settings {

public:
	static auto get() -> QSettings {
		return QSettings((m_confPath.isEmpty()) ? "configuration.ini" : m_confPath + "/" + "configuration.ini", QSettings::Format::IniFormat);
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

	static bool locked() {
		return Settings::get().value("locked").toBool();
	}

	static void setConfigurationPath(const QString& path) {
		m_confPath = path;
	}

	static QString settingsPath() {
		return m_confPath;
	}

private:

	static QString m_confPath;
	static QHash<QString, PluginSettings*> m_psettings;

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

template<typename T, typename D, typename S>
class PluginLoader : public Loader {
	
	using RegHandler = decltype(static_cast<PluginLoaderPrivate<D>*>(nullptr)->m_register);

public:
	PluginLoader(const QString& name, Plugin::Type type, const QString& version, const QString& author, const QString& description,
		RegHandler reg, RegHandler unreg, const QStringList& depends, bool multiple, qint32 weight)
		: Loader(name, type, version, author, description, depends, multiple, weight)
		, m_d(reg, unreg, D(QCoreApplication::instance())) {
	}

	QSharedPointer<Plugin> load(PluginsLoader* plugins, QObject* parent, const QString& settingsPath = QString(), PluginSettings* srcset = nullptr) const {
		QString path;
		QString uuid;
		
		auto opt = uuidFromPath(settingsPath);
		if (opt.has_value() == false) {
			return nullptr;
		}
			
		uuid = opt.value();
		path = settingsPath;
		S* setts = nullptr;
		
		if (srcset != nullptr) {
			const auto src = dynamic_cast<S*>(srcset);
			if (src == nullptr) {
				return nullptr;
			}
			setts = new S(*src);
		} else {
			auto s = Settings::get();
			setts = new S(s, path);
		}

		if (setts == nullptr) {
			return nullptr;
		}

		return QSharedPointer<T>(new T(
			const_cast<PluginLoader<T, D, S>*>(this),
			plugins,
			qobject_cast<std::conditional<std::is_base_of<Widget, T>::value, QWidget*, QObject*>::type>(parent),
			path,
			setts,
			uuid
		), &QObject::deleteLater).staticCast<Plugin>();
	}

	PluginSettings* settings(const QString& path) const {
		auto set = Settings::get();
		return new S(set, path);
	}

	PluginSettings* settings() const {
		return new S;
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

protected:

	inline std::optional<QString> uuidFromPath(const QString& settingsPath) const {
		QRegExp rx("([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})");
		int idx;

		if ((idx = rx.indexIn(settingsPath)) != -1) {
			return "{" + rx.cap(1) + "}";
		}

		return std::nullopt;
	}

private:
	
	PluginLoaderPrivate<D> m_d;
};

class API_EXPORT PluginsLoader : public QObject {

	Q_OBJECT

public:

	using PluginType = decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr));
	using SettingsType = decltype((static_cast<Loader*>(nullptr))->settings());
	using PluginRawType = decltype((static_cast<Loader*>(nullptr))->load(nullptr, nullptr).data());

	PluginsLoader() = default;

	virtual bool hasInstance(const QString& name, const QString& settingsPath = QString()) const = 0;

	virtual auto instance(const QString& name, QWidget* parent, const QString& settingsPath = QString(), const ModuleHint& hint = ModuleHint::INITIALIZE) -> PluginType = 0;

	virtual auto newInstance(const QString& name, QWidget* parent, const QString& settingsPath = QString(), const ModuleHint& hint = ModuleHint::INITIALIZE) -> PluginType = 0;

	virtual auto copy(const QString& uuid) -> PluginType = 0;

	virtual auto find(const QString& uuid) const -> PluginType = 0;

	virtual auto findByObjectName(const QString& name) const -> PluginType = 0;

	virtual void deleteInstance(const QString& uuid) = 0;

	virtual ~PluginsLoader() = default;

	virtual auto newSettings(const QString& name) const -> SettingsType = 0;

	virtual auto fetchSettings(const QString& path) const -> SettingsType = 0;

signals:
	void loaded(const Plugin* plugin);

	void aboutToDelete(const Plugin* plugin);

	void enabled(const Loader* loader);

	void disabled(const Loader* loader);
};

#define REGISTER_PLUGIN(name, type, version, author, description, reg, unreg, data, depends, multiple, weight, settings) \
extern "C" { __declspec(dllexport) PluginLoader<name, data, settings> name##_##Loader(#name, type, version, author, description, reg, unreg, depends, multiple, weight); }

#define LOADER_NAME(x) #x "_Loader"

#endif
