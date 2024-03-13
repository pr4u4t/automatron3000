#ifndef CLIN_H
#define CLIN_H

#include "../api/api.h"
#include <Windows.h>
#include "../include/vxlapi.h"

class CLin : public QObject {
	Q_OBJECT

	static constexpr const int MAXPORT = 8;
	static constexpr const int RECEIVE_EVENT_SIZE = 1;     // DO NOT EDIT! Currently 1 is supported only
	static constexpr const int MASTER = 1;     //!< channel is a master
	static constexpr const int SLAVE = 2;     //!< channel is a slave
	static constexpr const int DEFAULT_LIN_DLC = 8;     //!< default DLC for master/slave
	static constexpr const int DEFAULT_LIN_BAUDRATE = 16500;     //!< default LIN baudrate
	static constexpr const int DEFAULT_RX_QUEUE_SIZE = 256;

public:
	CLin(const QString& name, qint32 rxQueueSize = DEFAULT_RX_QUEUE_SIZE);
	~CLin() = default;

	XLstatus LINGetDevice();
	XLstatus LINInit(int linID);
	XLstatus LINSendMasterReq(BYTE data, int linID);
	XLstatus LINClose();

signals:

	void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

private:
	XLstatus linGetChannelMask();
	XLstatus linInitMaster(int linID);
	XLstatus linInitSlave(int linID);
	XLstatus linCreateRxThread();
	XLstatus linSetSlave(int linID, BYTE data);

	XLaccess m_xlChannelMask[MAXPORT] = {0};
	XLportHandle m_xlPortHandle = {0};
	QString m_name;
	qint32 m_rxQueueSize;
};









#endif
