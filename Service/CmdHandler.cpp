#include "CmdHandler.h"
#include "ServiceManager.h"
#include <QDebug>

QHash<QString, CmdHandler::CmdCallback> CmdHandler::m_cmd = QHash<QString, CmdHandler::CmdCallback>();

CmdHandler::CmdHandler() {
	registerCmd("start", &start);
	registerCmd("stop", &stop);
	registerCmd("restart", &restart);
	registerCmd("install", &install);
	registerCmd("uninstall", &uninstall);
	registerCmd("help", &help);
}

qint64 CmdHandler::exec(const QString& cmd) {
	qDebug() << "CmdHandler::exec(const QString& cmd)";
	
	if (cmd.isEmpty()) {
		qCritical() << "Command string empty";
		return 1;
	}

	QStringList args = cmd.split(" ");
	if (m_cmd.contains(args[1]) == false) {
		qCritical() << "Command not found";
		return 2;
	}

	qDebug() << "Executing command: " << cmd;

	return m_cmd[args[1]](args);
}

qint64 CmdHandler::exec(const QStringList& args) {
	qDebug() << "CmdHandler::exec(const QStringList& args)";

	if (args.isEmpty()) {
		qCritical() << "Command string empty";
		return 1;
	}

	if (m_cmd.contains(args[1]) == false) {
		qCritical() << "Command not found";
		return 2;
	}

	return m_cmd[args[1]](args);
}

qint64 CmdHandler::start(const QStringList& args) {
	qDebug() << "CmdHandler::start(const QStringList& args)";
	if (ServiceManager::instance()->exists()) {
		qCritical() << "Service does not exist";
		return 1;
	}
	return ServiceManager::instance()->start() == true ? 0 : 2;
}

qint64 CmdHandler::stop(const QStringList& args) {
	qDebug() << "CmdHandler::stop(const QStringList& args)";
	if (ServiceManager::instance()->exists()) {
		qCritical() << "Service does not exist";
		return 1;
	}
	return ServiceManager::instance()->stop() == true ? 0 : 2;
}

qint64 CmdHandler::restart(const QStringList& args) {
	qDebug() << "CmdHandler::restart(const QStringList& args)";
	if (ServiceManager::instance()->exists()) {
		qCritical() << "Service does not exist";
		return 1;
	}
	stop(args);
	start(args);
	return 0;
}

qint64 CmdHandler::install(const QStringList& args) {
	qDebug() << "CmdHandler::install(const QStringList& args)";
	if (ServiceManager::instance()->exists()) {
		qCritical() << "Service already exists";
		return 1;
	}

	return ServiceManager::instance()->create() == true ? 0 : 2;
}

qint64 CmdHandler::uninstall(const QStringList& args) {
	qDebug() << "CmdHandler::install(const QStringList& args)";
	if (ServiceManager::instance()->exists() == false) {
		qCritical() << "Service does not exist";
		return 1;
	}

	return ServiceManager::instance()->remove() == true ? 0 : 2;
}

qint64 CmdHandler::help(const QStringList& args){
	qDebug() << "CmdHandler::help(const QStringList& args)";

	for (auto i = m_cmd.cbegin(), end = m_cmd.cend(); i != end; ++i) {
		qInfo() << qPrintable(i.key());
	}

	return 0;
}