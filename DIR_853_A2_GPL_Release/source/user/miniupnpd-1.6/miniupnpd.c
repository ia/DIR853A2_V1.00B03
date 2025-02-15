/* $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/miniupnpd-1.6/miniupnpd.c#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2012 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#include "config.h"

/* Experimental support for NFQUEUE interfaces */
#ifdef ENABLE_NFQUEUE
/* apt-get install libnetfilter-queue-dev */
#include <netinet/ip.h>
#include <netinet/udp.h>
#if 0
#include <linux/netfilter_ipv4.h>  /* Defines verdicts (NF_ACCEPT, etc) */
#endif
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <linux/netfilter/nfnetlink_queue.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/file.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/param.h>
#if defined(sun)
#include <kstat.h>
#else
/* for BSD's sysctl */
#include <sys/sysctl.h>
#endif

/* unix sockets */
#ifdef USE_MINIUPNPDCTL
#include <sys/un.h>
#endif

#include "upnpglobalvars.h"
#include "upnphttp.h"
#include "upnpdescgen.h"
#include "miniupnpdpath.h"
#include "getifaddr.h"
#include "upnpsoap.h"
#include "options.h"
#include "minissdp.h"
#include "upnpredirect.h"
#include "miniupnpdtypes.h"
#include "daemonize.h"
#include "upnpevents.h"
#ifdef ENABLE_NATPMP
#include "natpmp.h"
#endif
#include "commonrdr.h"
#include "upnputils.h"

#ifdef HW_NAT_EBL
#if !defined (CONFIG_HNAT_V2)
#include "ac_ioctl.h"
#include "ac_api.h"
#endif
#endif // HW_NAT_EBL //

#ifdef ENABLE_WSC_SERVICE
#include "wsc/wsc_common.h"
//#include "wsc/wsc_netlink.h"
#ifdef RT_DEBUG
/* To be improved */
int
xml_print(const char * s, int len, FILE * f)
{
	int n = 0, i;
	int elt_close = 0;
	int c, indent = 0;

	while(len > 0)
	{
		c = *(s++);	len--;
		switch(c)
		{
		case '<':
			if(len>0 && *s == '/')
				elt_close++;
			else if(len>0 && *s == '?')
				elt_close = 1;
			else
				elt_close = 0;
			if(elt_close!=1)
			{
				if(elt_close > 1)
					indent--;
				fputc('\n', f); n++;
				for(i=indent; i>0; i--)
					fputc(' ', f);
				n += indent;
			}
			fputc(c, f); n++;
			break;
		case '>':
			fputc(c, f); n++;
			if(elt_close==1)
			{
				/*fputc('\n', f); n++; */
				//elt_close = 0;
				if(indent > 0)
					indent--;
			}
			else if(elt_close == 0)
				indent++;
			break;
		default:
			fputc(c, f); n++;
		}
	}
	return n;
}

int DescDump()
{
	char * Desc;
	int DescLen;
	char * s;
	int l;

	Desc = genWSCRootDesc((int *)&DescLen);
	xml_print(Desc, DescLen, stdout);
	free(Desc);
	printf("\n-------------\n");

	s = genWSC((int *)&l);
	xml_print(s, l, stdout);
	free(s);
	printf("\n-------------\n");
#ifdef ENABLE_EVENTS
	s = getVarsWSC((int *)&l);
	xml_print(s, l, stdout);
	free(s);
	printf("\n-------------\n");
#endif /* ENABLE_EVENTS */
	return 0;
}
#endif /* RT_DEBUG */
#endif /* ENABLE_WSC_SERVICE */


#ifdef USE_IFACEWATCHER
#include "ifacewatcher.h"
#endif

#ifndef DEFAULT_CONFIG
#define DEFAULT_CONFIG "/etc/miniupnpd.conf"
#endif

#ifdef USE_MINIUPNPDCTL
struct ctlelem {
	int socket;
	LIST_ENTRY(ctlelem) entries;
};
#endif

#ifdef ENABLE_NFQUEUE
/* globals */
static struct nfq_handle *nfqHandle;
static struct sockaddr_in ssdp;

/* prototypes */
static int nfqueue_cb( struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data) ;
int identify_ip_protocol (char *payload);
int get_udp_dst_port (char *payload);
#endif

/* variables used by signals */
static volatile int quitting = 0;
volatile int should_send_public_address_change_notif = 0;

#ifdef ENABLE_WSC_SERVICE
extern int wsc_debug_level;
#endif /* ENABLE_WSC_SERVICE */

/* OpenAndConfHTTPSocket() :
 * setup the socket used to handle incoming HTTP connections. */
static int
OpenAndConfHTTPSocket(unsigned short port)
{
	int s;
	int i = 1;
#ifdef ENABLE_IPV6
	struct sockaddr_in6 listenname;
#else
	struct sockaddr_in listenname;
#endif
	socklen_t listenname_len;

	if( (s = socket(
#ifdef ENABLE_IPV6
	                PF_INET6,
#else
	                PF_INET,
#endif
	                SOCK_STREAM, 0)) < 0)
	{
		syslog(LOG_ERR, "socket(http): %m");
		return -1;
	}

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
	{
		syslog(LOG_WARNING, "setsockopt(http, SO_REUSEADDR): %m");
	}
#if 0
	/* enable this to force IPV6 only for IPV6 socket.
	 * see http://www.ietf.org/rfc/rfc3493.txt section 5.3 */
	if(setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &i, sizeof(i)) < 0)
	{
		syslog(LOG_WARNING, "setsockopt(http, IPV6_V6ONLY): %m");
	}
#endif

	if(!set_non_blocking(s))
	{
		syslog(LOG_WARNING, "set_non_blocking(http): %m");
	}

#ifdef ENABLE_IPV6
	memset(&listenname, 0, sizeof(struct sockaddr_in6));
	listenname.sin6_family = AF_INET6;
	listenname.sin6_port = htons(port);
	listenname.sin6_addr = in6addr_any;
	listenname_len =  sizeof(struct sockaddr_in6);
#else
	listenname.sin_family = AF_INET;
	listenname.sin_port = htons(port);
	listenname.sin_addr.s_addr = htonl(INADDR_ANY);
	listenname_len =  sizeof(struct sockaddr_in);
#endif

	if(bind(s, (struct sockaddr *)&listenname, listenname_len) < 0)
	{
		syslog(LOG_ERR, "bind(http): %m");
		close(s);
		return -1;
	}

	if(listen(s, 5) < 0)
	{
		syslog(LOG_ERR, "listen(http): %m");
		close(s);
		return -1;
	}

	return s;
}
#ifdef ENABLE_NFQUEUE

int identify_ip_protocol(char *payload) {
    return payload[9];
}


/*
 * This function returns the destination port of the captured packet UDP
 */
int get_udp_dst_port(char *payload) {
        char *pkt_data_ptr = NULL;
        pkt_data_ptr = payload + sizeof(struct ip);

    /* Cast the UDP Header from the raw packet */
    struct udphdr *udp = (struct udphdr *) pkt_data_ptr;

    /* get the dst port of the packet */
    return(ntohs(udp->dest));

}
static int
OpenAndConfNFqueue(){

        struct nfq_q_handle *myQueue;
        struct nfnl_handle *netlinkHandle;

        int fd = 0, e = 0;

	inet_pton(AF_INET, "239.255.255.250", &(ssdp.sin_addr));

        /* Get a queue connection handle from the module */
        if (!(nfqHandle = nfq_open())) {
		syslog(LOG_ERR, "Error in nfq_open(): %m");
                return -1;
        }

        /* Unbind the handler from processing any IP packets
           Not totally sure why this is done, or if it's necessary... */
        if ((e = nfq_unbind_pf(nfqHandle, AF_INET)) < 0) {
		syslog(LOG_ERR, "Error in nfq_unbind_pf(): %m");
                return -1;
        }

        /* Bind this handler to process IP packets... */
        if (nfq_bind_pf(nfqHandle, AF_INET) < 0) {
		syslog(LOG_ERR, "Error in nfq_bind_pf(): %m");
                return -1;
        }

        /*      Install a callback on queue -Q */
        if (!(myQueue = nfq_create_queue(nfqHandle,  nfqueue, &nfqueue_cb, NULL))) {
		syslog(LOG_ERR, "Error in nfq_create_queue(): %m");
                return -1;
        }

        /*      Turn on packet copy mode */
        if (nfq_set_mode(myQueue, NFQNL_COPY_PACKET, 0xffff) < 0) {
		syslog(LOG_ERR, "Error setting packet copy mode (): %m");
                return -1;
        }

        netlinkHandle = nfq_nfnlh(nfqHandle);
        fd = nfnl_fd(netlinkHandle);

	return fd;

}


