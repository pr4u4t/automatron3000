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

	xlStatus = linInitMaster(linID);
	if (xlStatus) {
		m_pStatusBox->InsertString(-1, "Init Master failed!");
		return xlStatus;
	}
	sprintf_s(tmp, sizeof(tmp), "Init M/Slave id:%d", linID);
	m_pStatusBox->InsertString(-1, tmp);

	// for the next slave we take the next ID
	linID++;

	// if we have a second channel we setup a LIN slave
	if (m_xlChannelMask[SLAVE]) {
		xlStatus = linInitSlave(linID);
		if (xlStatus) {
			m_pStatusBox->InsertString(-1, "Init Slave failed!");
			return xlStatus;
		}
		sprintf_s(tmp, sizeof(tmp), "Init Slave id:%d", linID);
		m_pStatusBox->InsertString(-1, tmp);
	}

	return xlStatus;
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
		XLDEBUG(DEBUG_ADV, "Error in xlGetDriverConfig...");
		return xlStatus;
	}

	for (appChannel = 0; appChannel < 2; ++appChannel) {

		ret = xlGetApplConfig("xlLINExample", appChannel, &hwType, &hwIndex, &hwChannel, busType);

		if (xlStatus != XL_SUCCESS) {
			// Set the params into registry (default values...!)
			XLDEBUG(DEBUG_ADV, "set in registry");

			hwChannel = appChannel;

			for (i = 0; i < xlDrvConfig.channelCount; i++) {

				// check PC for hardware with LINCabs or LINPiggy's 
				if (xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_LIN) {
					hwType = xlDrvConfig.channel[i].hwType;
					sprintf_s(tmp, sizeof(tmp), "Found LIN hWType: %d;\n", hwType);
					XLDEBUG(DEBUG_ADV, tmp);

					xlStatus = xlSetApplConfig(                 // Registration of Application with default settings
						"xlLINExample", // Application Name
						appChannel,     // Application channel 0 or 1
						hwType,         // hwType  (CANcardXL...)    
						hwIndex,        // Index of hardware (slot) (0,1,...)
						hwChannel,      // Index of channel (connector) (0,1,...)
						busType);       // the application is for LIN.
				}
			}

		} else XLDEBUG(DEBUG_ADV, "found in registry");

		channelIndex = xlGetChannelIndex(hwType, hwIndex, hwChannel);

		// check if we have a valid LIN cab/piggy
		if (xlDrvConfig.channel[channelIndex].channelBusCapabilities & XL_BUS_ACTIVE_CAP_LIN) {
			XLDEBUG(DEBUG_ADV, "Found LIN cab/piggy\n");
			// and check the right hardwaretype
			if (xlDrvConfig.channel[channelIndex].hwType == hwType) {
				m_xlChannelMask[appChannel + 1] = xlGetChannelMask(hwType, hwIndex, hwChannel);
			}

		}
		else {
			XLDEBUG(DEBUG_ADV, "No LIN cab/piggy found\n");
		}

		sprintf_s(tmp, sizeof(tmp), "Init LIN hWType: %d; hWIndex: %d; hwChannel: %d; channelMask: 0x%I64x for appChannel: %d\n",
			hwType, hwIndex, hwChannel, m_xlChannelMask[appChannel + 1], appChannel);
		XLDEBUG(DEBUG_ADV, tmp);

	}

	return xlStatus;
}

XLstatus CLin::linInitMaster(int linID) {

}

XLstatus CLin::linInitSlave(int linID) {

}

XLstatus CLin::linCreateRxThread() {

}

XLstatus CLin::linSetSlave(int linID, BYTE data) {

}
