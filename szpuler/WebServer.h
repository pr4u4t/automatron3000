#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "../api/api.h"
#include "../core/core.h"
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaProperty>
#include <QJsonArray>
#include <QMetaType>

#include "Service.h"

struct WebServerData {
	WebServerData(MLoader* ld, QHttpServer* http, QTcpServer* tcp) 
		: m_ld(ld)
		, m_http(http)
		, m_tcp(tcp){
	}

	MLoader* m_ld;
	QHttpServer* m_http;
	QTcpServer* m_tcp;
};

class WebServer : public Service {

	Q_OBJECT

public:

	WebServer(MLoader *ld)
		: m_data(ld, new QHttpServer, new QTcpServer) {
		m_data.m_http->route("/", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest& req) {
			QJsonObject ret;
			const auto instances = this->m_data.m_ld->instances();
			for (auto i = instances.begin(), end = instances.end(); i != end; ++i) {
				auto qobject = i->dynamicCast<QObject>();

				if (i->data()->type() == Plugin::Type::WIDGET) {
					ret[qobject->objectName()] = pluginInfo(i->dynamicCast<Widget>());
				} else {
					ret[qobject->objectName()] = pluginInfo(i->dynamicCast<Extension>());
				}
				
			}
			
			return ret;
		});

		m_data.m_http->route("/<arg>/settings", QHttpServerRequest::Method::Get, [this](const QString& instanceName, const QHttpServerRequest& request) {
			QJsonObject ret;
			const auto instance = this->m_data.m_ld->findByObjectName(instanceName);
			if (instance.isNull()) {
				return QHttpServerResponse(QHttpServerResponse::StatusCode::NotFound);
			}
			
			return QHttpServerResponse(settingsDump(instance->settings<PluginSettings>()));
		});

		m_data.m_http->route("/<arg>/settings", QHttpServerRequest::Method::Post, [this](const QString& instanceName, const QHttpServerRequest& request) {
			QJsonObject ret;
			const auto instance = this->m_data.m_ld->findByObjectName(instanceName);
			if (instance.isNull()) {
				return QHttpServerResponse(QHttpServerResponse::StatusCode::NotFound);
			}
		
			return settingsStore(instance->settings<PluginSettings>(), request);
		});

		m_data.m_http->route("/<arg>/settings/hint", QHttpServerRequest::Method::Get, [this](const QString& instanceName, const QHttpServerRequest& request) {
			QJsonObject ret;
			const auto instance = this->m_data.m_ld->findByObjectName(instanceName);
			if (instance.isNull()) {
				return QHttpServerResponse(QHttpServerResponse::StatusCode::NotFound);
			}

			return QHttpServerResponse(settingsHint(instance->settings<PluginSettings>()));
		});
	}

	bool start() {
		if (m_data.m_tcp->listen(QHostAddress::Any, 1234) == false) {
			//TODO:
			emit message("WebServer::strart: failed to open listen socket");
			return false;
		}

		m_data.m_http->bind(m_data.m_tcp);
		return true;
	}

	void stop() {
		
	}

protected slots:

	/*
	QHttpServerResponse handleRoot(const QHttpServerRequest& request) {
	
		return QHttpServerResponse();
	}
	*/
protected:

	template<typename T>
	QJsonObject pluginInfo(const T& plugin) const {
		QJsonObject ret;
		ret["uuid"] = plugin->uuid();
		ret["settingsPath"] = plugin->settingsPath();
		ret["name"] = plugin->name();
		ret["objectName"] = plugin->objectName();
		ret["version"] = plugin->version();
		ret["author"] = plugin->author();
		ret["depends"] = QJsonArray::fromStringList(plugin->depends());
		ret["type"] = static_cast<qint32>(plugin->type());
		ret["multipleInstances"] = plugin->multipleInstances();

		return ret;
	}

	QJsonObject settingsDump(const PluginSettings* settings) const {
		QJsonObject ret;
		auto mo = settings->metaStack().last();
		
		for (int j = 0, count = mo->propertyCount(); j < count; ++j) {
			QVariant v = mo->property(j).readOnGadget(settings);
			switch (v.metaType().id()) {
			case QMetaType::Bool:
				ret[mo->property(j).name()] = v.toBool();
				break;
			case QMetaType::QByteArray:
				ret[mo->property(j).name()] = QString(v.toByteArray().toHex());
				break;
			case QMetaType::Double:
				ret[mo->property(j).name()] = v.toDouble();
				break;
			case QMetaType::Int:
				ret[mo->property(j).name()] = v.toInt();
				break;
			case QMetaType::UInt:
				ret[mo->property(j).name()] = v.toLongLong();
				break;
			case QMetaType::QString:
				ret[mo->property(j).name()] = v.toString();
				break;
			case QMetaType::QStringList:
				ret[mo->property(j).name()] = QJsonArray::fromStringList(v.toStringList());
				break;
			case QMetaType::QJsonArray:
				ret[mo->property(j).name()] = v.toJsonArray();
				break;
			case QMetaType::QJsonObject:
				ret[mo->property(j).name()] = v.toJsonObject();
				break;
			default:
				ret[mo->property(j).name()] = QString("Error: **** N/A **** : ")+v.metaType().name();
				emit message(QString("WebServer::settingsDump: unknown type ") + v.metaType().name());
			}
		}
		
		return ret;
	}

	QHttpServerResponse settingsStore(PluginSettings* settings, const QHttpServerRequest& request) const {
		const auto body = request.body();
		const auto json = QJsonDocument::fromJson(body);
		auto mo = settings->metaStack().last();

		if (json.isObject() == false) {
			return QHttpServerResponse::StatusCode::BadRequest;
		}

		auto o = json.object();

		for (int j = 0, count = mo->propertyCount(); j < count; ++j) {
			const char* const name = mo->property(j).name();
			
			if (strcmp(name, "objectName") == 0) {
				continue;
			}

			if (o.contains(name) == false) {
				continue;
			}

			QVariant v = o[name].toVariant();
			mo->property(j).writeOnGadget(settings, v);
		}

		return settingsDump(settings);
	}

	QJsonObject settingsHint(const PluginSettings* settings) const {
		QJsonObject ret;
		auto mo = settings->metaStack().last();

		for (int j = 0, count = mo->propertyCount(); j < count; ++j) {
			ret[mo->property(j).name()] = mo->property(j).typeName();
		}

		return ret;
	}

private:

	WebServerData m_data;
};

#endif
