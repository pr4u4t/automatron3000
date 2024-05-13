#include "CLin.h"
#include <QString>

bool CLin::m_driverOpen = false;


CLin::CLin(const SettingsDialog::LinSettings* settings)
	: m_xlPortHandle(XL_INVALID_PORTHANDLE)
	, m_xlChannelMask(0)
	, m_settings(settings)
	, m_data{0}
	, m_hMsgEvent(0) {
}

CLin::~CLin() {
	if (CLin::m_driverOpen == true) {
		xlCloseDriver();
		CLin::m_driverOpen = false;
	}
}

/*!
 fn XLstatus CLin::LINInit(int linID)

 initialize LIN driver with identifier \a linID 
*/
XLstatus CLin::LINOpen() {
	XLstatus ret = XL_ERROR;
	XLaccess xlChannelMask;
	XLaccess xlPermissionMask;

	if (LINFindDevice(m_settings->hwChannel) == false) {
		emit message(QString("CLin::LINOpen(): device %1 serial %2 not found").arg(m_settings->hwChannel.name).arg(m_settings->hwChannel.serialNumber), LoggerSeverity::LOG_ERROR);
		return ret;
	}

	if ((ret = LINGetChannelMask(m_settings->appName, m_settings->hwChannel, &xlChannelMask)) != XL_SUCCESS) {
		emit message("CLin::LINOpen(): failed to get channel mask", LoggerSeverity::LOG_ERROR);
		return ret;
	}

	xlPermissionMask = xlChannelMask;
	m_xlChannelMask = xlChannelMask;

	ret = xlOpenPort(
		&m_xlPortHandle, 
		m_settings->appName.toLocal8Bit().data(), 
		xlChannelMask, 
		&xlPermissionMask, 
		m_settings->queueSize, 
		XL_INTERFACE_VERSION, 
		XL_BUS_TYPE_LIN
	);

	if (m_xlPortHandle == XL_INVALID_PORTHANDLE) {
		emit message(QString("CLin::LINInit():xlOpenPort invalid handle"), LoggerSeverity::LOG_ERROR);
		return XL_ERROR;
	}
	if (ret != XL_SUCCESS) {
		emit message(QString("CLin::LINInit():xlOpenPort failed %1").arg(ret), LoggerSeverity::LOG_ERROR);
		m_xlPortHandle = XL_INVALID_PORTHANDLE;
		return ret;
	}

	emit message(QString("CLin::LINInit(): xlOpenPort PortHandle: %1; Permission_mask: 0x%2; Status: %3")
		.arg(m_xlPortHandle)
		.arg(QString::number(xlPermissionMask,16))
		.arg(ret)
	);

	linCreateRxThread();

	if (m_settings->mode != SettingsDialog::LinSettings::Mode::SLAVE) {
		if ((ret = linInitMaster()) != XL_SUCCESS) {
			emit message("CLin::LINInit(): init master failed", LoggerSeverity::LOG_ERROR);
			return ret;
		}

		emit message(QString("CLin::LINInit(): init Master: success"));
		
	} else {
		if ((ret = linInitSlave()) != XL_SUCCESS) {
			emit message("CLin::LINInit(): init slave failed");
			return ret;
		}

		emit message(QString("CLin::LINInit(): init Slave: success"));
	}

	return ret;
}

XLstatus CLin::linSetSlave(int linID, const unsigned char* data, size_t size) {
	XLstatus xlStatus = XL_ERROR;
	char tmp[100];
	unsigned char sdata[8];

	if (linID > 63) {
		emit message("CLin::linSetSlave(): ID invald max 63 allowed", LoggerSeverity::LOG_ERROR);
		return XL_ERROR;
	}

	memcpy(sdata, m_data, 8);
	memcpy(sdata, data, size);

	xlStatus = xlLinSetSlave(m_xlPortHandle, m_xlChannelMask, (unsigned char)linID, sdata, m_settings->dlc[linID], m_settings->checksumMethod);
	sprintf_s(tmp, sizeof(tmp), "Set Slave ID CM: '0x%I64x', status: %d", m_xlChannelMask, xlStatus);
	emit message(tmp);

	return xlStatus;
}

