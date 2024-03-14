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
XLstatus CLin::LINInit(int linID) {
	XLstatus ret = XL_ERROR;
	XLaccess xlChannelMask_both;
	XLaccess xlPermissionMask;

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
		emit message(QString("CLin::LINInit(%1):xlOpenPort invaid handle").arg(linID));
		return XL_ERROR;
	}
	if (ret != XL_SUCCESS) {
		emit message(QString("CLin::LINInit(%1):xlOpenPort failed %2").arg(linID).arg(ret));
		m_xlPortHandle = XL_INVALID_PORTHANDLE;
		return ret;
	}

	emit message(QString("CLin::LINInit(%1): xlOpenPort PortHandle: %2; Permission_mask: 0x%3; Status: %4")
		.arg(linID)
		.arg(m_xlPortHandle)
		.arg(QString::number(xlPermissionMask,16))
		.arg(ret)
	);

	// ---------------------------------------
	// Create ONE thread for both channels
	// ---------------------------------------
	//INSTEAD OF THREAD REGISTER HANDLER
	//linCreateRxThread();

	// ---------------------------------------
	// Init each channel (master+slave)
	// ---------------------------------------

	ret = linInitMaster(linID);
	if (ret != XL_SUCCESS) {
		emit message("CLin::LINInit(): init master failed");
		return ret;
	}

	emit message(QString("CLin::LINInit(): init M/Slave id:%1").arg(linID));

	// for the next slave we take the next ID
	linID++;

	// if we have a second channel we setup a LIN slave
	if (m_xlChannelMask[SLAVE]) {
		ret = linInitSlave(linID);
		if (ret != XL_SUCCESS) {
			emit message("CLin::LINInit(): init slave failed");
			return ret;
		}
		emit message(QString("CLin::LINInit(): init slave id:%1").arg(linID));
	}

	return ret;
}

XLstatus CLin::LINSendMasterReq(BYTE data, int linID) {
	XLstatus ret = XL_ERROR;

	// send the master request
	if ((ret = xlLinSendRequest(m_xlPortHandle, m_xlChannelMask[MASTER], (unsigned char)linID, 0)) != XL_SUCCESS) {
		emit message(QString("CLin::LINSendMasterReq(%1, %2)").arg(data).arg(linID));
	}
	// setup the only slave channel (LIN ID + 1)
	//xlStatus = linSetSlave(linID + 1, data);
	return ret;
}

XLstatus CLin::LINClose() {
	XLstatus ret = XL_SUCCESS;
	XLaccess xlChannelMask_both = m_xlChannelMask[MASTER] | m_xlChannelMask[SLAVE];
 
	/* DONT NEEDED */
	//g_bThreadRun = FALSE;

	// Wait until the thread is done...
	//Sleep(100);
	/*-----------------*/

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

XLstatus CLin::linInitMaster(int linID) {
	XLstatus ret = XL_ERROR;

	unsigned char   data[8];
	unsigned char   DLC[64];

	// ---------------------------------------
	// Setup the channel as a MASTER
	// ---------------------------------------

	XLlinStatPar     xlStatPar;

	xlStatPar.LINMode = XL_LIN_MASTER;
	xlStatPar.baudrate = DEFAULT_LIN_BAUDRATE;	// set the baudrate
	xlStatPar.LINVersion = XL_LIN_VERSION_1_3;  // use LIN 1.3

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

	memset(data, 0, 8);

	ret = xlLinSetSlave(m_xlPortHandle, m_xlChannelMask[MASTER], (unsigned char)linID, data, DEFAULT_LIN_DLC, XL_LIN_CALC_CHECKSUM);
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

XLstatus CLin::linInitSlave(int linID) {
	return XL_SUCCESS;
}

XLstatus CLin::linCreateRxThread() {
	return XL_SUCCESS;
}

XLstatus CLin::linSetSlave(int linID, BYTE data) {
	return XL_SUCCESS;
}
