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
	ModuleLoader(W* mWin = nullptr,const QString& dir = QString()) 
	: m_path(dir.isEmpty() ? QDir::currentPath() + "/plugins" : dir),
	m_win(mWin){}

	~ModuleLoader() {
		for (auto it = m_instances.begin(), end = m_instances.end(); it != end; ++it) {
			//it.value()->saveSettings();
		}

		m_instances.clear();
		m_loaders.clear();

		for (auto it = m_libraries.begin(), end = m_libraries.end(); it != end; ++it) {
			it.value()->unload();
		}

		m_libraries.clear();
	}

	QString path() const {
		return m_path;
	}

	auto instance(const QString& name, QWidget *parent) -> decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr)) {
		return (m_instances.contains(name) == true) ? m_instances[name] : newInstance(name, parent);
	}

	auto newInstance(const QString& name, QWidget *parent) -> decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr)) {
		if (m_loaders.contains(name) == false) {
			return nullptr;
		}

		auto ret = m_loaders[name]->load(this, parent);
		if (ret == nullptr) {
			return nullptr;
		}

		m_instances[name] = ret;
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

			qDebug().noquote() << "ModuleLoader::ModuleLoader loaded plugin name: " << ld->name() << " type: " << static_cast<qint64>(ld->type()) << " version: " << ld->version();
			m_libraries[ld->name()] = lib;
			m_loaders[ld->name()] = ld;
			++ret;

			ld->registerPlugin(m_win);
		}

		return ret;
	}

	W* window() {
		return m_win;
	}

	void setWindow(W* win) {
		m_win = win;
	}

private:
	QHash<QString, QLibrary*> m_libraries;
	QHash<QString, decltype((static_cast<T*>(nullptr))->load(nullptr, nullptr))> m_instances;
	QHash<QString, T*> m_loaders;
	QString m_path;
	W* m_win;
};

typedef ModuleLoader<Loader, Window> MLoader;

#endif