XLstatus CLin::LINSendMasterReq(unsigned int linID, const unsigned char* data, size_t size) {
	XLstatus ret = XL_ERROR;

	//ret = xlLinWakeUp(m_xlPortHandle, m_xlChannelMask[MASTER]);

	ret = linSetSlave(m_settings->slaveID, data, size);

	//send the master request
	if ((ret = xlLinSendRequest(m_xlPortHandle, m_xlChannelMask, (unsigned char)linID, 0)) != XL_SUCCESS) {
		emit message(QString("CLin::LINSendMasterReq(): failed for ID: %1").arg(linID));
	}
	
	return ret;
}

XLstatus CLin::LINSendMasterReq(unsigned int linID) {
	XLstatus ret = XL_ERROR;

	//ret = xlLinWakeUp(m_xlPortHandle, m_xlChannelMask[MASTER]);

	// send the master request
	if ((ret = xlLinSendRequest(m_xlPortHandle, m_xlChannelMask, (unsigned char)linID, 0)) != XL_SUCCESS) {
		emit message(QString("CLin::LINSendMasterReq(%1): failed").arg(linID));
	}

	return ret;
}

XLstatus CLin::LINClose() {
	XLstatus ret = XL_SUCCESS;

 
	if(m_xlChannelMask) {
		ret = xlDeactivateChannel(m_xlPortHandle, m_xlChannelMask);
		if (ret != XL_SUCCESS) {
			emit message(QString("CLin::LINClose(): xlDeactivateChannel failed: %1").arg(ret));
			return ret;
		}
		emit message("CLin::LINClose(): xlDeactivateChannel success");
	}

	if ((ret = xlClosePort(m_xlPortHandle)) != XL_SUCCESS) {
		emit message(QString("CLin::LINClose(): xlClosePort failed: %1").arg(ret));
		return ret;
	}

	emit message("CLin::LINClose(): xlClosePort success");
 
	m_xlPortHandle = XL_INVALID_PORTHANDLE;

	ret = xlCloseDriver();
  
	if (ret != XL_SUCCESS) {
		emit message(QString("CLin::LINClose(): xlCloseDriver: failed %1").arg(ret));
	}
	m_driverOpen = false;
	return ret;
}

