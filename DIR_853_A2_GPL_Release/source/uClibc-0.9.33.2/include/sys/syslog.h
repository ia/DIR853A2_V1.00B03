/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)syslog.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _SYS_SYSLOG_H
#define _SYS_SYSLOG_H 1

#include <features.h>
#define __need___va_list
#include <stdarg.h>


#define	_PATH_LOG	"/var/syslogd"

/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
				/* extract priority */
#define	LOG_PRI(p)	((p) & LOG_PRIMASK)
#define	LOG_MAKEPRI(fac, pri)	(((fac) << 3) | (pri))

#ifdef SYSLOG_NAMES
#define	INTERNAL_NOPRI	0x10	/* the "no priority" priority */
				/* mark "facility" */
#define	INTERNAL_MARK	LOG_MAKEPRI(LOG_NFACILITIES, 0)
typedef struct _code {
	const char      *c_name;
	int             c_val;
} CODE;

#ifdef SYSLOG_NAMES_CONST
const
#endif
CODE prioritynames[] =
  {
    { "alert", LOG_ALERT },
    { "crit", LOG_CRIT },
    { "debug", LOG_DEBUG },
    { "emerg", LOG_EMERG },
    { "err", LOG_ERR },
    { "error", LOG_ERR },		/* DEPRECATED */
    { "info", LOG_INFO },
    { "none", INTERNAL_NOPRI },		/* INTERNAL */
    { "notice", LOG_NOTICE },
    { "panic", LOG_EMERG },		/* DEPRECATED */
    { "warn", LOG_WARNING },		/* DEPRECATED */
    { "warning", LOG_WARNING },
    { NULL, -1 }
  };
#endif

/* facility codes */
#define	LOG_KERN	(0<<3)	/* kernel messages */
#define	LOG_USER	(1<<3)	/* random user-level messages */
#define	LOG_MAIL	(2<<3)	/* mail system */
#define	LOG_DAEMON	(3<<3)	/* system daemons */
#define	LOG_AUTH	(4<<3)	/* security/authorization messages */
#define	LOG_SYSLOG	(5<<3)	/* messages generated internally by syslogd */
#define	LOG_LPR		(6<<3)	/* line printer subsystem */
#define	LOG_NEWS	(7<<3)	/* network news subsystem */
#define	LOG_UUCP	(8<<3)	/* UUCP subsystem */
#define	LOG_CRON	(9<<3)	/* clock daemon */
#define	LOG_AUTHPRIV	(10<<3)	/* security/authorization messages (private) */
#define	LOG_FTP		(11<<3)	/* ftp daemon */

	/* other codes through 15 reserved for system use */
#define	LOG_LOCAL0	(16<<3)	/* reserved for local use */
#define	LOG_LOCAL1	(17<<3)	/* reserved for local use */
#define	LOG_LOCAL2	(18<<3)	/* reserved for local use */
#define	LOG_LOCAL3	(19<<3)	/* reserved for local use */
#define	LOG_LOCAL4	(20<<3)	/* reserved for local use */
#define	LOG_LOCAL5	(21<<3)	/* reserved for local use */
#define	LOG_LOCAL6	(22<<3)	/* reserved for local use */
#define	LOG_LOCAL7	(23<<3)	/* reserved for local use */

#define	LOG_NFACILITIES	24	/* current number of facilities */
#define	LOG_FACMASK	0x03f8	/* mask to extract facility part */
				/* facility of pri */
#define	LOG_FAC(p)	(((p) & LOG_FACMASK) >> 3)


