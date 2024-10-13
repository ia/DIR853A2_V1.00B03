 /*
 * iconf.h
 *
 * Config Management Common API
 * Interfaces to communicate with configuration manager 
 *
 */

#ifndef __IDXML_H__
#define __IDXML_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 
 * When attempting to get the value for the passing in XML path and it path does not exist
 * The function xget_int() will return the CONFIG_GET_NONE.
 */
#define CONFIG_GET_NONE			-9999

#define ICONF_LIB_VER			"1.1" SVN_REVISION

/**********************************************************************
 * Media type - where the configuration file should be saved
 *********************************************************************/
//typedef u_int8_t media_t;
#define MEDIA_ALL				0
#define MEDIA_RAM				1
#define MEDIA_FLASH				2
/* MEDIA_MAX is the amount of available media_t, media_t argument pass to libsysmgmt MUST < MEDIA_MAX */
#define MEDIA_MAX				3

/**********************************************************************
 * Command type - what to execute
 *********************************************************************/
//The order of command type DO matter!!
/* CMD_ALL must be the first and equal zero */
//typedef u_int8_t cmd_t;
#define CMD_ALL					0
#define CMD_LOGD				1
#define CMD_LAN					2
#define CMD_WAN					3
#define CMD_DNSRESOLVE			4
#define CMD_TELNETD				5
#define CMD_ROUTED				6
#define CMD_DHCPD				7
#define CMD_SECURITY			8
#define CMD_RIPD				9
#define CMD_WIRELESS			10
#define CMD_TIMEZONE			11
#define CMD_VOICE				12
#define CMD_PAGE_MISC			13
#define CMD_DDNS				14
#define CMD_ADSL				15
#define CMD_PORTMAP				16
#define CMD_IGMP				17
#define CMD_DIAGNOSIS			18
#define CMD_RUN_RGDB_CMD		19
#define CMD_PASSTHROUGH			20
#define CMD_FTPD				21
#define CMD_UPNP				22
#define CMD_TR069				23
#define CMD_PROVISION			24
#define CMD_ACCOUNT				25
#define CMD_SIP_SERVER			26
#define CMD_PHONE				27
#define CMD_INCOMING_CALL_RULES	28
#define CMD_OUT_DIAL_RULES		29
#define CMD_SPEED_DIAL			30
#define CMD_CALL_BLOCK			31
#define CMD_QOS					32	
/* CMD_MAX is the amount of available cmd_t, cmd_t argument pass to libsysmgmt MUST < CMD_MAX */
#define CMD_MAX					33

/**********************************************************************
 * Execute type - How to execute
 *********************************************************************/
//typedef u_int8_t exec_t;
//typedef u_int8_t exec_sub_t;
#define EXEC_START				0
#define EXEC_STOP				1
#define EXEC_RESTART			2
#define EXEC_SUSPEND			3
#define EXEC_RESUME				4
/* EXEC_MAX is the amount of available exec_t, exec_t argument pass to libsysmgmt MUST < EXEC_MAX */
#define EXEC_MAX				5

/**********************************************************************
 * GetSet type - How to map to ConfigGetSet
 *********************************************************************/
#define CONFIG_SET				0
#define CONFIG_GET				1
#define CONFIG_DEL				2
#define CONFIG_GET_ENCRY		4
#define CONFIG_SET_FILE			8
#define RG_OK					0

#if CONFIG_DEBUG
#define xprintf(fmt, args...)	fprintf(stderr, fmt, ## args)
#else
#define xprintf(fmt, args...)
#endif

#ifndef TRUE
#define TRUE					(1 == 1)
#endif

#ifndef FALSE
#define FALSE					(0 == 1)
#endif

extern char *iconfLibVersion();

extern int lib_ConfigGsd(const char *xpath, char *buf, int flag);
extern int lib_ConfigCountRow(const char *xpath, u_int16_t *rowcount);
extern int lib_ConfigCommit(u_int8_t media);
extern int lib_Execute(u_int8_t cmd, u_int8_t type);
extern int lib_ExecuteExtend(u_int8_t cmd, u_int8_t type, int index);
extern int lib_ExeShell(char *cmd);
extern int lib_ConfigAuth(const char *module, const char *username, const char *password, const char *ipaddr);
#endif /* __IDXML_H__ */