XLstatus CLin::linInitMaster() {
	XLstatus ret = XL_ERROR;
	unsigned char DLC[64];

	// ---------------------------------------
	// Setup the channel as a MASTER
	// ---------------------------------------

	XLlinStatPar xlStatPar = {0};
	xlStatPar.LINMode = XL_LIN_MASTER;
	xlStatPar.baudrate = m_settings->baudrate;
	xlStatPar.LINVersion = m_settings->linVersion;

	ret = xlLinSetChannelParams(m_xlPortHandle, m_xlChannelMask, xlStatPar);

	emit message(QString("CLin::linInitMaster(): xlLinSetChannelParams init Master PH: '%1', CM: '%2', status: %3")
		.arg(m_xlPortHandle)
		.arg(m_xlChannelMask)
		.arg(ret)
	);
	
	if (ret != XL_SUCCESS) {
		emit message("CLin::linInitMaster(): failed to set channel params");
		return ret;
	}

	// ---------------------------------------
	// Setup the Master DLC's
	// ---------------------------------------

	// set the DLC for all ID's to DEFAULT_LIN_DLC
	for (int i = 0; i < 64; i++) {
		DLC[i] = m_settings->dlc[i];
	}

	ret = xlLinSetDLC(m_xlPortHandle, m_xlChannelMask, DLC);
	emit message(QString("CLin::linInitMaster(): xlLinSetDLC, CM: %1, status: %2")
		.arg(m_xlChannelMask)
		.arg(ret)
	);

	if (ret != XL_SUCCESS) {
		emit message("CLin::linInitMaster(): set dlc failed");
		return ret;
	}

	// ---------------------------------------
	// Setup the channel as a SLAVE also
	// ---------------------------------------

	if (m_settings->mode == SettingsDialog::LinSettings::Mode::MASTER_WITH_SLAVE) {
		memset(m_data, 0, 8);
		memcpy(m_data, m_settings->initialData.constData(), m_settings->initialData.size());


		ret = xlLinSetSlave(
			m_xlPortHandle,
			m_xlChannelMask,
			m_settings->slaveID,
			m_data,
			m_settings->slaveDLC,
			m_settings->checksumMethod
		);

		emit message(QString("CLin::linInitMaster(): xlLinSetSlave set slave id:%1, CM: %2, status: %3")
			.arg(m_settings->slaveID)
			.arg(m_xlChannelMask)
			.arg(ret)
		);

		ret = xlLinSwitchSlave(
			m_xlPortHandle,
			m_xlChannelMask,
			m_settings->slaveID,
			XL_LIN_SLAVE_ON
		);

		if (ret != XL_SUCCESS) {
			emit message("CLin::linInitMaster(): set slave failed");
			return ret;
		}
	} else {
		ret = xlLinSwitchSlave(
			m_xlPortHandle,
			m_xlChannelMask,
			m_settings->slaveID,
			XL_LIN_SLAVE_OFF
		);

		if (ret != XL_SUCCESS) {
			emit message("CLin::linInitMaster(): failed to switch off slave");
			return ret;
		}
	}

	// ---------------------------------------
	// Activate the Channel
	// ---------------------------------------

	ret = xlActivateChannel(m_xlPortHandle, m_xlChannelMask, XL_BUS_TYPE_LIN, XL_ACTIVATE_RESET_CLOCK);
	emit message(QString("CLin::linInitMaster(): activate channel, CM: %1, status: %2")
		.arg(m_xlChannelMask)
		.arg(ret)
	);
	
	if (ret != XL_SUCCESS) {
		emit message("CLin::linInitMaster(): activate channel failed");
		return ret;
	}

	ret = xlFlushReceiveQueue(m_xlPortHandle);
	emit message(QString("CLin::linInitMaster(): xlFlushReceiveQueue stat %1").arg(ret));

	return ret;
}