#define   LOG_IGMP				"[IGMP]" 			/* IGMP */
#define   LOG_LAN				"[LAN]" 			/* LAN */
#define   LOG_IPT				"[IPT]" 			/* IPT */
#define   ETHLAN				"[ETHLAN]" 			/* LAN端以太网 */
#define   ETHWAN				"[ETHWAN]" 			/* WAN端以太网 */
#define   LOG_PPPOE 			"[PPPOE]" 			/* PPPOE */
#define   LOG_WLAN				"[WLAN]" 			/* WLAN模块 */
#define   LOG_TR069BE			"[TR069BE]" 		/* TR069BE */
#define   LOG_DGN				"[DGN]" 			/* Diagnostics */
#define   LOG_DHCPR 			"[DHCPR]" 			/* dhcp relay */
#define   LOG_DHCPS 			"[DHCPS]" 			/* dhcp server */
#define   LOG_DHCPC 			"[DHCPC]" 			/* dhcp server */
#define   LOG_DHCP6R 			"[DHCPR-6]" 			/* dhcp relay */
#define   LOG_DHCP6S 			"[DHCPS-6]" 			/* dhcp server */
#define   LOG_DHCP6C 			"[DHCPC-6]" 			/* dhcp server */
#define   LOG_TIMER 			"[TIMER]" 			/* Timer */
#define   LOG_IPCONN			"[IPCONN]" 			/* wan ip connection module */
#define   LOG_FIREWALL			"[FIREWALL]" 	   	/* Fire wall */
#define   LOG_SNMPC 			"[SNMPC]" 			/* SNMP配置管理模块 */
#define   LOG_EMAIL				"[EMAIL]" 			/* 邮件管理模块 */
#define   LOG_QOS				"[QOS]" 			/* QOS模块 */
#define   LOG_SROUTE			"[SROUTE]" 			/* static routing */
#define   LOG_VDSL				"[VDSL]" 			/* VDSL模块 */
#define   LOG_DNS				"[DNS]" 			/* DNS模块 */
#define   LOG_ALG				"[ALG]" 			/* ALG模块 */
#define   LOG_WAN				"[WAN]" 			/* WAN模块 */
#define   LOG_DROUTE			"[DROUTE]" 			/* dynamic routing */
#define   LOG_SNTP				"[SNTP]" 			/* sntp */
#define   LOG_SNTP_HOTA 		"[SNTP_HOTA]" 	 	/* sntp hota*/
#define   LOG_VLAN				"[VLAN]" 			/* vlan */
#define   LOG_USB_MASS			"[USB_MASS]" 	   	/* USB mass storage module */
#define   LOG_LOG				"[LOG]" 			/* LOGGER 模块 */
#define   LOG_FTPD				"[FTPD]" 			/* FTPD module */
#define   LOG_NATPRIO			"[NATPRIO]" 		/* NAT 优先级 */
#define   LOG_WPS				"[WPS]" 			/* WPS模块 */
#define   LOG_ACL				"[ACL]" 			/* ACL */
#define   LOG_UPNP				"[UPNP]" 			/* UPNP */
#define   LOG_LSVLAN			"[LSVLAN]" 			/* LSVLAN */
#define   LOG_PORTOFF			"[PORTOFF]" 		/* PORTOFF:端口隔离 */
#define   LOG_ANTIATTACK		"[ANTIATTACK]"    	/* ANTIATTACK:防攻击 */
#define   LOG_DEVINFO			"[DEVINFO]" 		/* DEVINFO模块 */
#define   LOG_PORTMAPPING		"[PORTMAPPING]"  	/* PortMapping模块 */
#define   LOG_URLFILTER 		"[URLFILTER]" 		/* URL FILTER */
#define   LOG_ATM				"[ATM]" 			/* ATM模块 */
#define   LOG_DSL				"[DSL]" 			/* DSL模块 */
#define   LOG_MDNS				"[MDNS]" 			/* MDNS模块 */
#define   LOG_HRE				"[HRE]" 			/* HRE模块 */
#define   LOG_HRE_DS			"[HRE_DS]" 			/* HRE DHCP SERVER模块 */
#define   LOG_HOTA				"[HOTA]" 			/* HOTA模块 */
#define   LOG_HOTA_WGET 		"[HOTA_WGET]" 		/* HOTA模块 */
#define   LOG_HOTA_DHCPC		"[HOTA_DHCPC]"    	/* HOTA模块 */
#define   LOG_AUTOUPG			"[AUTOUPG]" 		/* AUTOUPG模块 */
#define   LOG_ONETOUCH			"[ONETOUCH]" 	   	/* ONETOUCH模块 */
#define   LOG_BRIDGEEREGETIP	"[BRIDGEEREGETIP]"  /* BRIDGEEREGETIP模块 */
#define   LOG_PROUTE			"[PROUTE]" 			/* 策略路由 */
#define   LOG_CFG				"[CFG]" 			/* cfg恢复出厂默认保留关键参数 */
#define   LOG_SUPP				"[SUPP]" 			/* SUPP模块 */
#define   LOG_MACFILTER 		"[MACFILTER]" 	 	/* MACFILTER模块 */
#define   LOG_TRACERT			"[TRACERT]" 		/* TRACERT模块*/
#define   LOG_IPPD				"[IPPD]" 			/* IPPD模块*/
#define   LOG_WEBP				"[WEBP]" 			/* web代理模块 */
#define   LOG_BRIDGE_FILTER 	"[BRIDGE_FILTER]"   /* bridgefilter模块 */
#define   LOG_PORTTRIGGER		"[PORTTRIGGER]" 	/* 端口触发 */
#define   LOG_IP_ACL			"[IP_ACL]" 			/* IP_ACL */
#define   LOG_DEFAULTGW 		"[DEFAULTGW]" 	 	/* 默认网关 */
#define   LOG_DIAG				"[DIAG]" 			/* Diagnostics */
#define   LOG_SERVICE			"[SERVICE]" 		/* Diagnostics */
#define   LOG_IFPLUGD			"[IFPLUGD]" 		/* Diagnostics */
#define   LOG_FTPD				"[FTPD]" 			/* Diagnostics */
#define   LOG_SAMBA				"[SAMBA]" 			/* Diagnostics */
#define   LOG_BPALOGIN			"[BPALOGIN]" 			/* Diagnostics */
#define   LOG_MLD				"[MLD]" 			/* Diagnostics */
#define   LOG_ARPD				"[ARPD]" 			/* Diagnostics */
#define   LOG_SNMP				"[SNMP]" 			/* Diagnostics */
#define   LOG_XL2TP				"[XL2TP]" 			/* Diagnostics */
#define   LOG_PLUTO				"[PLUTO]" 			/* Diagnostics */
#define   LOG_HTTP				"[HTTP]" 			/* Diagnostics */
#define   LOG_DNSMASQ			"[DNSMASQ]" 			/* Diagnostics */
#define   LOG_DDNS				"[DDNS]" 			/* Diagnostics */
#define   LOG_L2TPD				"[L2TPD]" 			/* Diagnostics */
#define   LOG_RC				"[RC]" 			/* Diagnostics */






