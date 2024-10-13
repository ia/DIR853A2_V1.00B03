/****************************************************************************/
/*                                                                          */
/* XML_PARSER.H                                                           */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines all APIs to receive the RssXML uploaded by Rss APP.  */
/*                                                                          */
/****************************************************************************/
#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

#include "debug_log.h"

#define STARTER_LIB_VER		"1.4" SVN_REVISION
#define RSS_SPEC_VER		"1.1"

#define _OT_Root 			"dbros"		// root node name of xml data
#define _OT_Path 			"/onetouch"	// leading '/'
#define _OT_ExeName			"onetouch"	
#define _OT_Name			"onetouch"	// remove space
#define _OT_Title			"One-Touch"	// change space as '-'
#define _OT_AppTitle		"One Touch"	
#define _OT_Starter 		"Starter"
#define _OT_StarterKey 		"StarterKey"
#define _OT_DKey 			"DKey"
#define _OT_LogHeader		"OT"

#define _OT_UpnpDef_Domain		"d-link-com"
#define _OT_UpnpDef_Starter		"D-Link-Starter:1"
#define _OT_UpnpDef_CfgMaster	"D-Link-CfgMaster:1"
#define _OT_UpnpDef_CfgSlave	"D-Link-CfgSlave:1"

#define PATH_OtSpecVersion			_OT_Path "/specversion"
#define PATH_OtApCloning			_OT_Path "/apcloning"
#define PATH_OtApCloningInfo		_OT_Path "/apcloninginfo"
#define PATH_OtEasyRoaming			_OT_Path "/easyroaming"
#define PATH_OtEasyRoamingInfo		_OT_Path "/easyroaminginfo"
#define PATH_OtFwUpgrade			_OT_Path "/fwupgrade"
#define PATH_OtTempDir				_OT_Path "/tempdirecory"
#define PATH_OtScriptDir			_OT_Path "/scriptdirecory"
#define PATH_OtApcConnMaster		PATH_OtApCloningInfo "/connmaster"
#define PATH_UplinkExtenderSite		"/uplink/extendersite"
#define PATH_UplinkDSL				"/uplink/dsl"
#define PATH_UplinkAPN				"/uplink/apn"
#define PATH_SysUser				"/sys/user:1"
#define PATH_LanWlan				"/lan/wireless%s:1"
#define PATH_LanWlanID				"/lan/wireless%s:%d"
#define PATH_WlanWirelessBSSID		PATH_LanWlan "/wirelessbssid"
#define PATH_FwUpgInfo				"/fwupgradeinfo"
#define PATH_RtOT					"/runtime" _OT_Path
#define PATH_RtOtException			PATH_RtOT "/exception"
#define PATH_RtOtSysNotifyCmd		PATH_RtOT "/sysnotifycmd"
#define PATH_RtVersionInfo			"/runtime/VerInfo"
#define PATH_RtSys					"/runtime/sys"
#define PATH_RtSysAliveMsgId		PATH_RtSys "/AliveMsgId"
#define PATH_RtSysAliveActCmd		PATH_RtSys "/AliveActCmd"
#define PATH_RtChkStarterKey		"/runtime/check" _OT_StarterKey
#define PATH_RtRssValidate			"/runtime/rss_validate"
#define PATH_RtApCloningInfo		"/runtime/apcloninginfo"
#define PATH_RtApcLivingSlave 		PATH_RtApCloningInfo "/livingslave"
#define PATH_RtMonitorSlave 		PATH_RtApCloningInfo "/monitorslave"
#define PATH_RtApcFailoverBSSID 	PATH_RtApCloningInfo "/failoverbssid"
#define PATH_RtApcExtenderSite		PATH_RtApCloningInfo "/extendersite"
#define PATH_RtApcLivingMaster		PATH_RtApCloningInfo "/livingmaster"
#define PATH_RtEasyRoaming			"/runtime/easyroaming"
#define PATH_RtErConnStaInfo		PATH_RtEasyRoaming "/connstainfo"
#define PATH_RtErDeAuthSta			PATH_RtEasyRoaming "/deauthsta"
#define PATH_RtFwUpgInfo			"/runtime/fwupgradeinfo"
#define PATH_RtFwUpgResult 			PATH_RtFwUpgInfo "/FwUpgResult"