static int nfqueue_cb(
                struct nfq_q_handle *qh,
                struct nfgenmsg *nfmsg,
                struct nfq_data *nfa,
                void *data) {

	char	*pkt;
	struct nfqnl_msg_packet_hdr *ph;
	ph = nfq_get_msg_packet_hdr(nfa);

	if ( ph ) {

		int id = 0, size = 0;
		id = ntohl(ph->packet_id);

		size = nfq_get_payload(nfa, &pkt);

    		struct ip *iph = (struct ip *) pkt;

		int id_protocol = identify_ip_protocol(pkt);

		int dport = get_udp_dst_port(pkt);

		int x = sizeof (struct ip) + sizeof (struct udphdr);

		/* packets we are interested in are UDP multicast to 239.255.255.250:1900
		 * and start with a data string M-SEARCH
		 */
		if ( (dport == 1900) && (id_protocol == IPPROTO_UDP)
			&& (ssdp.sin_addr.s_addr == iph->ip_dst.s_addr) ) {

			/* get the index that the packet came in on */
			u_int32_t idx = nfq_get_indev(nfa);
			int i = 0;
			for ( ;i < n_nfqix ; i++) {
				if ( nfqix[i] == idx ) {

					struct udphdr *udp = (struct udphdr *) (pkt + sizeof(struct ip));

					char *dd = pkt + x;

					struct sockaddr_in sendername;
					sendername.sin_family = AF_INET;
					sendername.sin_port = udp->source;
					sendername.sin_addr.s_addr = iph->ip_src.s_addr;

					/* printf("pkt found %s\n",dd);*/
					ProcessSSDPData (sudp, dd, size - x,
					                 &sendername, (unsigned short) 5555);
				}
			}
		}

		nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);

	} else {
		syslog(LOG_ERR,"nfq_get_msg_packet_hdr failed");
		return 1;
		/* from nfqueue source: 0 = ok, >0 = soft error, <0 hard error */
	}

	return 0;
}

static void ProcessNFQUEUE(int fd){
	char buf[4096];

	socklen_t len_r;
	struct sockaddr_in sendername;
	len_r = sizeof(struct sockaddr_in);

        int res = recvfrom(fd, buf, sizeof(buf), 0,
			(struct sockaddr *)&sendername, &len_r);

	nfq_handle_packet(nfqHandle, buf, res);
}
#endif

/* Functions used to communicate with miniupnpdctl */
#ifdef USE_MINIUPNPDCTL
static int
OpenAndConfCtlUnixSocket(const char * path)
{
	struct sockaddr_un localun;
	int s;
	s = socket(AF_UNIX, SOCK_STREAM, 0);
	localun.sun_family = AF_UNIX;
	strncpy(localun.sun_path, path,
	          sizeof(localun.sun_path));
	if(bind(s, (struct sockaddr *)&localun,
	        sizeof(struct sockaddr_un)) < 0)
	{
		syslog(LOG_ERR, "bind(sctl): %m");
		close(s);
		s = -1;
	}
	else if(listen(s, 5) < 0)
	{
		syslog(LOG_ERR, "listen(sctl): %m");
		close(s);
		s = -1;
	}
	return s;
}

static void
write_upnphttp_details(int fd, struct upnphttp * e)
{
	char buffer[256];
	int len;
	write(fd, "HTTP :\n", 7);
	while(e)
	{
		len = snprintf(buffer, sizeof(buffer),
		               "%d %d %s req_buf=%p(%dbytes) res_buf=%p(%dbytes alloc)\n",
		               e->socket, e->state, e->HttpVer,
		               e->req_buf, e->req_buflen,
		               e->res_buf, e->res_buf_alloclen);
		write(fd, buffer, len);
		e = e->entries.le_next;
	}
}

static void
write_ctlsockets_list(int fd, struct ctlelem * e)
{
	char buffer[256];
	int len;
	write(fd, "CTL :\n", 6);
	while(e)
	{
		len = snprintf(buffer, sizeof(buffer),
		               "struct ctlelem: socket=%d\n", e->socket);
		write(fd, buffer, len);
		e = e->entries.le_next;
	}
}

static void
write_option_list(int fd)
{
	char buffer[256];
	int len;
	int i;
	write(fd, "Options :\n", 10);
	for(i=0; i<num_options; i++)
	{
		len = snprintf(buffer, sizeof(buffer),
		               "opt=%02d %s\n",
		               ary_options[i].id, ary_options[i].value);
		write(fd, buffer, len);
	}
}

static void
write_command_line(int fd, int argc, char * * argv)
{
	char buffer[256];
	int len;
	int i;
	write(fd, "Command Line :\n", 15);
	for(i=0; i<argc; i++)
	{
		len = snprintf(buffer, sizeof(buffer),
		               "argv[%02d]='%s'\n",
		                i, argv[i]);
		write(fd, buffer, len);
	}
}

#endif

/* Handler for the SIGTERM signal (kill)
 * SIGINT is also handled */
static void
sigterm(int sig)
{
	/*int save_errno = errno;*/
	signal(sig, SIG_IGN);	/* Ignore this signal while we are quitting */

	syslog(LOG_NOTICE, "received signal %d, good-bye", sig);

	quitting = 1;
	/*errno = save_errno;*/
}

/* Handler for the SIGUSR1 signal indicating public IP address change. */
static void
sigusr1(int sig)
{
	syslog(LOG_INFO, "received signal %d, public ip address change", sig);

	should_send_public_address_change_notif = 1;
}

/* record the startup time, for returning uptime */
static void
set_startup_time(int sysuptime)
{
	startup_time = time(NULL);
	if(sysuptime)
	{
		/* use system uptime instead of daemon uptime */
#if defined(__linux__)
		char buff[64];
		int uptime = 0, fd;
		fd = open("/proc/uptime", O_RDONLY);
		if(fd < 0)
		{
			syslog(LOG_ERR, "open(\"/proc/uptime\" : %m");
		}
		else
		{
			memset(buff, 0, sizeof(buff));
			if(read(fd, buff, sizeof(buff) - 1) < 0)
			{
				syslog(LOG_ERR, "read(\"/proc/uptime\" : %m");
			}
			else
			{
				uptime = atoi(buff);
				syslog(LOG_INFO, "system uptime is %d seconds", uptime);
			}
			close(fd);
			startup_time -= uptime;
		}
#elif defined(SOLARIS_KSTATS)
		kstat_ctl_t *kc;
		kc = kstat_open();
		if(kc != NULL)
		{
			kstat_t *ksp;
			ksp = kstat_lookup(kc, "unix", 0, "system_misc");
			if(ksp && (kstat_read(kc, ksp, NULL) != -1))
			{
				void *ptr = kstat_data_lookup(ksp, "boot_time");
				if(ptr)
					memcpy(&startup_time, ptr, sizeof(startup_time));
				else
					syslog(LOG_ERR, "cannot find boot_time kstat");
			}
			else
				syslog(LOG_ERR, "cannot open kstats for unix/0/system_misc: %m");
			kstat_close(kc);
		}
#else
		struct timeval boottime;
		size_t size = sizeof(boottime);
		int name[2] = { CTL_KERN, KERN_BOOTTIME };
		if(sysctl(name, 2, &boottime, &size, NULL, 0) < 0)
		{
			syslog(LOG_ERR, "sysctl(\"kern.boottime\") failed");
		}
		else
		{
			startup_time = boottime.tv_sec;
		}
#endif
	}
}

