#include "plugin.h"

QString Plugin::name() const {
	return m_loader->name();
}

QString Plugin::version() const {
	return m_loader->version();
}

Plugin::Type Plugin::type() const {
	return m_loader->type();
}

PluginsLoader* Plugin::plugins() const {
	return m_plugins;
}

QString Plugin::uuid() const {
	return m_uuid;
}

//QWidget* Plugin::parentWidget() const {
//	return m_parentWidget;
//}

QString Plugin::author() const {
	return m_loader->author();
}

QString Plugin::description() const {
	return m_loader->description();
}