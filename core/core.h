#ifndef CORE_H
#define CORE_H

#include "core_global.h"

#include <QString>
#include <QHash>
#include <QMultiHash>
#include <QLibrary>
#include <QDirIterator>
#include <QMainWindow>

#include "../api/api.h"

struct ModuleLoaderSettings {
	static constexpr const char* const modulesEnabledKey = "enabled";
	static constexpr const char* const modulesEnabledValue = nullptr;
	static constexpr const char* const modulesDirKey = "directory";
	static constexpr const char* const modulesDirValue = "./plugins";

	ModuleLoaderSettings()
		: modulesEnabled(modulesEnabledValue)
		, modulesDir(modulesDirValue){
	}

	ModuleLoaderSettings(const QSettings& settings, const QString& settingsPath)
		: modulesEnabled(settings.value(settingsPath + "/" + modulesEnabledKey, modulesEnabledValue).toStringList())
		, modulesDir(settings.value(settingsPath + "/" + modulesDirKey, modulesDirValue).toString()) {
	}

	void save(QSettings& settings, const QString& settingsPath) const {
		settings.setValue(settingsPath + "/" + modulesEnabledKey, modulesEnabled);
		settings.setValue(settingsPath + "/" + modulesDirKey, modulesDir);
	}

	QStringList modulesEnabled;
	QString modulesDir;
};

template<typename T>
struct ModuleLoaderData {
	ModuleLoaderData(Logger* logger) 
		: m_logger(logger){
	}

	ModuleLoaderData(Logger* logger, const ModuleLoaderSettings& settings) 
		: m_logger(logger)
		, m_settings(settings){}
	QHash<QString, QLibrary*> m_libraries;
	QMultiHash<QString, PluginsLoader::PluginType> m_instances;
	static QHash<QString, T*> m_loaders;
	ModuleLoaderContext* m_ctx = nullptr;
	Logger* m_logger = nullptr;
	ModuleLoaderSettings m_settings;
};

template<typename T>
class ModuleLoader : public PluginsLoader {
public:

	ModuleLoader(Logger* log = nullptr) 
		: m_data(log) {}

	ModuleLoader(const QSettings& settings, const QString& settingsPath, Logger* log = nullptr)
		: m_data(log, ModuleLoaderSettings(settings, settingsPath)) {
		setObjectName("ModuleLoader");
	}

	virtual ~ModuleLoader() {

		m_data.m_instances.clear();
		m_data.m_loaders.clear();

		//for (auto it = m_libraries.begin(), end = m_libraries.end(); it != end; ++it) {
		//	it.value()->unload();
		//}

		m_data.m_libraries.clear();
	}

	QString path() const {
		return m_data.m_settings.modulesDir;
	}

	void setContext(ModuleLoaderContext* ctx) {
		m_data.m_ctx = ctx;
	}

	ModuleLoaderContext* context() const {
		return m_data.m_ctx;
	}

	bool hasInstance(const QString& name, const QString& settingsPath = QString()) const {
		return hasInstanceInternal(name, settingsPath) != nullptr;
	}

	auto instance(const QString& name, QWidget* parent, const QString& settingsPath = QString(), const ModuleHint& hint = ModuleHint::INITIALIZE) -> PluginType {
		return (hasInstanceInternal(name, settingsPath) != nullptr) ? hasInstanceInternal(name, settingsPath) : newInstance(name, parent, settingsPath, hint);
	}

	auto newInstance(const QString& name, QWidget* parent, const QString& settingsPath = QString(), const ModuleHint& hint = ModuleHint::INITIALIZE) -> PluginType {
		if (m_data.m_loaders.contains(name) == false) {
			return nullptr;
		}

		auto ret = m_data.m_loaders[name]->load(this, parent, settingsPath);
		if (ret == nullptr) {
			return nullptr;
		}

		m_data.m_instances.insert(name, ret);
		
		QObject* o = dynamic_cast<QObject*>(ret.data());
		connect(o, SIGNAL(message(const QString&, LoggerSeverity)), logger(), SLOT(message(const QString&, LoggerSeverity)));

		if (o->objectName().isEmpty() == true) {
			const int count = m_data.m_instances.count(name) - 1;
			const QString objName = (count == 0) ? name : QString("%1 %2").arg(name).arg(count);
			o->setObjectName(objName);
		}

		if (hint == ModuleHint::INITIALIZE) {
			ret->initialize();
		}

		emit loaded(ret.data());
		return ret;
	}