#ifdef ENABLE_WSC_SERVICE
#if 0
/* structure containing variables used during "main loop"
 * that are filled during the init */
struct runtime_vars {
	/* LAN IP addresses for SSDP traffic and HTTP */
	/* moved to global vars */
	int port;	/* HTTP Port */
	int notify_interval;	/* seconds between SSDP announces */
	/* unused rules cleaning related variables : */
	int clean_ruleset_threshold;	/* threshold for removing unused rules */
	int clean_ruleset_interval;		/* (minimum) interval between checks */
};
#endif
#endif /* ENABLE_WSC_SERVICE */

/* parselanaddr()
 * parse address with mask
 * ex: 192.168.1.1/24 or 192.168.1.1/255.255.255.0
 * When MULTIPLE_EXTERNAL_IP is enabled, the ip address of the
 * external interface associated with the lan subnet follows.
 * ex : 192.168.1.1/24 81.21.41.11
 *
 * return value :
 *    0 : ok
 *   -1 : error */
static int
parselanaddr(struct lan_addr_s * lan_addr, const char * str)
{
	const char * p;
	int n;
	char tmp[16];

	p = str;
	while(*p && *p != '/' && !isspace(*p))
		p++;
	n = p - str;
	if(n>15)
		goto parselan_error;
	memcpy(lan_addr->str, str, n);
	lan_addr->str[n] = '\0';
	if(!inet_aton(lan_addr->str, &lan_addr->addr))
		goto parselan_error;
	if(*p == '/')
	{
		const char * q = ++p;
		while(*p && isdigit(*p))
			p++;
		if(*p=='.')
		{
			while(*p && (*p=='.' || isdigit(*p)))
				p++;
			n = p - q;
			if(n>15)
				goto parselan_error;
			memcpy(tmp, q, n);
			tmp[n] = '\0';
			if(!inet_aton(tmp, &lan_addr->mask))
				goto parselan_error;
		}
		else
		{
			int nbits = atoi(q);
			if(nbits > 32 || nbits < 0)
				goto parselan_error;
			lan_addr->mask.s_addr = htonl(nbits ? (0xffffffffu << (32 - nbits)) : 0);
		}
	}
	else
	{
		/* by default, networks are /24 */
		lan_addr->mask.s_addr = htonl(0xffffff00u);
	}
#ifdef MULTIPLE_EXTERNAL_IP
	/* skip spaces */
	while(*p && isspace(*p))
		p++;
	if(*p) {
		/* parse the exteral ip address to associate with this subnet */
		n = 0;
		while(p[n] && !isspace(*p))
			n++;
		if(n<=15) {
			memcpy(lan_addr->ext_ip_str, p, n);
			lan_addr->ext_ip_str[n] = '\0';
			if(!inet_aton(lan_addr->ext_ip_str, &lan_addr->ext_ip_addr)) {
				/* error */
				fprintf(stderr, "Error parsing address : %s\n", lan_addr->ext_ip_str);
			}
		}
	}
#endif
	return 0;
parselan_error:
	fprintf(stderr, "Error parsing address/mask : %s\n", str);
	return -1;
}

/* init phase :
 * 1) read configuration file
 * 2) read command line arguments
 * 3) daemonize
 * 4) open syslog
 * 5) check and write pid file
 * 6) set startup time stamp
 * 7) compute presentation URL
 * 8) set signal handlers */