XLstatus CLin::linInitSlave() {
	XLstatus        xlStatus = XL_ERROR;
	char            tmp[100];
	unsigned char   data[8];
	unsigned char   DLC[64];

	// ---------------------------------------
	// Setup the channel as a SLAVE
	// ---------------------------------------

	XLlinStatPar     xlStatPar;

	xlStatPar.LINMode = XL_LIN_SLAVE;
	xlStatPar.baudrate = m_settings->baudrate;
	xlStatPar.LINVersion = m_settings->linVersion;		


	xlStatus = xlLinSetChannelParams(m_xlPortHandle, m_xlChannelMask, xlStatPar);

	sprintf_s(tmp, sizeof(tmp), "Init Slave PH: '%d', CM: '0x%I64x', status: %d\n", m_xlPortHandle, m_xlChannelMask, xlStatus);
	emit message(tmp);

	if (xlStatus != XL_SUCCESS) {
		emit message("CLin::linInitSlave(): failed to set channel params");
		return xlStatus;
	}

	// ---------------------------------------
	// Setup the Slave DLC's
	// ---------------------------------------

	// set the DLC for all ID's to DEFAULT_LIN_DLC
	for (int i = 0; i < 64; i++) {
		DLC[i] = m_settings->dlc[i];
	}

	xlStatus = xlLinSetDLC(m_xlPortHandle, m_xlChannelMask, DLC);
	sprintf_s(tmp, sizeof(tmp), "xlLinSetDLC, CM: '0x%I64x', status: %d", m_xlChannelMask, xlStatus);
	emit message(tmp);

	if (xlStatus != XL_SUCCESS) {
		emit message("CLin::linInitSlave(): failed to set dlc");
		return xlStatus;
	}

	// ---------------------------------------
	// Setup the SLAVE 
	// ---------------------------------------

	memset(data, 0, 8);
	memcpy(m_data, m_settings->initialData.constData(), m_settings->initialData.size());

	xlStatus = xlLinSetSlave(m_xlPortHandle, m_xlChannelMask, m_settings->slaveID, data, m_settings->slaveDLC, m_settings->checksumMethod);
	sprintf_s(tmp, sizeof(tmp), "Set Slave id:%d, CM: '0x%I64x', status: %d\n", m_settings->slaveID, m_xlChannelMask, xlStatus);
	emit message(tmp);

	if (xlStatus != XL_SUCCESS) {
		emit message("CLin::linInitSlave(): set slave failed");
		return xlStatus;
	}

	// ---------------------------------------
	// Activate the Slave Channel
	// ---------------------------------------

	xlStatus = xlActivateChannel(m_xlPortHandle, m_xlChannelMask, XL_BUS_TYPE_LIN, XL_ACTIVATE_RESET_CLOCK);
	sprintf_s(tmp, sizeof(tmp), "Activate Channel CM: '0x%I64x', status: %d\n", m_xlChannelMask, xlStatus);
	emit message(tmp);

	if (xlStatus != XL_SUCCESS) {
		emit message("");
		return xlStatus;
	}

	xlStatus = xlFlushReceiveQueue(m_xlPortHandle);
	sprintf_s(tmp, sizeof(tmp), "FlushReceiveQueue stat: %d\n", xlStatus);
	emit message(tmp);

	xlStatus = xlLinSwitchSlave(
		m_xlPortHandle,
		m_xlChannelMask,
		m_settings->slaveID,
		XL_LIN_SLAVE_ON
	);

	return xlStatus;
}

XLstatus CLin::linCreateRxThread() {
	HANDLE hnd = nullptr;
	XLstatus ret;

	if (m_xlPortHandle != XL_INVALID_PORTHANDLE) {

		emit message(QString("CLin::linCreateRxThread(): found OpenPort: %1").arg(m_xlPortHandle));

		// Send a event for each Msg!!!
		ret = xlSetNotification(m_xlPortHandle, &m_hMsgEvent, 1);
		emit message(QString("CLin::linCreateRxThread(): set notification %1, xlStatus: %2")
			.arg((long long)m_hMsgEvent).arg(ret));

		if (RegisterWaitForSingleObject(
			&hnd,
			m_hMsgEvent,
			RxThread,
			this,
			INFINITE,
			0
		) == false) {
			emit message("CLin::linCreateRxThread(): failed to setup rxthread");
		}

	}
	return ret;
}

