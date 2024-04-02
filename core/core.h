#ifndef CORE_H
#define CORE_H

#include "core_global.h"

#include <QString>
#include <QHash>
#include <QLibrary>
#include <QDirIterator>
#include <QMainWindow>

#include "../api/api.h"

template<typename T>
class ModuleLoader : PluginsLoader {
public:

	ModuleLoader(Logger* log = nullptr) 
		: m_path(QDir::currentPath() + "/plugins")
		, m_logger(log) {}

	ModuleLoader(const QString& dir, Logger* log = nullptr)
		: m_path(dir.isEmpty() ? QDir::currentPath() + "/plugins" : dir)
		, m_logger(log) {}

	~ModuleLoader() {

		m_instances.clear();
		m_loaders.clear();

		//for (auto it = m_libraries.begin(), end = m_libraries.end(); it != end; ++it) {
		//	it.value()->unload();
		//}

		m_libraries.clear();
	}

	QString path() const {
		return m_path;
	}

	void setContext(ModuleLoaderContext* ctx) {
		m_ctx = ctx;
	}

	ModuleLoaderContext* context() const {
		return m_ctx;
	}

	bool hasInstance(const QString& name, const QString& settingsPath = QString()) {
		return m_instances.contains(name);
	}

	auto instance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> PluginType {
		return (m_instances.contains(name) == true) ? m_instances[name] : newInstance(name, parent, settingsPath);
	}

	auto newInstance(const QString& name, QWidget* parent, const QString& settingsPath = QString()) -> PluginType {
		if (m_loaders.contains(name) == false) {
			return nullptr;
		}

		auto ret = m_loaders[name]->load(this, parent, settingsPath);
		if (ret == nullptr) {
			return nullptr;
		}

		m_instances[name] = ret;

		connect(dynamic_cast<QObject*>(ret.data()), SIGNAL(message(const QString&, LoggerSeverity)), logger(), SLOT(message(const QString&, LoggerSeverity)));
		emit loaded(ret.data());
		return ret;
	}

	const QList<const T*> loaders() const {
		QList<const T*> ret;

		for (auto i = m_loaders.cbegin(), end = m_loaders.cend(); i != end; ++i) {
			ret << i.value();
		}

		return ret;
	}

	qint32 loadPlugins() {
		qint32 ret = 0;
		QDirIterator it(path(), { "*.dll", "*.so", "*.dylib" }, QDir::Files);

		QSettings settings = Settings::get();
		auto list = settings.value("plugins/active").toString();
		QStringList active;
		bool isGui = Settings::isGui();

		if (list.isEmpty() == false) {
			active = list.split(' ');
		}

		while (it.hasNext()) {
			QString libpath = it.next();
			if (active.isEmpty() == false) {
				if (active.indexOf(it.fileInfo().baseName()) == -1) {
					continue;
				}
			}
			QLibrary* lib = new QLibrary(libpath);
			if (lib == nullptr) {
				continue;
			}

			if (lib->load() == false) {
				m_logger->message(QString("ModuleLoader::loadPlugins(): failed to load %1 %2").arg(libpath).arg(lib->errorString()));
				delete lib;
				continue;
			}

			QFileInfo info(libpath);
			QString symbol = info.baseName() + "Loader";
			QFunctionPointer sym = lib->resolve(symbol.toLocal8Bit());

			Loader* ld = reinterpret_cast<Loader*>(sym);

			if (ld == nullptr) {
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
			m_libraries[ld->name()] = lib;
			m_loaders[ld->name()] = ld;
			++ret;
		}

		auto exts = loaders();

		for (auto it : exts) {
			auto deps = it->depends();
			for (auto begin = deps.begin(), end = deps.end(); begin != end; ++begin) {
				if (hasLoader(*begin) == false) {
					m_loaders[it->name()]->setEnabled(false);
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

		for (auto item : order) {
			m_loaders[item]->registerPlugin(context(), this, logger());
		}

		return ret;
	}

	QList<PluginType> instances() const {
		QList<decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr))> ret;
		for (auto it = m_instances.begin(), end = m_instances.end(); it != end; ++it) {
			ret << it.value();
		}
		return ret;
	}

	static bool registerStaticLoader(T* loader) {
		if (loader == nullptr) {
			return false;
		}

		if (m_loaders.contains(loader->name())) {
			return false;
		}

		m_loaders[loader->name()] = loader;

		return true;
	}

	Logger* logger() const {
		return m_logger;
	}

private:
	inline bool hasLoader(const QString& name) const {
		return m_loaders.contains(name);
	}

	QHash<QString, QLibrary*> m_libraries;
	QHash<QString, PluginType> m_instances;
	static QHash<QString, T*> m_loaders;
	ModuleLoaderContext* m_ctx = nullptr;
	Logger* m_logger = nullptr;
	QString m_path;
};

typedef ModuleLoader<Loader> MLoader;
template<typename T>
QHash<QString, T*> ModuleLoader<T>::m_loaders = QHash<QString, T*>();

#define REGISTER_STATIC_PLUGIN(name, type, version, author, description, reg, unreg, data, depends, multiple) \
	PluginLoader<name, data> name##Loader(#name, type, version, author, description, reg, unreg, depends, multiple); \
	RegisterStaticPlugin name##RegisterLoader(&name##Loader);

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