static int
init(int argc, char * * argv, struct runtime_vars * v)
{
	int i;
	int pid;
	int debug_flag = 0;
	int options_flag = 0;
	int openlog_option;
	struct sigaction sa;
	/*const char * logfilename = 0;*/
	const char * presurl = 0;
	const char * optionsfile = DEFAULT_CONFIG;
	struct lan_addr_s * lan_addr;
	struct lan_addr_s * lan_addr2;

	/* only print usage if -h is used */
	for(i=1; i<argc; i++)
	{
		if(0 == strcmp(argv[i], "-h"))
			goto print_usage;
	}
	/* first check if "-f" option is used */
	for(i=2; i<argc; i++)
	{
		if(0 == strcmp(argv[i-1], "-f"))
		{
			optionsfile = argv[i];
			options_flag = 1;
			break;
		}
	}

#ifdef ENABLE_WSC_SERVICE
	SETFLAG(ENABLEWPSMASK);
#else
	/* set initial values */
	SETFLAG(ENABLEUPNPMASK);
#endif /* ENABLE_WSC_SERVICE */

	LIST_INIT(&lan_addrs);

	v->port = -1;
	v->notify_interval = 30;	/* seconds between SSDP announces */
	v->clean_ruleset_threshold = 20;
	v->clean_ruleset_interval = 0;	/* interval between ruleset check. 0=disabled */

	/* read options file first since
	 * command line arguments have final say */
	if(readoptionsfile(optionsfile) < 0)
	{
		/* only error if file exists or using -f */
		if(access(optionsfile, F_OK) == 0 || options_flag)
			fprintf(stderr, "Error reading configuration file %s\n", optionsfile);
	}
	else
	{
		for(i=0; i<num_options; i++)
		{
			switch(ary_options[i].id)
			{
			case UPNPEXT_IFNAME:
				ext_if_name = ary_options[i].value;
				break;
			case UPNPEXT_IP:
				use_ext_ip_addr = ary_options[i].value;
				break;
			case UPNPLISTENING_IP:
				lan_addr = (struct lan_addr_s *) malloc(sizeof(struct lan_addr_s));
				if (lan_addr == NULL)
				{
					fprintf(stderr, "malloc(sizeof(struct lan_addr_s)): %m");
					break;
				}
				if(parselanaddr(lan_addr, ary_options[i].value) != 0)
				{
					fprintf(stderr, "can't parse \"%s\" as valid lan address\n", ary_options[i].value);
					free(lan_addr);
					break;
				}
				LIST_INSERT_HEAD(&lan_addrs, lan_addr, list);
				break;
			case UPNPPORT:
				v->port = atoi(ary_options[i].value);
				break;
			case UPNPBITRATE_UP:
				upstream_bitrate = strtoul(ary_options[i].value, 0, 0);
				break;
			case UPNPBITRATE_DOWN:
				downstream_bitrate = strtoul(ary_options[i].value, 0, 0);
				break;
			case UPNPPRESENTATIONURL:
				presurl = ary_options[i].value;
				break;
			case UPNPFRIENDLY_NAME:
				strncpy(friendly_name, ary_options[i].value, FRIENDLY_NAME_MAX_LEN);
				friendly_name[FRIENDLY_NAME_MAX_LEN-1] = '\0';
				break;
#ifdef USE_NETFILTER
			case UPNPFORWARDCHAIN:
				miniupnpd_forward_chain = ary_options[i].value;
				break;
			case UPNPNATCHAIN:
				miniupnpd_nat_chain = ary_options[i].value;
				break;
#endif
			case UPNPNOTIFY_INTERVAL:
				v->notify_interval = atoi(ary_options[i].value);
				break;
			case UPNPSYSTEM_UPTIME:
				if(strcmp(ary_options[i].value, "yes") == 0)
					SETFLAG(SYSUPTIMEMASK);	/*sysuptime = 1;*/
				break;
			case UPNPPACKET_LOG:
				if(strcmp(ary_options[i].value, "yes") == 0)
					SETFLAG(LOGPACKETSMASK);	/*logpackets = 1;*/
				break;
			case UPNPUUID:
				strncpy(uuidvalue+5, ary_options[i].value,
				        strlen(uuidvalue+5) + 1);
				break;
			case UPNPSERIAL:
				strncpy(serialnumber, ary_options[i].value, SERIALNUMBER_MAX_LEN);
				serialnumber[SERIALNUMBER_MAX_LEN-1] = '\0';
				break;
			case UPNPMODEL_NUMBER:
				strncpy(modelnumber, ary_options[i].value, MODELNUMBER_MAX_LEN);
				modelnumber[MODELNUMBER_MAX_LEN-1] = '\0';
				break;
			case UPNPCLEANTHRESHOLD:
				v->clean_ruleset_threshold = atoi(ary_options[i].value);
				break;
			case UPNPCLEANINTERVAL:
				v->clean_ruleset_interval = atoi(ary_options[i].value);
				break;
#ifdef USE_PF
			case UPNPANCHOR:
				anchor_name = ary_options[i].value;
				break;
			case UPNPQUEUE:
				queue = ary_options[i].value;
				break;
			case UPNPTAG:
				tag = ary_options[i].value;
				break;
#endif
#ifdef ENABLE_NATPMP
			case UPNPENABLENATPMP:
				if(strcmp(ary_options[i].value, "yes") == 0)
					SETFLAG(ENABLENATPMPMASK);	/*enablenatpmp = 1;*/
				else
					if(atoi(ary_options[i].value))
						SETFLAG(ENABLENATPMPMASK);
					/*enablenatpmp = atoi(ary_options[i].value);*/
				break;
#endif
#ifdef PF_ENABLE_FILTER_RULES
			case UPNPQUICKRULES:
				if(strcmp(ary_options[i].value, "no") == 0)
					SETFLAG(PFNOQUICKRULESMASK);
				break;
#endif
			case UPNPENABLE:
				if(strcmp(ary_options[i].value, "yes") != 0)
					CLEARFLAG(ENABLEUPNPMASK);
				break;
			case UPNPSECUREMODE:
				if(strcmp(ary_options[i].value, "yes") == 0)
					SETFLAG(SECUREMODEMASK);
				break;
#ifdef ENABLE_LEASEFILE
			case UPNPLEASEFILE:
				lease_file = ary_options[i].value;
				break;
#endif
			case UPNPMINISSDPDSOCKET:
				minissdpdsocketpath = ary_options[i].value;
				break;
			default:
				fprintf(stderr, "Unknown option in file %s\n",
				        optionsfile);
			}
		}
	}

	/* command line arguments processing */
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]!='-')
		{
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
		}
		else switch(argv[i][1])
		{
		case 'o':
			if(i+1 < argc)
				use_ext_ip_addr = argv[++i];
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 't':
			if(i+1 < argc)
				v->notify_interval = atoi(argv[++i]);
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'u':
			if(i+1 < argc)
				strncpy(uuidvalue+5, argv[++i], strlen(uuidvalue+5) + 1);
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 's':
			if(i+1 < argc)
				strncpy(serialnumber, argv[++i], SERIALNUMBER_MAX_LEN);
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			serialnumber[SERIALNUMBER_MAX_LEN-1] = '\0';
			break;
#ifdef ENABLE_WSC_SERVICE
		case 'M':
			if(i+1 < argc)
				strncpy(modelnumber, argv[++i], MODELNUMBER_MAX_LEN);
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			modelnumber[MODELNUMBER_MAX_LEN-1] = '\0';
			break;
		case 'm':
			i++;	/* discarding */
			break;
#else
		case 'm':
			if(i+1 < argc)
				strncpy(modelnumber, argv[++i], MODELNUMBER_MAX_LEN);
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			modelnumber[MODELNUMBER_MAX_LEN-1] = '\0';
			break;
#endif /* ENABLE_WSC_SERVICE */
#ifdef ENABLE_NATPMP
		case 'N':
			/*enablenatpmp = 1;*/
			SETFLAG(ENABLENATPMPMASK);
			break;
#endif
#ifdef ENABLE_WSC_SERVICE
		case 'G':
			/*enable IGD service*/
			SETFLAG(ENABLEUPNPMASK);
			break;
		case 'n':
			i++;
			break;
#endif /* ENABLE_WSC_SERVICE */

		case 'U':
			/*sysuptime = 1;*/
			SETFLAG(SYSUPTIMEMASK);
			break;
		/*case 'l':
			logfilename = argv[++i];
			break;*/
		case 'L':
			/*logpackets = 1;*/
			SETFLAG(LOGPACKETSMASK);
			break;
		case 'S':
			SETFLAG(SECUREMODEMASK);
			break;
		case 'i':
			if(i+1 < argc)
				ext_if_name = argv[++i];
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
#ifdef USE_PF
		case 'q':
			if(i+1 < argc)
				queue = argv[++i];
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'T':
			if(i+1 < argc)
				tag = argv[++i];
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
#endif
		case 'p':
			if(i+1 < argc)
				v->port = atoi(argv[++i]);
			else
#ifdef ENABLE_NFQUEUE
		case 'Q':
			if(i+1<argc)
			{
				nfqueue = atoi(argv[++i]);
			}
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'n':
			if (i+1 < argc) {
				i++;
				if(n_nfqix < MAX_LAN_ADDR) {
					nfqix[n_nfqix++] = if_nametoindex(argv[i]);
				} else {
					fprintf(stderr,"Too many nfq interfaces. Ignoring %s\n", argv[i]);
				}
			} else {
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			}
			break;
#endif
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'P':
			if(i+1 < argc)
				pidfilename = argv[++i];
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'd':
			debug_flag = 1;
			break;
		case 'w':
			if(i+1 < argc)
				presurl = argv[++i];
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'B':
			if(i+2<argc)
			{
				downstream_bitrate = strtoul(argv[++i], 0, 0);
				upstream_bitrate = strtoul(argv[++i], 0, 0);
			}
			else
				fprintf(stderr, "Option -%c takes two arguments.\n", argv[i][1]);
			break;
		case 'a':
			if(i+1 < argc)
			{
				i++;
				lan_addr = (struct lan_addr_s *) malloc(sizeof(struct lan_addr_s));
				if (lan_addr == NULL)
				{
					fprintf(stderr, "malloc(sizeof(struct lan_addr_s)): %m");
					break;
				}
				if(parselanaddr(lan_addr, argv[i]) != 0)
				{
					fprintf(stderr, "can't parse \"%s\" as valid lan address\n", argv[i]);
					free(lan_addr);
					break;
				}
				/* check if we already have this address */
				for(lan_addr2 = lan_addrs.lh_first; lan_addr2 != NULL; lan_addr2 = lan_addr2->list.le_next)
				{
					if (0 == strncmp(lan_addr2->str, lan_addr->str, 15))
						break;
				}
				if (lan_addr2 == NULL)
					LIST_INSERT_HEAD(&lan_addrs, lan_addr, list);
			}
			else
				fprintf(stderr, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'f':
			i++;	/* discarding, the config file is already read */
			break;
		default:
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
		}
	}
	if(!ext_if_name || !lan_addrs.lh_first)
	{
		/* bad configuration */
		goto print_usage;
	}

	if(debug_flag)
	{
		pid = getpid();
	}
	else
	{
#ifdef ENABLE_WSC_SERVICE
		wsc_debug_level = RT_DBG_OFF;
#endif /* ENABLE_WSC_SERVICE */

#ifdef USE_DAEMON
		if(daemon(0, 0)<0) {
			perror("daemon()");
		}
		pid = getpid();
#else
		pid = daemonize();
#endif
	}

	openlog_option = LOG_CONS;
	if(debug_flag)
	{
		openlog_option |= LOG_PERROR;	/* also log on stderr */
	}

	openlog(LOG_UPNP, openlog_option, LOG_MINIUPNPD);

	if(!debug_flag)
	{
		/* speed things up and ignore LOG_INFO and LOG_DEBUG */
		setlogmask(LOG_UPTO(LOG_ERR));
	}

	if(checkforrunning(pidfilename) < 0)
	{
		syslog(LOG_ERR, "MiniUPnPd is already running. EXITING");
		return 1;
	}

	set_startup_time(GETFLAG(SYSUPTIMEMASK));

	/* presentation url */
	if(presurl)
	{
		strncpy(presentationurl, presurl, PRESENTATIONURL_MAX_LEN);
		presentationurl[PRESENTATIONURL_MAX_LEN-1] = '\0';
	}
	else
	{
		snprintf(presentationurl, PRESENTATIONURL_MAX_LEN,
		         "http://%s/", lan_addrs.lh_first->str);
		         /*"http://%s:%d/", lan_addrs.lh_first->str, 80);*/
	}

	/* set signal handler */
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = sigterm;

	if (sigaction(SIGTERM, &sa, NULL))
	{
		syslog(LOG_ERR, "Failed to set %s handler. EXITING", "SIGTERM");
		return 1;
	}
	if (sigaction(SIGINT, &sa, NULL))
	{
		syslog(LOG_ERR, "Failed to set %s handler. EXITING", "SIGINT");
		return 1;
	}

	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		syslog(LOG_ERR, "Failed to ignore SIGPIPE signals");
	}

	sa.sa_handler = sigusr1;
	if (sigaction(SIGUSR1, &sa, NULL))
	{
		syslog(LOG_NOTICE, "Failed to set %s handler", "SIGUSR1");
	}

	if(init_redirect() < 0)
	{
		syslog(LOG_ERR, "Failed to init redirection engine. EXITING");
		return 1;
	}

	writepidfile(pidfilename, pid);

