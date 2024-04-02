#include "CLin.h"
#include <QString>

CLin::CLin(const QString& name, qint32 rxQueueSize)
	: m_xlPortHandle(XL_INVALID_PORTHANDLE)
	, m_xlChannelMask{ 0,0 }
	, m_name(name)
	, m_rxQueueSize(rxQueueSize){
}

XLstatus CLin::LINGetDevice() {
	XLstatus ret = XL_ERROR;

	if ((ret = xlOpenDriver()) != XL_SUCCESS) {
		emit message("CLin::LINGetDevice(): xlOpenDriver failed " + QString::number(ret));
		return ret;
	} else {
		emit message("CLin::LINGetDevice(): success");
	}
	
	ret = linGetChannelMask();

	// we need minimum one LIN channel for MASTER/SLAVE config
	if (m_xlChannelMask[MASTER] == 0) {
		emit message("CLin::LINGetDevice(): failed no channel");
		return XL_ERROR;
	}

	return ret;
}

/*!
 fn XLstatus CLin::LINInit(int linID)

 initialize LIN driver with identifier \a linID 
*/
XLstatus CLin::LINInit(int masterID, unsigned int linVersion, int baudrate, int mos, int slaveID, const unsigned char* data, size_t dsize) {
	XLstatus ret = XL_ERROR;
	XLaccess xlChannelMask_both;
	XLaccess xlPermissionMask;

	m_masterID = masterID;
	m_slaveID = slaveID;

	// ---------------------------------------
	// Open ONE port for both channels master+slave
	// ---------------------------------------

	// calculate the channelMask for both channel 
	xlChannelMask_both = m_xlChannelMask[MASTER] | m_xlChannelMask[SLAVE];
	xlPermissionMask = xlChannelMask_both;

	ret = xlOpenPort(
		&m_xlPortHandle, 
		m_name.toLocal8Bit().data(), 
		xlChannelMask_both, 
		&xlPermissionMask, 
		256, 
		XL_INTERFACE_VERSION, 
		XL_BUS_TYPE_LIN
	);

	if (m_xlPortHandle == XL_INVALID_PORTHANDLE) {
		emit message(QString("CLin::LINInit():xlOpenPort invalid handle"));
		return XL_ERROR;
	}
	if (ret != XL_SUCCESS) {
		emit message(QString("CLin::LINInit():xlOpenPort failed %1").arg(ret));
		m_xlPortHandle = XL_INVALID_PORTHANDLE;
		return ret;
	}

	emit message(QString("CLin::LINInit(): xlOpenPort PortHandle: %1; Permission_mask: 0x%2; Status: %3")
		.arg(m_xlPortHandle)
		.arg(QString::number(xlPermissionMask,16))
		.arg(ret)
	);

	// ---------------------------------------
	// Create ONE thread for both channels
	// ---------------------------------------
	//INSTEAD OF THREAD REGISTER HANDLER
	linCreateRxThread();

	// ---------------------------------------
	// Init each channel (master+slave)
	// ---------------------------------------

	ret = linInitMaster(masterID, linVersion, baudrate, slaveID, data, dsize);
	if (ret != XL_SUCCESS) {
		emit message("CLin::LINInit(): init master failed");
		return ret;
	}

	emit message(QString("CLin::LINInit(): init Master %1 Slave id:%2").arg(masterID).arg(slaveID));

	// for the next slave we take the next ID
	//linID++;

	// if we have a second channel we setup a LIN slave
	//if (m_xlChannelMask[SLAVE]) {
	//	ret = linInitSlave(linID, linVersion);
	//	if (ret != XL_SUCCESS) {
	//		emit message("CLin::LINInit(): init slave failed");
	//		return ret;
	//	}
	//	emit message(QString("CLin::LINInit(): init slave id:%1").arg(linID));
	//}

	return ret;
}

