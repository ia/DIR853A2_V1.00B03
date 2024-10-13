/****************************************************************************/
/*                                                                          */
/* RSS.H                                                                    */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines all APIs to for Rss APP.                            */
/*                                                                          */
/****************************************************************************/
#ifndef __RSSR_H__
#define __RSSR_H__

#define RssSetResultFileName  	"rssset_result.xml"
#define RssKeyUpdateFileName  	"rss_keyupdate.xml"
#define RssVarChkResultFileName "rssvar_checkresult.xml"
#define RssDbDumpFileName 		"rssdb_dump.xml"
#define RssGetResultFileName  	"rss.xml"
#define FwUpgdResultFileName 	"fwupgd_result.xml"

// use for Starter
#define EXTCALL_WanResync           0x0001
#define EXTCALL_DataValidate        0x0002    // use for Data Validate
#define EXTCALL_WlanResync          0x0004
#define EXTCALL_Wlan5gResync        0x0008
#define EXTCALL_DeviceResync        0x0010
#define EXTCALL_CfgSaveToFlash      0x0020
// only use for Repeater mode
#define EXTCALL_WlanSiteSurvey      0x0040
// use for Firmware Upgrader
#define EXTCALL_FwUpgrade           0x0080
#define EXTCALL_FwUpload            0x0400
// use for Easy Roaming
#define EXTCALL_UpdateStaRssi       0x0100
#define EXTCALL_DeauthSta           0x0200

// Max connected Slave Number
#define ConnSlave_MaxNo				32

typedef enum { 
	RssActionType_RssStartUp=0, 
	RssActionType_RssShutDown=1, 
	RssActionType_CloneToSlaves=2, 		// For Master Only
	RssActionType_SlaveRegister=3, 		// For Master Only
	RssActionType_SlaveUnRegister=4, 	// For Master Only
	RssActionType_ConnectMaster=5,		// For Slave Only
	RssActionType_DisconnectMaster=6,	// For Slave Only
	RssActionType_Unknow
} rssActionType_t;

typedef enum { 
    RssActionStat_Before=0, 
	RssActionStat_Done=1, 
	RssActionStat_Exception=2, 
	RssActionStat_Unknow
} rssActionStat_t;

typedef enum { 
	RssExceptionType_None=0, 
	RssExceptionType_CloneFail=1, 			// For Master Only
	RssExceptionType_RejectAction=2, 		// For Master Only
	RssExceptionType_ExpiredConnect=3, 		// For Master & Slave
	RssExceptionType_Unknow
} rssExceptionType_t;

#endif /* __RSSR_H__ */