#ifdef ENABLE_LEASEFILE
	/*remove(lease_file);*/
	syslog(LOG_INFO, "Reloading rules from lease file");
	reload_from_lease_file();
#endif

	return 0;
print_usage:
	fprintf(stderr, "Usage:\n\t"
	        "%s [-f config_file] [-i ext_ifname] [-o ext_ip]\n"
#ifndef ENABLE_NATPMP
			"\t\t[-a listening_ip] [-p port] [-d] [-L] [-U] [-S]\n"
#else
			"\t\t[-a listening_ip] [-p port] [-d] [-L] [-U] [-S] [-N]\n"
#endif
			/*"[-l logfile] " not functionnal */
#ifdef ENABLE_WSC_SERVICE
				"\t\t[-u uuid] [-s serial] [-M model_number] \n"
#else
				"\t\t[-u uuid] [-s serial] [-m model_number] \n"
#endif /* ENABLE_WSC_SERVICE */
			"\t\t[-u uuid] [-s serial] [-m model_number] \n"
			"\t\t[-t notify_interval] [-P pid_filename]\n"
			"\t\t[-B down up] [-w url]\n"
#ifdef USE_PF
                        "\t\t[-q queue] [-T tag]\n"
#endif
#ifdef ENABLE_NFQUEUE
                        "\t\t[-Q queue] [-n name]\n"
#endif
	        "\nNotes:\n\tThere can be one or several listening_ips.\n"
	        "\tNotify interval is in seconds. Default is 30 seconds.\n"
			"\tDefault pid file is '%s'.\n"
			"\tDefault config file is '%s'.\n"
			"\tWith -d miniupnpd will run as a standard program.\n"
			"\t-L sets packet log in pf and ipf on.\n"
			"\t-S sets \"secure\" mode : clients can only add mappings to their own ip\n"
			"\t-U causes miniupnpd to report system uptime instead "
			"of daemon uptime.\n"
#ifdef ENABLE_NATPMP
			"\t-N enable NAT-PMP functionality.\n"
#endif
			"\t-B sets bitrates reported by daemon in bits per second.\n"
			"\t-w sets the presentation url. Default is http address on port 80\n"
#ifdef USE_PF
			"\t-q sets the ALTQ queue in pf.\n"
			"\t-T sets the tag name in pf.\n"
#endif
#ifdef ENABLE_WSC_SERVICE
				"\t-G turns on IGD Device service.\n"
#endif /* ENABLE_WSC_SERVICE */

#ifdef ENABLE_NFQUEUE
                        "\t-Q sets the queue number that is used by NFQUEUE.\n"
                        "\t-n sets the name of the interface(s) that packets will arrive on.\n"
#endif
			"\t-h prints this help and quits.\n"
	        "", argv[0], pidfilename, DEFAULT_CONFIG);
	return 1;
}