XLstatus CLin::LINSendMasterReq(unsigned int linID, const unsigned char* data, size_t size) {
	XLstatus ret = XL_ERROR;

	//ret = xlLinWakeUp(m_xlPortHandle, m_xlChannelMask[MASTER]);

	// setup the only slave channel (LIN ID + 1)
	ret = linSetSlave(m_slaveID, data, size);

	// send the master request
	if ((ret = xlLinSendRequest(m_xlPortHandle, m_xlChannelMask[MASTER], (unsigned char)linID, 0)) != XL_SUCCESS) {
		emit message(QString("CLin::LINSendMasterReq(): failed for ID: %1").arg(linID));
	}
	
	return ret;
}

XLstatus CLin::LINSendMasterReq(unsigned int linID) {
	XLstatus ret = XL_ERROR;

	//ret = xlLinWakeUp(m_xlPortHandle, m_xlChannelMask[MASTER]);

	// send the master request
	if ((ret = xlLinSendRequest(m_xlPortHandle, m_xlChannelMask[MASTER], (unsigned char)linID, 0)) != XL_SUCCESS) {
		emit message(QString("CLin::LINSendMasterReq(%1): failed").arg(linID));
	}

	return ret;
}

XLstatus CLin::LINClose() {
	XLstatus ret = XL_SUCCESS;
	XLaccess xlChannelMask_both = m_xlChannelMask[MASTER] | m_xlChannelMask[SLAVE];

	if(m_xlPortHandle == XL_INVALID_PORTHANDLE) {
		emit message("CLin::LINClose(): no port open");
		return ret;
	}
 
	if(xlChannelMask_both) {
		ret = xlDeactivateChannel(m_xlPortHandle, xlChannelMask_both);
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

	return ret;
}

XLstatus CLin::linGetChannelMask() {
	XLstatus ret = XL_ERROR;
	XLdriverConfig xlDrvConfig;

	// default values
	unsigned int    hwType;
	unsigned int    hwIndex;
	unsigned int    hwChannel;
	unsigned int    appChannel;
	int             channelIndex;
	unsigned int    busType = XL_BUS_TYPE_LIN;
	unsigned int    i;

	//check for hardware:
	ret = xlGetDriverConfig(&xlDrvConfig);
	if (ret != XL_SUCCESS) {
		emit message(QString("CLin::linGetChannelMask(): xlGetDriverConfig failed %1").arg(ret));
		return ret;
	}

	for (appChannel = 0; appChannel < 2; ++appChannel) {

		ret = xlGetApplConfig(m_name.toLocal8Bit().data(), appChannel, &hwType, &hwIndex, &hwChannel, busType);

		if (ret != XL_SUCCESS) {
			// Set the params into registry (default values...!)
			hwChannel = appChannel;

			for (i = 0; i < xlDrvConfig.channelCount; i++) {
				// check PC for hardware with LINCabs or LINPiggy's 
				if (xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_LIN) {
					hwType = xlDrvConfig.channel[i].hwType;
					emit message(QString("CLin::linGetChannelMask(): xlGetApplConfig: found LIN hWType: %1").arg(hwType));
					
					ret = xlSetApplConfig(				// Registration of Application with default settings
						m_name.toLocal8Bit().data(),	// Application Name
						appChannel,						// Application channel 0 or 1
						hwType,							// hwType  (CANcardXL...)    
						hwIndex,						// Index of hardware (slot) (0,1,...)
						hwChannel,						// Index of channel (connector) (0,1,...)
						busType							// the application is for LIN.
					);
				}
			}
		} else {
			emit message("CLin::linGetChannelMask(): xlGetApplConfig: found in registry");
		}

		channelIndex = xlGetChannelIndex(hwType, hwIndex, hwChannel);

		// check if we have a valid LIN cab/piggy
		if (xlDrvConfig.channel[channelIndex].channelBusCapabilities & XL_BUS_ACTIVE_CAP_LIN) {
			emit message("CLin::linGetChannelMask(): found LIN cab/piggy");
			// and check the right hardwaretype
			if (xlDrvConfig.channel[channelIndex].hwType == hwType) {
				m_xlChannelMask[appChannel + 1] = xlGetChannelMask(hwType, hwIndex, hwChannel);
			}

		} else {
			emit message("CLin::linGetChannelMask(): No LIN cab/piggy found");
		}

		emit message(QString("CLin::linGetChannelMask(): init LIN hWType: %1; hWIndex: %2; hwChannel: %3; channelMask: %4 for appChannel: %5")
			.arg(hwType)
			.arg(hwIndex)
			.arg(hwChannel) 
			.arg(m_xlChannelMask[appChannel + 1]) 
			.arg(appChannel)
		);

	}

	return ret;
}

XLstatus CLin::linInitMaster(int linID, unsigned int linVersion, int baudrate, int slaveID, const unsigned char* data, size_t dsize) {
	XLstatus ret = XL_ERROR;
	unsigned char DLC[64];

	// ---------------------------------------
	// Setup the channel as a MASTER
	// ---------------------------------------

	XLlinStatPar     xlStatPar = {0};

	xlStatPar.LINMode = XL_LIN_MASTER;
	xlStatPar.baudrate = baudrate;	// set the baudrate
	xlStatPar.LINVersion = linVersion;

	ret = xlLinSetChannelParams(m_xlPortHandle, m_xlChannelMask[MASTER], xlStatPar);

	emit message(QString("CLin::linInitMaster(): xlLinSetChannelParams init Master PH: '%1', CM: '%2', status: %3")
		.arg(m_xlPortHandle)
		.arg(m_xlChannelMask[MASTER])
		.arg(ret)
	);
	
	// ---------------------------------------
	// Setup the Master DLC's
	// ---------------------------------------

	// set the DLC for all ID's to DEFAULT_LIN_DLC
	for (int i = 0; i < 64; i++) {
		DLC[i] = DEFAULT_LIN_DLC;
	}

	ret = xlLinSetDLC(m_xlPortHandle, m_xlChannelMask[MASTER], DLC);
	emit message(QString("CLin::linInitMaster(): xlLinSetDLC, CM: %1, status: %2")
		.arg(m_xlChannelMask[MASTER])
		.arg(ret));

	// ---------------------------------------
	// Setup the channel as a SLAVE also
	// ---------------------------------------

	memset(m_data, 0, 8);
	if (data != nullptr && dsize > 0) {
		memcpy(m_data, data, dsize);
	}
	unsigned char tmp = (unsigned char)(slaveID == -1 ? linID : slaveID);

	ret = xlLinSetSlave(
		m_xlPortHandle, 
		m_xlChannelMask[MASTER], 
		tmp, 
		m_data, 
		DEFAULT_LIN_DLC, 
		XL_LIN_CALC_CHECKSUM
	);

	emit message(QString("CLin::linInitMaster(): xlLinSetSlave set slave id:%1, CM: %2, status: %3")
		.arg(linID)
		.arg(m_xlChannelMask[MASTER])
		.arg(ret)
	);

	// ---------------------------------------
	// Activate the Master Channel
	// ---------------------------------------

	ret = xlActivateChannel(m_xlPortHandle, m_xlChannelMask[MASTER], XL_BUS_TYPE_LIN, XL_ACTIVATE_RESET_CLOCK);
	emit message(QString("CLin::linInitMaster(): activate channel, CM: %1, status: %2")
		.arg(m_xlChannelMask[MASTER])
		.arg(ret)
	);
	
	if (ret != XL_SUCCESS) {
		return ret;
	}

	ret = xlFlushReceiveQueue(m_xlPortHandle);
	emit message(QString("CLin::linInitMaster(): xlFlushReceiveQueue stat %1").arg(ret));

	return ret;
}

XLstatus CLin::linInitSlave(int linID, unsigned int linVersion) {
	XLstatus        xlStatus = XL_ERROR;
	char            tmp[100];

	unsigned char   data[8];
	unsigned char   DLC[64];

	// ---------------------------------------
	// Setup the channel as a SLAVE
	// ---------------------------------------

	XLlinStatPar     xlStatPar;

	xlStatPar.LINMode = XL_LIN_SLAVE;
	xlStatPar.baudrate = DEFAULT_LIN_BAUDRATE; // set the baudrate
	xlStatPar.LINVersion = linVersion;   // use LIN 1.3

	xlStatus = xlLinSetChannelParams(m_xlPortHandle, m_xlChannelMask[SLAVE], xlStatPar);

	sprintf_s(tmp, sizeof(tmp), "Init Slave PH: '%d', CM: '0x%I64x', status: %d\n", m_xlPortHandle, m_xlChannelMask[SLAVE], xlStatus);
	emit message(tmp);

	// ---------------------------------------
	// Setup the Slave DLC's
	// ---------------------------------------

	// set the DLC for all ID's to DEFAULT_LIN_DLC
	for (int i = 0; i < 64; i++) {
		DLC[i] = DEFAULT_LIN_DLC;
	}

	xlStatus = xlLinSetDLC(m_xlPortHandle, m_xlChannelMask[SLAVE], DLC);
	sprintf_s(tmp, sizeof(tmp), "xlLinSetDLC, CM: '0x%I64x', status: %d", m_xlChannelMask[SLAVE], xlStatus);
	emit message(tmp);

	// ---------------------------------------
	// Setup the SLAVE 
	// ---------------------------------------

	memset(data, 0, 8);

	xlStatus = xlLinSetSlave(m_xlPortHandle, m_xlChannelMask[SLAVE], (unsigned char)linID, data, DEFAULT_LIN_DLC, XL_LIN_CALC_CHECKSUM);
	sprintf_s(tmp, sizeof(tmp), "Set Slave id:%d, CM: '0x%I64x', status: %d\n", linID, m_xlChannelMask[SLAVE], xlStatus);
	emit message(tmp);

	// ---------------------------------------
	// Activate the Slave Channel
	// ---------------------------------------

	xlStatus = xlActivateChannel(m_xlPortHandle, m_xlChannelMask[SLAVE], XL_BUS_TYPE_LIN, XL_ACTIVATE_RESET_CLOCK);
	sprintf_s(tmp, sizeof(tmp), "Activate Channel CM: '0x%I64x', status: %d\n", m_xlChannelMask[SLAVE], xlStatus);
	emit message(tmp);

	if (xlStatus != XL_SUCCESS) {
		return xlStatus;
	}

	xlStatus = xlFlushReceiveQueue(m_xlPortHandle);
	sprintf_s(tmp, sizeof(tmp), "FlushReceiveQueue stat: %d\n", xlStatus);
	emit message(tmp);

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
	WAITORTIMERCALLBACKFUNC o;
}

XLstatus CLin::linSetSlave(int linID, const unsigned char* data, size_t size) {
	XLstatus xlStatus = XL_ERROR;
	char tmp[100];
	unsigned char sdata[8];

	memcpy(sdata, m_data, 8);
	memcpy(sdata, data, size);
	

	xlStatus = xlLinSetSlave(m_xlPortHandle, m_xlChannelMask[MASTER], (unsigned char)linID, sdata, DEFAULT_LIN_DLC, XL_LIN_CALC_CHECKSUM);
	sprintf_s(tmp, sizeof(tmp), "Set Slave ID CM: '0x%I64x', status: %d", m_xlChannelMask[SLAVE], xlStatus);
	emit message(tmp);

	return xlStatus;
}

void WINAPI RxThread(LPVOID ctx, BOOLEAN tow){
	CLin* clin = static_cast<CLin*>(ctx);
	XLstatus xlStatus = XL_SUCCESS;
	unsigned int msgsrx = CLin::RECEIVE_EVENT_SIZE;
	XLevent xlEvent;
	char tmp[100];

	ResetEvent(clin->m_hMsgEvent);

	while (!xlStatus) {
		msgsrx = CLin::RECEIVE_EVENT_SIZE;
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