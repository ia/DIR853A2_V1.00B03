/****************************************************************************/
/*                                                                          */
/* DEBUG_LOG.H                                                              */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines all APIs to log the (debug) message to a temp file. */
/*                                                                          */
/****************************************************************************/
#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#define LOG_LEVEL_ALERT			1
#define LOG_LEVEL_WARNING		4
#define LOG_LEVEL_NOTICE		5
#define LOG_LEVEL_INFO			6
#define LOG_LEVEL_DEBUG			7

#define D_LOGLEVEL daemon_log_level
#define D_LOGHEADER daemon_log_header
#define D_LOGPATH daemon_log_path
#define D_LOGFILESIZE_K daemon_log_file_size_k
extern int D_LOGLEVEL;
extern char D_LOGHEADER[];
extern char D_LOGPATH[];
extern int D_LOGFILESIZE_K;

void logPrintf(int log_level, char *fmt, ...);

#define LOG_MSG(fmt, args...)  		do {logPrintf(0, fmt, ## args);} while(0)
#define LOG_ALERT(fmt, args...)  	do {if (D_LOGLEVEL >= LOG_LEVEL_ALERT)   {logPrintf(LOG_LEVEL_ALERT, fmt, ## args);		}} while(0)
#define LOG_WARNING(fmt, args...)  	do {if (D_LOGLEVEL >= LOG_LEVEL_WARNING) {logPrintf(LOG_LEVEL_WARNING, fmt, ## args);	}} while(0)
#define LOG_NOTICE(fmt, args...)  	do {if (D_LOGLEVEL >= LOG_LEVEL_NOTICE)  {logPrintf(LOG_LEVEL_NOTICE, fmt, ## args);	}} while(0)
#define LOG_INFO(fmt, args...)  	do {if (D_LOGLEVEL >= LOG_LEVEL_INFO)    {logPrintf(LOG_LEVEL_INFO, fmt, ## args);		}} while(0)
#define LOG_DEBUG(fmt, args...)  	do {if (D_LOGLEVEL >= LOG_LEVEL_DEBUG) 	 {logPrintf(LOG_LEVEL_DEBUG, fmt, ## args);		}} while(0)

#define THELOGTIME()						dbgPrintf("%ld:",sys_uptime_seconds())
#define LOG_MSG_TIME(fmt, args...)  		do {msgPrintf("%ld:",sys_uptime_seconds()); logPrintf(0, fmt, ## args);} while(0)
#define LOG_ALERT_TIME(fmt, args...)  		do {if (D_LOGLEVEL >= LOG_LEVEL_ALERT)   {THELOGTIME(); logPrintf(LOG_LEVEL_ALERT, fmt, ## args);	}} while(0)
#define LOG_WARNING_TIME(fmt, args...)  	do {if (D_LOGLEVEL >= LOG_LEVEL_WARNING) {THELOGTIME(); logPrintf(LOG_LEVEL_WARNING, fmt, ## args);	}} while(0)
#define LOG_NOTICE_TIME(fmt, args...)  		do {if (D_LOGLEVEL >= LOG_LEVEL_NOTICE)  {THELOGTIME(); logPrintf(LOG_LEVEL_NOTICE, fmt, ## args);	}} while(0)
#define LOG_INFO_TIME(fmt, args...)  		do {if (D_LOGLEVEL >= LOG_LEVEL_INFO)    {THELOGTIME(); logPrintf(LOG_LEVEL_INFO, fmt, ## args);	}} while(0)
#define LOG_DEBUG_TIME(fmt, args...)  		do {if (D_LOGLEVEL >= LOG_LEVEL_DEBUG)   {THELOGTIME(); logPrintf(LOG_LEVEL_DEBUG, "%s]",__func__); logPrintf(LOG_LEVEL_DEBUG, fmt, ## args);	}} while(0)
#define LOG_DEBUG_LINE()			  		do {if (D_LOGLEVEL >= LOG_LEVEL_DEBUG)   {logPrintf(LOG_LEVEL_DEBUG, "%d>",__LINE__); }} while(0)

#endif /* __DEBUG_LOG_H__ */