/* === main === */
/* process HTTP or SSDP requests */
int
main(int argc, char * * argv)
{
	int i;
	int shttpl = -1;	/* socket for HTTP */
	int sudp = -1;		/* IP v4 socket for receiving SSDP */
#ifdef ENABLE_IPV6
	int sudpv6 = -1;	/* IP v6 socket for receiving SSDP */
#endif
#ifdef ENABLE_NATPMP
	int * snatpmp = NULL;
#endif
#ifdef ENABLE_NFQUEUE
	int nfqh = -1;
#endif
#ifdef USE_IFACEWATCHER
	int sifacewatcher = -1;
#endif

#ifdef ENABLE_WSC_SERVICE
	int shttpwpsl = -1;
#endif /* ENABLE_WSC_SERVICE */

	int * snotify = NULL;
	int addr_count;
	LIST_HEAD(httplisthead, upnphttp) upnphttphead;
	struct upnphttp * e = 0;
	struct upnphttp * next;
	fd_set readset;	/* for select() */
	fd_set writeset;
	struct timeval timeout, timeofday, lasttimeofday = {0, 0};
	int max_fd = -1;
#ifdef USE_MINIUPNPDCTL
	int sctl = -1;
	LIST_HEAD(ctlstructhead, ctlelem) ctllisthead;
	struct ctlelem * ectl;
	struct ctlelem * ectlnext;
#endif
	struct runtime_vars v;
	/* variables used for the unused-rule cleanup process */
	struct rule_state * rule_list = 0;
	struct timeval checktime = {0, 0};
	struct lan_addr_s * lan_addr;

	if(init(argc, argv, &v) != 0)
		return 1;
	/* count lan addrs */
	addr_count = 0;
	for(lan_addr = lan_addrs.lh_first; lan_addr != NULL; lan_addr = lan_addr->list.le_next)
		addr_count++;
	if(addr_count > 0) {
		snotify = calloc(addr_count, sizeof(int));
	}
#ifdef ENABLE_NATPMP
	if(addr_count > 0) {
		snatpmp = malloc(addr_count * sizeof(int));
		for(i = 0; i < addr_count; i++)
			snatpmp[i] = -1;
	}
#endif

#ifdef ENABLE_WSC_SERVICE
	if (GETFLAG(ENABLEWPSMASK))
	{
		if (WPSInit(argc, argv))
		{
	    	DBGPRINTF(RT_DBG_OFF, "WPSInit() ok !\n");
		}
		else
		{
	    	DBGPRINTF(RT_DBG_OFF, "WPSInit() failed !\n");
			CLEARFLAG(ENABLEWPSMASK);
			return 1;
		}
	}
#endif /* ENABLE_WSC_SERVICE */

#ifdef HW_NAT_EBL
#if !defined (CONFIG_HNAT_V2)
	struct ac_args args;
	args.vid=1; //LAN
	SetAcEntry(&args, AC_ADD_VLAN_UL_ENTRY); //LAN->DUT (in)
	SetAcEntry(&args, AC_ADD_VLAN_DL_ENTRY); //DUT->LAN (out)
	args.vid=2;
	SetAcEntry(&args, AC_ADD_VLAN_UL_ENTRY); //WAN->DUT (in)
	SetAcEntry(&args, AC_ADD_VLAN_DL_ENTRY); //DUT->WAN (out)
#endif
#endif // HW_NAT_EBL //

	LIST_INIT(&upnphttphead);
#ifdef USE_MINIUPNPDCTL
	LIST_INIT(&ctllisthead);
#endif

	if(
#ifdef ENABLE_NATPMP
        !GETFLAG(ENABLENATPMPMASK) &&
#endif
#ifdef ENABLE_WSC_SERVICE
        !GETFLAG(ENABLEWPSMASK) &&
#endif /* ENABLE_WSC_SERVICE */
        !GETFLAG(ENABLEUPNPMASK) ) {
		syslog(LOG_ERR, "Why did you run me anyway?");
		return 0;
	}

	if(GETFLAG(ENABLEUPNPMASK)
#ifdef ENABLE_WSC_SERVICE
		|| GETFLAG(ENABLEWPSMASK)
#endif
		)
	{

#ifdef ENABLE_WSC_SERVICE
		if (GETFLAG(ENABLEWPSMASK))
		{
			/* open socket for HTTP connections. Listen on the 1st LAN address */
			shttpwpsl = OpenAndConfHTTPSocket(WPS_PORT);
			if(shttpwpsl < 0)
			{
				syslog(LOG_ERR, "Failed to open socket for WPS. EXITING");
				return 1;
			}
			syslog(LOG_ERR, "WPS listening on port %d", WPS_PORT);

			{
				int flags;

				flags = fcntl(netlink_sock, F_GETFL, 0); 
				if (flags == -1) 
			    	return 1; 
			    fcntl(netlink_sock, F_SETFL, flags | O_NONBLOCK);
			}		
		}
#endif /* ENABLE_WSC_SERVICE */

		if(GETFLAG(ENABLEUPNPMASK))
		{
		/* open socket for HTTP connections. Listen on the 1st LAN address */
		shttpl = OpenAndConfHTTPSocket((v.port > 0) ? v.port : 0);
		if(shttpl < 0)
		{
			syslog(LOG_ERR, "Failed to open socket for HTTP. EXITING");
			return 1;
		}
		if(v.port <= 0) {
			struct sockaddr_in sockinfo;
			socklen_t len = sizeof(struct sockaddr_in);
			if (getsockname(shttpl, (struct sockaddr *)&sockinfo, &len) < 0) {
				syslog(LOG_ERR, "getsockname(): %m");
				return 1;
			}
			v.port = ntohs(sockinfo.sin_port);
		}
		syslog(LOG_NOTICE, "HTTP listening on port %d", v.port);
#ifdef ENABLE_IPV6
		if(find_ipv6_addr(NULL, ipv6_addr_for_http_with_brackets, sizeof(ipv6_addr_for_http_with_brackets)) > 0) {
			syslog(LOG_NOTICE, "HTTP IPv6 address given to control points : %s",
			       ipv6_addr_for_http_with_brackets);
		} else {
			memcpy(ipv6_addr_for_http_with_brackets, "[::1]", 6);
			syslog(LOG_WARNING, "no HTTP IPv6 address");
		}
#endif
		}

		/* open socket for SSDP connections */
		sudp = OpenAndConfSSDPReceiveSocket(0);
		if(sudp < 0)
		{
			syslog(LOG_INFO, "Failed to open socket for receiving SSDP. Trying to use MiniSSDPd");
			if(SubmitServicesToMiniSSDPD(lan_addrs.lh_first->str, v.port) < 0) {
				syslog(LOG_ERR, "Failed to connect to MiniSSDPd. EXITING");
				return 1;
			}
		}
#ifdef ENABLE_IPV6
		sudpv6 = OpenAndConfSSDPReceiveSocket(1);
		if(sudpv6 < 0)
		{
			syslog(LOG_INFO, "Failed to open socket for receiving SSDP (IP v6).");
		}
#endif

		/* open socket for sending notifications */
		if(OpenAndConfSSDPNotifySockets(snotify) < 0)
		{
			syslog(LOG_ERR, "Failed to open sockets for sending SSDP notify "
		                "messages. EXITING");
			return 1;
		}

#ifdef USE_IFACEWATCHER
		/* open socket for kernel notifications about new network interfaces */
		if (sudp >= 0)
		{
			sifacewatcher = OpenAndConfInterfaceWatchSocket();
			if (sifacewatcher < 0)
			{
				syslog(LOG_ERR, "Failed to open socket for receiving network interface notifications");
			}
		}
#endif
	}

#ifdef ENABLE_NATPMP
	/* open socket for NAT PMP traffic */
	if(GETFLAG(ENABLENATPMPMASK))
	{
		if(OpenAndConfNATPMPSockets(snatpmp) < 0)
		{
			syslog(LOG_ERR, "Failed to open sockets for NAT PMP.");
		} else {
			syslog(LOG_NOTICE, "Listening for NAT-PMP traffic on port %u",
			       NATPMP_PORT);
		}
#if 0
		ScanNATPMPforExpiration();
#endif
	}
#endif

	/* for miniupnpdctl */
#ifdef USE_MINIUPNPDCTL
	sctl = OpenAndConfCtlUnixSocket("/var/run/miniupnpd.ctl");
#endif

#ifdef ENABLE_WSC_SERVICE
	if (GETFLAG(ENABLEWPSMASK))
	{
		DescDump();
	}
#endif /* ENABLE_WSC_SERVICE */

#ifdef ENABLE_NFQUEUE
	if ( nfqueue != -1 && n_nfqix > 0) {
		nfqh = OpenAndConfNFqueue();
		if(nfqh < 0) {
			syslog(LOG_ERR, "Failed to open fd for NFQUEUE.");
			return 1;
		} else {
			syslog(LOG_NOTICE, "Opened NFQUEUE %d",nfqueue);
		}
	}
#endif
	/* main loop */
	while(!quitting)
	{
		/* Correct startup_time if it was set with a RTC close to 0 */
		if((startup_time<60*60*24) && (time(NULL)>60*60*24))
		{
			set_startup_time(GETFLAG(SYSUPTIMEMASK));
		}
		/* send public address change notifications if needed */
		if(should_send_public_address_change_notif)
		{
			syslog(LOG_DEBUG, "should send external iface address change notification(s)");
#ifdef ENABLE_NATPMP
			if(GETFLAG(ENABLENATPMPMASK))
				SendNATPMPPublicAddressChangeNotification(snatpmp, addr_count);
#endif
#ifdef ENABLE_EVENTS
			if(GETFLAG(ENABLEUPNPMASK))
			{
				upnp_event_var_change_notify(EWanIPC);
			}
#endif
			should_send_public_address_change_notif = 0;
		}
		/* Check if we need to send SSDP NOTIFY messages and do it if
		 * needed */
		if(gettimeofday(&timeofday, 0) < 0)
		{
			syslog(LOG_ERR, "gettimeofday(): %m");
			timeout.tv_sec = v.notify_interval;
			timeout.tv_usec = 0;
		}
		else
		{
			/* the comparaison is not very precise but who cares ? */
			if(timeofday.tv_sec >= (lasttimeofday.tv_sec + v.notify_interval))
			{
#ifdef ENABLE_WSC_SERVICE
				{
					// how often to verify the timeouts, in seconds
					// default value is equal to notify interval
					WscMsgQVerifyTimeouts(v.notify_interval);
				}
#endif

				if (GETFLAG(ENABLEUPNPMASK))
					SendSSDPNotifies2(snotify,
				                  (unsigned short)v.port,
				                  v.notify_interval << 1);
#ifdef ENABLE_WSC_SERVICE
				if (GETFLAG(ENABLEWPSMASK))
				{
					WSC_SendSSDPNotifies2(snotify,
				                  (unsigned short)WPS_PORT,
				                  v.notify_interval << 1);
				}
#endif /* ENABLE_WSC_SERVICE */

				memcpy(&lasttimeofday, &timeofday, sizeof(struct timeval));
				timeout.tv_sec = v.notify_interval;
				timeout.tv_usec = 0;
			}
			else
			{
				timeout.tv_sec = lasttimeofday.tv_sec + v.notify_interval
				                 - timeofday.tv_sec;
				if(timeofday.tv_usec > lasttimeofday.tv_usec)
				{
					timeout.tv_usec = 1000000 + lasttimeofday.tv_usec
					                  - timeofday.tv_usec;
					timeout.tv_sec--;
				}
				else
				{
					timeout.tv_usec = lasttimeofday.tv_usec - timeofday.tv_usec;
				}
			}
		}
		/* remove unused rules */
		if( v.clean_ruleset_interval
		  && (timeofday.tv_sec >= checktime.tv_sec + v.clean_ruleset_interval))
		{
			if(rule_list)
			{
				remove_unused_rules(rule_list);
				rule_list = NULL;
			}
			else
			{
				rule_list = get_upnp_rules_state_list(v.clean_ruleset_threshold);
			}
			memcpy(&checktime, &timeofday, sizeof(struct timeval));
		}
		/* Remove expired port mappings, based on UPnP IGD LeaseDuration
		 * or NAT-PMP lifetime) */
		if(nextruletoclean_timestamp
		  && (timeofday.tv_sec >= nextruletoclean_timestamp))
		{
			syslog(LOG_DEBUG, "cleaning expired Port Mappings");
			get_upnp_rules_state_list(0);
		}
		if(nextruletoclean_timestamp
		  && timeout.tv_sec >= (nextruletoclean_timestamp - timeofday.tv_sec))
		{
			timeout.tv_sec = nextruletoclean_timestamp - timeofday.tv_sec;
			timeout.tv_usec = 0;
			syslog(LOG_DEBUG, "setting timeout to %u sec",
			       (unsigned)timeout.tv_sec);
		}
#ifdef ENABLE_NATPMP
#if 0
		/* Remove expired NAT-PMP mappings */
		while(nextnatpmptoclean_timestamp
		     && (timeofday.tv_sec >= nextnatpmptoclean_timestamp + startup_time))
		{
			/*syslog(LOG_DEBUG, "cleaning expired NAT-PMP mappings");*/
			if(CleanExpiredNATPMP() < 0) {
				syslog(LOG_ERR, "CleanExpiredNATPMP() failed");
				break;
			}
		}
		if(nextnatpmptoclean_timestamp
		  && timeout.tv_sec >= (nextnatpmptoclean_timestamp + startup_time - timeofday.tv_sec))
		{
			/*syslog(LOG_DEBUG, "setting timeout to %d sec",
			       nextnatpmptoclean_timestamp + startup_time - timeofday.tv_sec);*/
			timeout.tv_sec = nextnatpmptoclean_timestamp + startup_time - timeofday.tv_sec;
			timeout.tv_usec = 0;
		}
#endif
#endif

		/* select open sockets (SSDP, HTTP listen, and all HTTP soap sockets) */
		FD_ZERO(&readset);
		FD_ZERO(&writeset);

		if (sudp >= 0)
		{
			FD_SET(sudp, &readset);
			max_fd = MAX( max_fd, sudp);
#ifdef USE_IFACEWATCHER
			if (sifacewatcher >= 0)
			{
				FD_SET(sifacewatcher, &readset);
				max_fd = MAX(max_fd, sifacewatcher);
			}
#endif
		}
		if (shttpl >= 0)
		{
			FD_SET(shttpl, &readset);
			max_fd = MAX( max_fd, shttpl);
		}

#ifdef ENABLE_WSC_SERVICE
		if(shttpwpsl >= 0)
		{
			FD_SET(shttpwpsl, &readset);
			max_fd = MAX(max_fd, shttpwpsl);
		}
		
		if(netlink_sock >= 0)
		{
			FD_SET(netlink_sock, &readset);
			max_fd = MAX(max_fd, netlink_sock);
		}
#endif /* ENABLE_WSC_SERVICE */

#ifdef ENABLE_IPV6
		if (sudpv6 >= 0)
		{
			FD_SET(sudpv6, &readset);
			max_fd = MAX( max_fd, sudpv6);
		}
#endif

#ifdef ENABLE_NFQUEUE
		if (nfqh >= 0)
		{
			FD_SET(nfqh, &readset);
			max_fd = MAX( max_fd, nfqh);
		}
#endif

		i = 0;	/* active HTTP connections count */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if(e->socket >= 0)
			{
				if(e->state <= EWaitingForHttpContent)
					FD_SET(e->socket, &readset);
				else if(e->state == ESendingAndClosing)
					FD_SET(e->socket, &writeset);
				else
					continue;
				max_fd = MAX(max_fd, e->socket);
				i++;
			}
		}
		/* for debug */
#ifdef DEBUG
		if(i > 1)
		{
			syslog(LOG_DEBUG, "%d active incoming HTTP connections", i);
		}
#endif
#ifdef ENABLE_NATPMP
		for(i=0; i<addr_count; i++) {
			if(snatpmp[i] >= 0) {
				FD_SET(snatpmp[i], &readset);
				max_fd = MAX( max_fd, snatpmp[i]);
			}
		}
#endif
#ifdef USE_MINIUPNPDCTL
		if(sctl >= 0) {
			FD_SET(sctl, &readset);
			max_fd = MAX( max_fd, sctl);
		}

		for(ectl = ctllisthead.lh_first; ectl; ectl = ectl->entries.le_next)
		{
			if(ectl->socket >= 0) {
				FD_SET(ectl->socket, &readset);
				max_fd = MAX( max_fd, ectl->socket);
			}
		}
#endif

#ifdef ENABLE_EVENTS
		upnpevents_selectfds(&readset, &writeset, &max_fd);
#endif

		if(select(max_fd+1, &readset, &writeset, 0, &timeout) < 0)
		{
			if(quitting) goto shutdown;
			if(errno == EINTR) continue; /* interrupted by a signal, start again */
			syslog(LOG_ERR, "select(all): %m");
			syslog(LOG_ERR, "Failed to select open sockets. EXITING");
			return 1;	/* very serious cause of error */
		}
#ifdef USE_MINIUPNPDCTL
		for(ectl = ctllisthead.lh_first; ectl;)
		{
			ectlnext =  ectl->entries.le_next;
			if((ectl->socket >= 0) && FD_ISSET(ectl->socket, &readset))
			{
				char buf[256];
				int l;
				l = read(ectl->socket, buf, sizeof(buf));
				if(l > 0)
				{
					/*write(ectl->socket, buf, l);*/
					write_command_line(ectl->socket, argc, argv);
					write_option_list(ectl->socket);
					write_permlist(ectl->socket, upnppermlist, num_upnpperm);
					write_upnphttp_details(ectl->socket, upnphttphead.lh_first);
					write_ctlsockets_list(ectl->socket, ctllisthead.lh_first);
					write_ruleset_details(ectl->socket);
#ifdef ENABLE_EVENTS
					write_events_details(ectl->socket);
#endif
					/* close the socket */
					close(ectl->socket);
					ectl->socket = -1;
				}
				else
				{
					close(ectl->socket);
					ectl->socket = -1;
				}
			}
			if(ectl->socket < 0)
			{
				LIST_REMOVE(ectl, entries);
				free(ectl);
			}
			ectl = ectlnext;
		}
		if((sctl >= 0) && FD_ISSET(sctl, &readset))
		{
			int s;
			struct sockaddr_un clientname;
			struct ctlelem * tmp;
			socklen_t clientnamelen = sizeof(struct sockaddr_un);
			//syslog(LOG_DEBUG, "sctl!");
			s = accept(sctl, (struct sockaddr *)&clientname,
			           &clientnamelen);
			syslog(LOG_DEBUG, "sctl! : '%s'", clientname.sun_path);
			tmp = malloc(sizeof(struct ctlelem));
			tmp->socket = s;
			LIST_INSERT_HEAD(&ctllisthead, tmp, entries);
		}
#endif
#ifdef ENABLE_EVENTS
		upnpevents_processfds(&readset, &writeset);
#endif
#ifdef ENABLE_NATPMP
		/* process NAT-PMP packets */
		for(i=0; i<addr_count; i++)
		{
			if((snatpmp[i] >= 0) && FD_ISSET(snatpmp[i], &readset))
			{
				ProcessIncomingNATPMPPacket(snatpmp[i]);
			}
		}
#endif
		/* process SSDP packets */
		if(sudp >= 0 && FD_ISSET(sudp, &readset))
		{
			/*syslog(LOG_INFO, "Received UDP Packet");*/
			ProcessSSDPRequest(sudp, (unsigned short)v.port);
		}
#ifdef ENABLE_IPV6
		if(sudpv6 >= 0 && FD_ISSET(sudpv6, &readset))
		{
			syslog(LOG_INFO, "Received UDP Packet (IPv6)");
			ProcessSSDPRequest(sudpv6, (unsigned short)v.port);
		}
#endif
#ifdef USE_IFACEWATCHER
		/* process kernel notifications */
		if (sifacewatcher >= 0 && FD_ISSET(sifacewatcher, &readset))
			ProcessInterfaceWatchNotify(sifacewatcher);
#endif

		/* process active HTTP connections */
		/* LIST_FOREACH macro is not available under linux */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if(e->socket >= 0)
			{
				if((FD_ISSET(e->socket, &readset) ||
				   FD_ISSET(e->socket, &writeset))
#ifdef ENABLE_WSC_SERVICE
				    && (e->state != DO_NOTHING_IN_MAIN_LOOP)
#endif
				   )
				{
					Process_upnphttp(e);
				}
			}
		}