#define MATCH_PREFIX(a, b)  (strncmp((a),(b),sizeof(b)-1)==0)
#define IMATCH_PREFIX(a, b)  (strncasecmp((a),(b),sizeof(b)-1)==0)

#define WLANIF_Virtual_MAC	"00:00:00:00:00:00"

char *starterLibVersion();
char *rssSpecVersion();

unsigned long getFileSize(const char *path); 
char *strChgRssKeywords(char *data);

/* DLINK add [Jack: 2014/09/04, monitor Wlan channelusing, used by Rss] */
int checkWlanChannelUsing(); 

/* DLINK add/s [Jack: 2014/08/29, used for RssUI one device] */
#define  rssVarHeader	"rss_"
void write_TmpXmlFile(char *fileName, char *tagName, char *tagVal, int isClose);
void write_ResultXmlFile(char *fileName, char *tagName, char *tagVal);
void remove_TmpXmlFile(char *fileName);
void do_RssVar_Commit();
int do_RssVar_Parser(char *bb, char dchar, char *errmsg, int isCommit);
char *cvtRss_XmlPathVsVarName(char *src, int isGetPath);
typedef enum { RssVar_GET = 1, RssVar_XMLPATH=256, RssVar_SET=512, RssVar_VALIDATE=1024} rssVar_Act_t;
int setget_RssVar(char *varName, char *theVal, rssVar_Act_t act);
int set_RssVar(char *varName, char *newVal);
int get_RssVar(char *varName, char *theVal, int isXmlPath);
char *getRssVar(char *varName);
int validate_RssVar(char *varName, char *theVal);
int validate_RssVar2(char *data);
void init_RssVar();
/* DLINK add/e [Jack: 2014/08/29] */

void write_RssXml_ResultXml(int result, char *msg, int sec);
void do_RssXml_Commit_for_ApCloning(int isChgSeq);
void do_RssXml_Commit();
char *get_FwServerWebSite();
int do_RssXml_Parser_for_ApCloning(char *bb);
int do_RssXml_Parser(char *bb);

int do_FwXml_Parser(char *bb, char *newFwFileUrl, char *newFwNotesUrl);

#define ChkFW_XmlFile	"rsscheckfw.xml"
#define FWUPLOAD_WEBSITE_ENG    "http://fwcontroller.oss.aliyuncs.com"
#define FWUPLOAD_WEBSITE        "http://fwcontroller.dlink.com.tw"

typedef enum { 
	FWUPGRADE_ERR_FWXML = -1,
	FWUPGRADE_ERR_BIN = -2,
	FWUPGRADE_ERR_ACT = -3,
	FWUPGRADE_ERR_TIMEOUT = -4,
	FWUPGRADE_ERR_WAN = -5,
	FWUPGRADE_CHKXML = 0,
	FWUPGRADE_XMLGETTING = 1,
	FWUPGRADE_XMLREADY = 2,
	FWUPGRADE_FINDNEW = 10,
	FWUPGRADE_BINGETTING = 11,
	FWUPGRADE_BINREADY = 20,
	FWUPGRADE_WRITINGFLASH = 30,
	FWUPGRADE_UPToDATE = 91,
} FwUpgradeState_t;

typedef enum { 
	FWUPGRADE_ACT_INIT = '0',
	FWUPGRADE_ACT_SEARCH = '1',
	FWUPGRADE_ACT_DOWNLOAD = '2',
	FWUPGRADE_ACT_CHKBIN = '3',
	FWUPGRADE_ACT_UPGRADE = '4',
	FWUPGRADE_ACT_NONE = '9',
} FwUpgradeAction_t;

typedef enum { 
    RssEnblType_OFF = 0, 
    RssEnblType_ON = 1, 
    RssEnblType_ALWAYSOFF = 2,
	RssEnblType_ALWAYSON = 3
} rssEnblType_t;

void do_FwUpgradeAction(int act);

#endif /* __XML_PARSER_H__ */