void WINAPI RxThread(LPVOID ctx, BOOLEAN tow){
	CLin* clin = static_cast<CLin*>(ctx);
	XLstatus xlStatus = XL_SUCCESS;
	unsigned int msgsrx = 1;
	XLevent xlEvent;
	char tmp[100];

	ResetEvent(clin->m_hMsgEvent);

	while (!xlStatus) {
		msgsrx = 1;
		xlStatus = xlReceive(clin->m_xlPortHandle, &msgsrx, &xlEvent);

		if (xlStatus != XL_ERR_QUEUE_IS_EMPTY) {

			switch (xlEvent.tag) {

				// CAN events
				case XL_SYNC_PULSE:
					sprintf_s(tmp, sizeof(tmp), "SYNC_PULSE: on Ch: '%d'", xlEvent.chanIndex);
					clin->postMessage(tmp);
					//clin->postMessage(QString("RxThread; SYNC_PULSE: on Ch: '%1'").arg(xlEvent.chanIndex));
					break;

				case XL_TRANSCEIVER:
					sprintf_s(tmp, sizeof(tmp), "TRANSCEIVER: on Ch: '%d'", xlEvent.chanIndex);
					clin->postMessage(tmp);
					//clin->postMessage(QString("RxThread: TRANSCEIVER: on Ch: '%1'").arg(xlEvent.chanIndex));
					break;

				// LIN events
				case XL_LIN_NOANS:
					sprintf_s(tmp, sizeof(tmp), "LIN NOANS ID: 0x%x, Ch: '%d', time: %I64u", xlEvent.tagData.linMsgApi.linNoAns.id, xlEvent.chanIndex, xlEvent.timeStamp);
					clin->postMessage(tmp);
					clin->postData(tmp);
					/*
					clin->postMessage(QString("RxThread: LIN NOANS ID: '0x %1' on Ch: '%2', time: %3 64u")
						.arg(xlEvent.tagData.linMsgApi.linNoAns.id) 
						.arg(xlEvent.chanIndex) 
						.arg(xlEvent.timeStamp)
					);
					*/
					break;

					case XL_LIN_MSG: {
						QString str;
						QString         str1, sData;
						str = "RX: ";
						if (xlEvent.tagData.linMsgApi.linMsg.flags & XL_LIN_MSGFLAG_TX) str = "TX: ";

						str1 = "";
						for (int i = 0; i < xlEvent.tagData.linMsgApi.linMsg.dlc; i++) {
							str1 = QString::number(xlEvent.tagData.linMsgApi.linMsg.data[i], 16); //Format(_T("%02x"), xlEvent.tagData.linMsgApi.linMsg.data[i]);
							sData = sData + str1;
						}

						sprintf_s(tmp, sizeof(tmp), "ID: 0x%02x, dlc: '%d', Data: 0x%s, time: %I64u, Ch: '%d'", 
							xlEvent.tagData.linMsgApi.linMsg.id, 
							xlEvent.tagData.linMsgApi.linMsg.dlc, 
							sData.toLocal8Bit().data(), 
							xlEvent.timeStamp, 
							xlEvent.chanIndex
						);
						
						clin->postMessage(tmp);
						clin->postData(tmp);
						break;
					}

				case XL_LIN_SLEEP:
					sprintf_s(tmp, sizeof(tmp), "LIN SLEEP flag: 0x%x, time: %I64u, Ch: '%d'", xlEvent.tagData.linMsgApi.linSleep.flag, xlEvent.timeStamp, xlEvent.chanIndex);
					clin->postMessage(tmp);
					/*
					clin->postMessage(QString("RxThread: LIN SLEEP flag: 0x %1, time: %2 I64u, Ch: '%3'")
						.arg(xlEvent.tagData.linMsgApi.linSleep.flag) 
						.arg(xlEvent.timeStamp) 
						.arg(xlEvent.chanIndex)
					);
					*/
					break;

				case XL_LIN_ERRMSG:
					sprintf_s(tmp, sizeof(tmp), "LIN ERROR, Ch: '%d'", xlEvent.chanIndex);
					clin->postMessage(tmp);
					//clin->postMessage(QString("RxThread: LIN ERROR, Ch: '%1'").arg(xlEvent.chanIndex));
					break;

				case XL_LIN_SYNCERR:
					sprintf_s(tmp, sizeof(tmp), "LIN SYNCERR on Ch: '%d'", xlEvent.chanIndex);
					clin->postMessage(tmp);
					//clin->postMessage(QString("RxThread: LIN SYNCERR on Ch: '%1'").arg(xlEvent.chanIndex));
					break;

				case XL_LIN_WAKEUP:
					sprintf_s(tmp, sizeof(tmp), "LIN WAKEUP flags: 0x%x on Ch: '%d'", xlEvent.tagData.linMsgApi.linWakeUp.flag, xlEvent.chanIndex);
					clin->postMessage(tmp);
					break;
			}
				
			
		}
	}
}