#ifdef ENABLE_WSC_SERVICE
		if (GETFLAG(ENABLEWPSMASK))
		{
			/* process incoming HTTP connections */
			if(shttpwpsl >= 0 && FD_ISSET(shttpwpsl, &readset))
			{
				int shttpwps;
				socklen_t clientnamelen;
				struct sockaddr_in clientname;
				clientnamelen = sizeof(struct sockaddr_in);
				shttpwps = accept(shttpwpsl, (struct sockaddr *)&clientname, &clientnamelen);
				if (shttpwps<0)
				{
					syslog(LOG_ERR, "accept(shttpwpsl): %m");
				}
				else
				{
					struct upnphttp * tmp = 0;
					/* Create a new upnphttp object and add it to
					 * the active upnphttp object list */
					tmp = New_upnphttp(shttpwps);
					if(tmp)
					{
						tmp->clientaddr = clientname.sin_addr;
						LIST_INSERT_HEAD(&upnphttphead, tmp, entries);
					}
					else
					{
						syslog(LOG_ERR, "New_upnphttp() failed");
						close(shttpwps);
						shttpwps = -1;
					}
				}
			}

			if(FD_ISSET(netlink_sock, &readset))
			{
				DBGPRINTF(RT_DBG_LOUD, "(%s):netlink socket data is available now.\n", __FUNCTION__);
				wscNLSockRecv(netlink_sock);
			}
		}