	auto find(const QString& uuid) const -> PluginType {
		for (QMultiHash<QString, PluginType>::const_iterator i = m_data.m_instances.begin(); i != m_data.m_instances.end(); ++i) {
			if (i->data()->uuid() == uuid) {
				return i.value();
			}
		}

		return nullptr;
	}

	auto findByObjectName(const QString& name) const -> PluginType {
		for (QMultiHash<QString, PluginType>::const_iterator i = m_data.m_instances.begin(); i != m_data.m_instances.end(); ++i) {
			if (dynamic_cast<QObject*>(i->data())->objectName() == name) {
				return i.value();
			}
		}

		return nullptr;
	}

	const QList<const T*> loaders() const {
		QList<const T*> ret;

		for (auto i = m_data.m_loaders.cbegin(), end = m_data.m_loaders.cend(); i != end; ++i) {
			ret << i.value();
		}

		return ret;
	}

	qint32 loadPlugins() {
		m_data.m_logger->message(QString("ModuleLoader::loadPlugins()"));
		qint32 ret = 0;
		QDirIterator it(path(), { "*.dll", "*.so", "*.dylib" }, QDir::Files);

		m_data.m_logger->message(QString("ModuleLoader::loadPlugins: using plugins directory: %1").arg(it.path()));

		QStringList active = m_data.m_settings.modulesEnabled;
		bool isGui = Settings::isGui();


		m_data.m_logger->message(QString("ModuleLoader::loadPlugins: found plugins list of %1 elements %2").arg(active.size()).arg(active.join(',')));

		while (it.hasNext()) {
			QString libpath = it.next();

			m_data.m_logger->message(QString("ModuleLoader::loadPlugins: trying to load %1").arg(libpath));

			if (active.isEmpty() == false) {
				if (active.indexOf(it.fileInfo().baseName()) == -1) {
					m_data.m_logger->message(QString("ModuleLoader::loadPlugins(): module %1 not active").arg(it.fileInfo().baseName()));
					continue;
				}
			}

			QLibrary* lib = new QLibrary(libpath);
			if (lib == nullptr) {
				m_data.m_logger->message(QString("ModuleLoader::loadPlugins(): failed to open library %1").arg(libpath));
				continue;
			}

			if (lib->load() == false) {
				m_data.m_logger->message(QString("ModuleLoader::loadPlugins(): failed to load %1 %2").arg(libpath).arg(lib->errorString()));
				delete lib;
				continue;
			}

			QFileInfo info(libpath);

			QString symbol = info.baseName()+"_Loader";
			QFunctionPointer sym = lib->resolve(symbol.toLocal8Bit());

			Loader* ld = reinterpret_cast<Loader*>(sym);

			if (ld == nullptr) {
				m_data.m_logger->message(QString("ModuleLoader::loadPlugins(): failed to check library type %1").arg(libpath));
				lib->unload();
				delete lib;
				continue;
			}

			if (ld->type() == Plugin::Type::WIDGET && isGui == false) {
				logger()->message(QString("ModuleLoader::loadPlugins(): tried to load %1 widget plugin in non gui environment").arg(ld->name()));
				lib->unload();
				delete lib;
				continue;
			}

			logger()->message(QString("ModuleLoader::loadPlugins() loaded plugin name: %1 type: %2 version: %3").arg(ld->name()).arg(static_cast<qint64>(ld->type())).arg(ld->version()));
			m_data.m_libraries[ld->name()] = lib;
			m_data.m_loaders[ld->name()] = ld;
			++ret;
		}

		auto exts = loaders();

		for (auto it : exts) {
			auto deps = it->depends();
			for (auto begin = deps.begin(), end = deps.end(); begin != end; ++begin) {
				if (hasLoader(*begin) == false) {
					m_data.m_loaders[it->name()]->setEnabled(false);
					break;
				}
			}
		}

		for (int i = 0; i < exts.size(); ++i) {
			if (exts[i]->enabled() == false) {
				exts.removeAt(i);
				--i;
			}
		}

		QStringList order;

		while (exts.size() > 0) {
			for (int i = 0; i < exts.size() && exts.size() > 0; ++i) {
				if (exts[i]->depends().size() == 0) {
					order << exts[i]->name();
					exts.removeAt(i);
					--i;
				}
				else {
					QStringList tmp = exts[i]->depends();
					QSet<QString> deps(tmp.begin(), tmp.end());
					QSet<QString> met(order.begin(), order.end());
					if (deps.subtract(met).size() == 0) {
						order << exts[i]->name();
						exts.removeAt(i);
						--i;
					}
				}
			}
		}

		QList<T*> lds = m_data.m_loaders.values();
		std::sort(lds.begin(), lds.end(), [](T* lhs, T* rhs) {
			return lhs->weight() < rhs->weight();
		});

		for (auto item : lds) {
			item->registerPlugin(context(), this, logger());
		}

		return ret;
	}

