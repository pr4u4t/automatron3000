#ifndef CLIN_H
#define CLIN_H

#include "../api/api.h"
#include <Windows.h>
#include "../include/vxlapi.h"
#include "settingsdialog.h"

void WINAPI RxThread(LPVOID ctx, BOOLEAN tow);

class CLin : public QObject {
	Q_OBJECT

	friend void WINAPI RxThread(LPVOID ctx, BOOLEAN tow);

	//static constexpr const int MAXPORT = 8;
	//static constexpr const int RECEIVE_EVENT_SIZE = 1;			// DO NOT EDIT! Currently 1 is supported only
	//static constexpr const int MASTER = 1;						//!< channel is a master
	//static constexpr const int SLAVE = 2;						//!< channel is a slave
	//static constexpr const int DEFAULT_LIN_DLC = 8;				//!< default DLC for master/slave
	//static constexpr const int DEFAULT_LIN_BAUDRATE = 16500;	//!< default LIN baudrate
	//static constexpr const int DEFAULT_RX_QUEUE_SIZE = 256;

public:
	CLin(const LinSettings* settings);

	~CLin();

	static bool LINFindDevice(const ChannelConfig& channel) {
		XLstatus status = 0;
	
		QList<XLchannelConfig> channels = LINGetDevices(&status);
		if (status != XL_SUCCESS) {
			return false;
		}

		for (const XLchannelConfig& conf : channels) {
			if (conf == channel) {
				return true;
			}
		}

		return false;
	}

	static QList<XLchannelConfig> LINGetDevices(XLstatus* status = nullptr) {
		XLstatus err = XL_ERROR;
		XLdriverConfig xlDrvConfig;
		QList<XLchannelConfig> ret;
		
		if (m_driverOpen == false) {
			if ((err = xlOpenDriver()) != XL_SUCCESS) {
				//emit message("CLin::LINGetDevice(): xlOpenDriver failed " + QString::number(ret));
				goto FAIL;
			} else {
				m_driverOpen = true;
			}
		}
		
		if ((err = xlGetDriverConfig(&xlDrvConfig)) != XL_SUCCESS) {
			//emit message(QString("CLin::linGetChannelMask(): xlGetDriverConfig failed %1").arg(ret));
			goto FAIL;
		}

		for (int i = 0; i < xlDrvConfig.channelCount; i++) {
			if (xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_LIN) {
				ret << xlDrvConfig.channel[i];
			}
		}
		
		//ret = linGetChannelMask();

		// we need minimum one LIN channel for MASTER/SLAVE config
		//if (m_xlChannelMask[MASTER] == 0) {
		//	emit message("CLin::LINGetDevice(): failed no channel");
		//	return XL_ERROR;
		//}

		return ret;

	FAIL:
		if (status != nullptr) {
			*status = err;
		}

		return ret;
	}

	static XLstatus LINGetChannelMask(const QString& appName, const ChannelConfig& channel, XLaccess* mask) {
		XLstatus ret = XL_ERROR;
		XLdriverConfig xlDrvConfig;

		//check for hardware:
		ret = xlGetDriverConfig(&xlDrvConfig);
		if (ret != XL_SUCCESS) {
			//emit message(QString("CLin::linGetChannelMask(): xlGetDriverConfig failed %1").arg(ret));
			return ret;
		}

		for (int i = 0; i < xlDrvConfig.channelCount; i++) { 
			if (xlDrvConfig.channel[i] == channel) {
				*mask = xlDrvConfig.channel[i].channelMask;
				break;
			}
		}

		return ret;
	}

	static XLstatus LINSaveConfiuration(const LinSettings& settings) {
		return xlSetApplConfig(										// Registration of Application with default settings
			settings.appName().toLocal8Bit().data(),	// Application Name
			settings.hwChannel().appChannel,			// Application channel 0 or 1
			settings.hwChannel().hwType,				// hwType  (CANcardXL...)
			settings.hwChannel().hwIndex,				// Index of hardware (slot) (0,1,...)
			settings.hwChannel().hwChannel,			// Index of channel (connector) (0,1,...)
			settings.hwChannel().busType				// the application is for LIN.
		);
	}

	XLstatus LINOpen();
	XLstatus LINSendMasterReq(unsigned int linID, const unsigned char* data, size_t size);
	XLstatus LINSendMasterReq(unsigned int linID);
	XLstatus LINClose();

signals:

	void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

	void dataReady(const QByteArray& data) const;

	void error(const QByteArray& data);

protected:

	void postMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) {
		emit message(msg, severity);
	}

	void postData(const QByteArray& data) {
		emit dataReady(data);
	}

	void postError(const QByteArray& data) {
		emit error(data);
	}

private:
	XLstatus linInitMaster();
	XLstatus linInitSlave();
	XLstatus linCreateRxThread();
	XLstatus linSetSlave(int linID, const unsigned char* data, size_t size);

	XLaccess m_xlChannelMask = {0};
	XLportHandle m_xlPortHandle = {0};
	XLhandle m_hMsgEvent;
	BYTE m_data[8];
	static bool m_driverOpen;
	const LinSettings* m_settings = nullptr;
};

#endif