#endif /* ENABLE_WSC_SERVICE */
		
		/* process incoming HTTP connections */
		if(shttpl >= 0 && FD_ISSET(shttpl, &readset))
		{
			int shttp;
			socklen_t clientnamelen;
#ifdef ENABLE_IPV6
			struct sockaddr_storage clientname;
			clientnamelen = sizeof(struct sockaddr_storage);
#else
			struct sockaddr_in clientname;
			clientnamelen = sizeof(struct sockaddr_in);
#endif
			shttp = accept(shttpl, (struct sockaddr *)&clientname, &clientnamelen);
			if(shttp<0)
			{
				/* ignore EAGAIN, EWOULDBLOCK, EINTR, we just try again later */
				if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
					syslog(LOG_ERR, "accept(http): %m");
			}
			else
			{
				struct upnphttp * tmp = 0;
				char addr_str[64];

				sockaddr_to_string((struct sockaddr *)&clientname, addr_str, sizeof(addr_str));
				/* Create a new upnphttp object and add it to
				 * the active upnphttp object list */
				tmp = New_upnphttp(shttp);
				if(tmp)
				{
#ifdef ENABLE_IPV6
					if(clientname.ss_family == AF_INET)
					{
						tmp->clientaddr = ((struct sockaddr_in *)&clientname)->sin_addr;
					}
					else if(clientname.ss_family == AF_INET6)
					{
						struct sockaddr_in6 * addr = (struct sockaddr_in6 *)&clientname;
						if(IN6_IS_ADDR_V4MAPPED(&addr->sin6_addr))
						{
							memcpy(&tmp->clientaddr,
							       &addr->sin6_addr.s6_addr[12],
							       4);
						}
						else
						{
							tmp->ipv6 = 1;
							memcpy(&tmp->clientaddr_v6,
							       &addr->sin6_addr,
							       sizeof(struct in6_addr));
						}
					}
#else
					tmp->clientaddr = clientname.sin_addr;
#endif
					LIST_INSERT_HEAD(&upnphttphead, tmp, entries);
				}
				else
				{
					syslog(LOG_ERR, "New_upnphttp() failed");
					close(shttp);
				}
			}
		}
#ifdef ENABLE_NFQUEUE
		/* process NFQ packets */
		if(nfqh >= 0 && FD_ISSET(nfqh, &readset))
		{
			/* syslog(LOG_INFO, "Received NFQUEUE Packet");*/
			ProcessNFQUEUE(nfqh);
		}
#endif
		/* delete finished HTTP connections */
		for(e = upnphttphead.lh_first; e != NULL; )
		{
			next = e->entries.le_next;
			if(e->state >= EToDelete)
			{
				LIST_REMOVE(e, entries);
				Delete_upnphttp(e);
			}
			e = next;
		}

	}	/* end of main loop */

shutdown:
	/* close out open sockets */
	while(upnphttphead.lh_first != NULL)
	{
		e = upnphttphead.lh_first;
		LIST_REMOVE(e, entries);
		Delete_upnphttp(e);
	}

	if (sudp >= 0) close(sudp);
	if (shttpl >= 0) close(shttpl);
#ifdef ENABLE_IPV6
	if (sudpv6 >= 0) close(sudpv6);
#endif
#ifdef USE_IFACEWATCHER
	if(sifacewatcher >= 0) close(sifacewatcher);
#endif
#ifdef ENABLE_NATPMP
	for(i=0; i<addr_count; i++) {
		if(snatpmp[i]>=0)
		{
			close(snatpmp[i]);
			snatpmp[i] = -1;
		}
	}
#endif
#ifdef ENABLE_WSC_SERVICE
	if(shttpwpsl >= 0)
	{
		close(shttpwpsl);
		shttpwpsl = -1;
	}
	if(netlink_sock >= 0)
	{
		close(netlink_sock);
		netlink_sock = -1;
	}
#endif /* ENABLE_WSC_SERVICE */
#ifdef USE_MINIUPNPDCTL
	if(sctl>=0)
	{
		close(sctl);
		sctl = -1;
		if(unlink("/var/run/miniupnpd.ctl") < 0)
		{
			syslog(LOG_ERR, "unlink() %m");
		}
	}
#endif

#ifdef ENABLE_WSC_SERVICE
	if (GETFLAG(ENABLEWPSMASK))
	{
		if(WSC_SendSSDPGoodbye(snotify, addr_count) < 0)
		{
			syslog(LOG_ERR, "Failed to broadcast good-bye notifications for WSC");
		}
	}

	if (!(GETFLAG(ENABLEUPNPMASK)))
	{
		for(i=0; i<addr_count; i++)
			close(snotify[i]);
	}
#endif /* ENABLE_WSC_SERVICE */

	/*if(SendSSDPGoodbye(snotify, v.n_lan_addr) < 0)*/
	if (GETFLAG(ENABLEUPNPMASK))
	{
		if(SendSSDPGoodbye(snotify, addr_count) < 0)
		{
			syslog(LOG_ERR, "Failed to broadcast good-bye notifications");
		}
		for(i=0; i<addr_count; i++)
			close(snotify[i]);
	}

	if(unlink(pidfilename) < 0)
	{
		syslog(LOG_ERR, "Failed to remove pidfile %s: %m", pidfilename);
	}

	/* delete lists */
	while(lan_addrs.lh_first != NULL)
	{
		lan_addr = lan_addrs.lh_first;
		LIST_REMOVE(lan_addrs.lh_first, list);
		free(lan_addr);
	}

#ifdef ENABLE_NATPMP
	free(snatpmp);
#endif
	free(snotify);
	closelog();
	freeoptions();
#ifdef ENABLE_WSC_SERVICE
	CLEARFLAG(ENABLEWPSMASK);
    DBGPRINTF(RT_DBG_INFO, "wscSystemStop()...\n");
    wscSystemStop();

#if 1 // def MULTIPLE_CARD_SUPPORT
	unlink(pid_file_path);
#else
	unlink(DEFAULT_PID_FILE_PATH);
#endif // MULTIPLE_CARD_SUPPORT //
#endif /* ENABLE_WSC_SERVICE */
	return 0;
}

