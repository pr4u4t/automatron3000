#ifndef MODULE_LOADER_H
#define MODULE_LOADER_H

#include <QString>
#include <QHash>
#include <QLibrary>
#include <QDirIterator>
#include <QMainWindow>

#include "api/api.h"

template<typename T, typename W>
class ModuleLoader : PluginsLoader{
public:
	ModuleLoader(const QString& dir = QString(), Logger* log = nullptr)
	: m_path(dir.isEmpty() ? QDir::currentPath() + "/plugins" : dir)
	, m_logger(log){}

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

	bool hasInstance(const QString& name, const QString& settingsPath = QString()) {
		return m_instances.contains(name);
	}

	auto instance(const QString& name, QWidget *parent, const QString& settingsPath = QString()) -> decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr)) {
		return (m_instances.contains(name) == true) ? m_instances[name] : newInstance(name, parent, settingsPath);
	}

	auto newInstance(const QString& name, QWidget *parent, const QString& settingsPath = QString()) -> decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr)) {
		if (m_loaders.contains(name) == false) {
			return nullptr;
		}

		auto ret = m_loaders[name]->load(this, parent, settingsPath);
		if (ret == nullptr) {
			return nullptr;
		}

		m_instances[name] = ret;

		connect(dynamic_cast<QObject*>(ret.data()),SIGNAL(message(const QString&, LoggerSeverity)), logger(), SLOT(message(const QString&, LoggerSeverity)));

		return ret;
	}

	const QList<const T*> loaders() const {
		QList<const T*> ret;

		for (auto i = m_loaders.cbegin(), end = m_loaders.cend(); i != end; ++i) {
			ret << i.value();
		}

		return ret;
	}

	qint32 loadPlugins(W* win) {
		qint32 ret = 0;
		QDirIterator it(path(), { "*.dll", "*.so", "*.dylib"}, QDir::Files);
		while (it.hasNext()) {
			QString libpath = it.next();
			QLibrary* lib = new QLibrary(libpath);
			if (lib == nullptr) {
				//failed
				continue;
			}

			if (lib->load() == false) {
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

			logger()->message(QString("ModuleLoader::loadPlugins() loaded plugin name: %1 type: %2 version: %3").arg(ld->name()).arg(static_cast<qint64>(ld->type())).arg(ld->version()));
			m_libraries[ld->name()] = lib;
			m_loaders[ld->name()] = ld;
			++ret;
		}

		auto exts = loaders();
		QStringList order;

		while (exts.size() > 0) {
			for (int i = 0; i < exts.size() && exts.size() > 0; ++i) {
				if (exts[i]->depends().size() == 0) {
					order << exts[i]->name();
					exts.removeAt(i);
					--i;
				} else {
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
			m_loaders[item]->registerPlugin(win, this, logger());
		}

		return ret;
	}

	QList<decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr))> instances() const {
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
	QHash<QString, QLibrary*> m_libraries;
	QHash<QString, decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr))> m_instances;
	static QHash<QString, T*> m_loaders;
	QString m_path;
	Logger* m_logger = nullptr;
};

typedef ModuleLoader<Loader, Window> MLoader;
template<typename T, typename W>
QHash<QString, T*> ModuleLoader<T,W>::m_loaders = QHash<QString, T*>();

#define REGISTER_STATIC_PLUGIN(name, type, version, author, description, reg, unreg, data) \
	PluginLoader<name, data> name##Loader(#name, type, version, author, description, reg, unreg); \
	RegisterStaticPlugin name##RegisterLoader(&name##Loader);

class RegisterStaticPlugin {
public:
	RegisterStaticPlugin(Loader* ld) {
		MLoader::registerStaticLoader(ld);
	}
};

#endif