	void deleteInstance(const QString& uuid) {
		m_data.m_instances.removeIf([uuid](QMultiHash<QString, PluginType>::iterator it) {
			if (it->data()->uuid() == uuid) {
				return true;
			}

			return false;
		});
	}

	QList<PluginType> instances() const {
		QList<decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr))> ret;
		for (auto it = m_data.m_instances.begin(), end = m_data.m_instances.end(); it != end; ++it) {
			ret << it.value();
		}
		return ret;
	}

	static bool registerStaticLoader(T* loader) {
		if (loader == nullptr) {
			return false;
		}

		if (ModuleLoaderData<T>::m_loaders.contains(loader->name())) {
			return false;
		}

		ModuleLoaderData<T>::m_loaders[loader->name()] = loader;

		return true;
	}

	Logger* logger() const {
		return m_data.m_logger;
	}

protected:

	QString findUnusedPath() const {
		return QString();
	}

	PluginType hasInstanceInternal(const QString& name, const QString& settingsPath) const {
		if (settingsPath.isEmpty()) {
			return m_data.m_instances.contains(name) ? m_data.m_instances[name] : nullptr;
		}

		for (QMultiHash<QString, PluginType>::const_iterator i = m_data.m_instances.find(name); i != m_data.m_instances.end() && i.key() == name; ++i) {
			if (i->data()->settingsPath() == settingsPath) {
				return i.value();
			}
		}

		return nullptr;
	}	

private:
	inline bool hasLoader(const QString& name) const {
		return m_data.m_loaders.contains(name);
	}

	ModuleLoaderData<T> m_data;
};

typedef ModuleLoader<Loader> MLoader;

template<typename T>
QHash<QString, T*> ModuleLoaderData<T>::m_loaders = QHash<QString, T*>();

#define REGISTER_STATIC_PLUGIN(name, type, version, author, description, reg, unreg, data, depends, multiple, weight) \
	PluginLoader<name, data> name##_##Loader(#name, type, version, author, description, reg, unreg, depends, multiple, weight); \
	RegisterStaticPlugin name##RegisterLoader(&name##_##Loader);

class RegisterStaticPlugin {
public:
	RegisterStaticPlugin(Loader* ld) {
		MLoader::registerStaticLoader(ld);
	}
};

struct GuiLoaderContext 
	: public ModuleLoaderContext {
	GuiLoaderContext(Window* win);

	~GuiLoaderContext() = default;
	Window* m_win = nullptr;
};

#endif