#ifdef SYSLOG_NAMES
#ifdef SYSLOG_NAMES_CONST
const
#endif
CODE facilitynames[] =
  {
    { "auth", LOG_AUTH },
    { "authpriv", LOG_AUTHPRIV },
    { "cron", LOG_CRON },
    { "daemon", LOG_DAEMON },
    { "ftp", LOG_FTP },
    { "kern", LOG_KERN },
    { "lpr", LOG_LPR },
    { "mail", LOG_MAIL },
    { "mark", INTERNAL_MARK },		/* INTERNAL */
    { "news", LOG_NEWS },
    { "security", LOG_AUTH },		/* DEPRECATED */
    { "syslog", LOG_SYSLOG },
    { "user", LOG_USER },
    { "uucp", LOG_UUCP },
    { "local0", LOG_LOCAL0 },
    { "local1", LOG_LOCAL1 },
    { "local2", LOG_LOCAL2 },
    { "local3", LOG_LOCAL3 },
    { "local4", LOG_LOCAL4 },
    { "local5", LOG_LOCAL5 },
    { "local6", LOG_LOCAL6 },
    { "local7", LOG_LOCAL7 },
    { NULL, -1 }
  };
#endif

/*
 * arguments to setlogmask.
 */
#define	LOG_MASK(pri)	(1 << (pri))		/* mask for one priority */
#define	LOG_UPTO(pri)	((1 << ((pri)+1)) - 1)	/* all priorities through pri */

/*
 * Option flags for openlog.
 *
 * LOG_ODELAY no longer does anything.
 * LOG_NDELAY is the inverse of what it used to be.
 */
#define	LOG_PID		0x01	/* log the pid with each message */
#define	LOG_CONS	0x02	/* log on the console if errors in sending */
#define	LOG_ODELAY	0x04	/* delay open until first syslog() (default) */
#define	LOG_NDELAY	0x08	/* don't delay open */
#define	LOG_NOWAIT	0x10	/* don't wait for console forks: DEPRECATED */
#define	LOG_PERROR	0x20	/* log to stderr as well */

__BEGIN_DECLS

/* Close descriptor used to write to system logger.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern void closelog (void);
libc_hidden_proto(closelog);

/* Open connection to system logger.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern void openlog (__const char *__ident, int __option, int __facility);
libc_hidden_proto(openlog);

/* Set the log mask level.  */
extern int setlogmask (int __mask) __THROW;

/* Generate a log message using FMT string and option arguments.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern void syslog (int __pri, __const char *__fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));
libc_hidden_proto(syslog);

#ifdef __USE_BSD
/* Generate a log message using FMT and using arguments pointed to by AP.

   This function is not part of POSIX and therefore no official
   cancellation point.  But due to similarity with an POSIX interface
   or due to the implementation it is a cancellation point and
   therefore not marked with __THROW.  */
extern void vsyslog (int __pri, __const char *__fmt, __gnuc_va_list __ap)
     __attribute__ ((__format__ (__printf__, 2, 0)));
libc_hidden_proto(vsyslog);
#endif

__END_DECLS

#endif /* sys/syslog.h */
