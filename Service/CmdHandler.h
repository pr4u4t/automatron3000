#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <QString>
#include <QHash>
#include <QStringList>

class CmdHandler {

public:

	using CmdCallback = std::function<qint64(const QStringList& argv)>;

	CmdHandler();

	qint64 exec(const QString& cmd);

	qint64 exec(const QStringList& cmd);

	static bool registerCmd(const QString& name, const CmdCallback& cback) {
		if (m_cmd.contains(name)) {
			return false;
		}

		m_cmd[name] = cback;
		return true;
	}

protected:

	static qint64 start(const QStringList& args);

	static qint64 stop(const QStringList& args);

	static qint64 restart(const QStringList& args);

	static qint64 install(const QStringList& args);

	static qint64 uninstall(const QStringList& args);

	static qint64 help(const QStringList& args);

private:

	static QHash<QString, CmdCallback> m_cmd;

};

#endif
