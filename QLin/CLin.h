#ifndef CLIN_H
#define CLIN_H

#include "../api/api.h"
#include <Windows.h>
#include "../include/vxlapi.h"

void WINAPI RxThread(LPVOID ctx, BOOLEAN tow);

class CLin : public QObject {
	Q_OBJECT

	friend void WINAPI RxThread(LPVOID ctx, BOOLEAN tow);

	static constexpr const int MAXPORT = 8;
	static constexpr const int RECEIVE_EVENT_SIZE = 1;			// DO NOT EDIT! Currently 1 is supported only
	static constexpr const int MASTER = 1;						//!< channel is a master
	static constexpr const int SLAVE = 2;						//!< channel is a slave
	static constexpr const int DEFAULT_LIN_DLC = 8;				//!< default DLC for master/slave
	static constexpr const int DEFAULT_LIN_BAUDRATE = 16500;	//!< default LIN baudrate
	static constexpr const int DEFAULT_RX_QUEUE_SIZE = 256;

public:
	CLin(const QString& name, qint32 rxQueueSize = DEFAULT_RX_QUEUE_SIZE);
	~CLin() = default;

	XLstatus LINGetDevice();
	XLstatus LINInit(int masterID, unsigned int linVersion, int baudrate, int mos, int slaveID, const unsigned char* data = nullptr, size_t dsize = 0);
	XLstatus LINSendMasterReq(unsigned int linID, const unsigned char* data, size_t size);
	XLstatus LINSendMasterReq(unsigned int linID);
	XLstatus LINClose();

signals:

	void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

	void dataReady(const QByteArray& data) const;

protected:

	void postMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) {
		emit message(msg, severity);
	}

	void postData(const QByteArray& data) {
		emit dataReady(data);
	}

private:
	XLstatus linGetChannelMask();
	XLstatus linInitMaster(int linID, unsigned int linVersion, int baudrate, int slaveID = -1, const unsigned char* data = nullptr, size_t dsize = 0);
	XLstatus linInitSlave(int linID, unsigned int linVersion);
	XLstatus linCreateRxThread();
	XLstatus linSetSlave(int linID, const unsigned char* data, size_t size);

	XLaccess m_xlChannelMask[MAXPORT] = {0};
	XLportHandle m_xlPortHandle = {0};
	QString m_name;
	qint32 m_rxQueueSize;
	XLhandle m_hMsgEvent;
	BYTE m_masterID;
	BYTE m_slaveID;
	BYTE m_data[8];
};

#endif
