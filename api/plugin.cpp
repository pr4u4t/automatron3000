#include "plugin.h"

QString Settings::m_confPath = QString();
QHash<QString, PluginSettings*> Settings::m_psettings = QHash<QString, PluginSettings*>();

PluginPrivate::PluginPrivate(Loader* ld, PluginsLoader* plugins, const QString& path, PluginSettings* set, const QString& uuid)
	: m_loader(ld)
	, m_plugins(plugins)
	, m_settingsPath(path)
	, m_settings(set)
	, m_uuid(uuid){
}

Plugin::Plugin(Loader* ld, PluginsLoader* plugins, const QString& path, PluginSettings* set, const QString& uuid)
	:  m_d(ld, plugins, path, set, uuid){}

QString Plugin::name() const {
	return m_d.m_loader->name();
}

QString Plugin::version() const {
	return m_d.m_loader->version();
}

Plugin::Type Plugin::type() const {
	return m_d.m_loader->type();
}

PluginsLoader* Plugin::plugins() const {
	return m_d.m_plugins;
}

QString Plugin::uuid() const {
	return m_d.m_uuid;
}

QString Plugin::author() const {
	return m_d.m_loader->author();
}

QString Plugin::description() const {
	return m_d.m_loader->description();
}

QStringList Plugin::depends() const {
	return m_d.m_loader->depends();
}

QString Plugin::settingsPath() const {
	return m_d.m_settingsPath;
}

Loader* Plugin::loader() const {
	return m_d.m_loader;
}

bool Plugin::multipleInstances() const {
	return m_d.m_loader->multipleInstances();
}

//void Plugin::updateSettings(const PluginSettings& settings) {
//	*(m_d.m_settings) = settings;
//}

Extension::Extension(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path, PluginSettings* set, const QString& uuid)
	: QObject()//(parent)
	, Plugin(ld, plugins, path, set, uuid) {

	if (set != nullptr && set->objectName().isEmpty() == false) {
		setObjectName(set->objectName());
	}

	QObject::connect(this, &QObject::objectNameChanged, this, &Extension::nameChanged);
}

void Extension::nameChanged(const QString& name) {

	const auto set = settings<PluginSettings>();
	if (set != nullptr && set->objectName() != name) {
		set->setObjectName(name);
		//set->objectName() = name;
		//QSettings s = Settings::get();
		//PluginSettings ps;
		//ps.setObjectName(name);
		//ps.save(s, settingsPath());
		//set->save(s, settingsPath());
	}
}

Widget::Widget(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path, PluginSettings* set, const QString& uuid)
	: Plugin(ld, plugins, path, set, uuid)
	, MdiChild(parent) {
	
	if (set != nullptr && set->objectName().isEmpty() == false) {
		setObjectName(set->objectName());
	}

	QObject::connect(this, &QObject::objectNameChanged, this, &Widget::nameChanged);
}

void Widget::nameChanged(const QString& objName) {
	const auto set = settings<PluginSettings>();
	if (set != nullptr) {
		QRegExp rx("^" + name() + "([ ][0-9]+)?$");
		if (rx.exactMatch(objName)) {
			if (set->objectName() != objName) {
				set->setObjectName(objName);
			}
		} else {
			blockSignals(true);
			setObjectName("");
			blockSignals(false);
		}
	}
}

LoaderPrivate::LoaderPrivate(const QString& name, Plugin::Type type, const QString& version,
	const QString& author, const QString& description, const QStringList& depends, bool multiple, qint32 weight)
	: m_name(name)
	, m_type(type)
	, m_version(version)
	, m_author(author)
	, m_description(description)
	, m_depends(depends)
	, m_enabled(true)
	, m_multiple(multiple)
	, m_weight(weight){
}

Loader::Loader(const QString& name, Plugin::Type type, const QString& version,
	const QString& author, const QString& description, const QStringList& depends, bool multiple, qint32 weight)
	: m_d(name, type, version, author, description, depends, multiple, weight){
	}

QString Loader::name() const {	
	return m_d.m_name;
}

Plugin::Type Loader::type() const {
	return m_d.m_type;
}

QString Loader::version() const {
	return m_d.m_version;
}

QString Loader::author() const {
	return m_d.m_author;
}

QString Loader::description() const {
	return m_d.m_description;
}

QStringList Loader::depends() const {
	return m_d.m_depends;
}

bool Loader::enabled() const {
	return m_d.m_enabled;
}

void Loader::setEnabled(bool enabled) {
	m_d.m_enabled = enabled;
}

bool Loader::multipleInstances() const {
	return m_d.m_multiple;
}

qint32 Loader::weight() const {
	return m_d.m_weight;
}

ModuleLoaderContext::ModuleLoaderContext() {
}

ModuleLoaderContext::~ModuleLoaderContext() {
}
