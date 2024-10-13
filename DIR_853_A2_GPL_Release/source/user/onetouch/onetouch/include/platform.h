/****************************************************************************/
/*                                                                          */
/* PLATFORM.H                                                               */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines all APIs of platform indenpent                      */
/*                                                                          */
/****************************************************************************/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "rss.h"

#define xget_str 		dxmlGetStr
#define xget_encrystr	dxmlGetEncryStr
#define xset_str		dxmlSetStr
#define xset_file		dxmlSetFile
#define xget_int		dxmlGetInt
#define xset_int		dxmlSetInt
#define xget_row		dxmlGetRow
#define xget_noe		dxmlGetNoe
#define xdel 			dxmlDel

#ifndef __IDXML_API_H__ 
extern char *dxmlGetStr(char *buf, const char *format, ...);
extern char *dxmlGetEncryStr(char *buf, const char *format, ...);
extern int dxmlSetStr(char *buf, const char *format, ...);
extern int dxmlSetFile(char *filename);
extern signed int dxmlGetInt(const char *format, ...);
extern int dxmlSetInt(int set_value, const char *format, ...);
extern int dxmlGetRow(unsigned short *rowcount, const char *format, ...);
extern int dxmlGetNoe(const char * format, ...);
extern int dxmlDel(const char *format, ...);
#endif  /* __IDXML_API_H__ */

void dbgPrintf(char *fmt,...);
void msgPrintf(char *fmt,...);
char *uiDefaultUserID();
char *uiDefaultUserPW();
int get_ExternalCall_UseSec(int sync_id);
int exe_ExternalCall(int exeFlag);
int processRssNotification(rssActionType_t act_ID, rssActionStat_t stat_ID, rssExceptionType_t excI_D, char *msg);

#define RssNotification_Done(act_ID) 					processRssNotification(act_ID, RssActionStat_Done, RssExceptionType_None, "")	
#define RssNotification_DoneMsg(act_ID, msg) 			processRssNotification(act_ID, RssActionStat_Done, RssExceptionType_None, msg)	
#define RssNotification_Exception(act_ID, exc_ID, msg) 	processRssNotification(act_ID, RssActionStat_Exception, exc_ID, msg)	
#define RssNotification_CloneReject(msg)				processRssNotification(RssActionType_CloneToSlaves, RssActionStat_Exception, RssExceptionType_RejectAction, msg)	
#define RssNotification_CloneBefore() 					processRssNotification(RssActionType_CloneToSlaves, RssActionStat_Before, RssExceptionType_None, "")	
#define RssNotification_CloneException(FailSlaveList) 	processRssNotification(RssActionType_CloneToSlaves, RssActionStat_Exception, RssExceptionType_CloneFail, FailSlaveList)	
#define RssNotification_CloneDone(DoneSlaveList) 	processRssNotification(RssActionType_CloneToSlaves, RssActionStat_Done, RssExceptionType_None, DoneSlaveList)	

#endif /* __PLATFORM_H__ */
