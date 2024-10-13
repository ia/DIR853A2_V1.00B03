#include <stdio.h>
#include "nvram.h"
#include "notify_rc.h"
#include <time.h>

typedef enum {
	NONE = 0,
	WIRELESS_2G = 0x1,
	WIRELESS_5G = 0x2,
	ONETOUCH = 0x4,
	SYS = 0x8,
	UPLINK = 0xF,
	ALL = 0xFF
} FLAG_T;

// get string value of the specified node from DXml
char *dxmlGetStr(char *buf, const char *format, ...);
// get encoded string value of the specified node from DXml
char *dxmlGetEncryStr(char *buf, const char *format, ...);
// set string value of the specified node to DXml
int dxmlSetStr(char *buf, const char *format, ...);
// get integer value of the specified node from DXml
signed int dxmlGetInt(const char *format, ...);
// set integer value of the specified node to DXml
int dxmlSetInt(int set_value, const char *format, ...);
// delete a specified node in DXml
int dxmlDel(const char *format, ...);

#define xget_str 		dxmlGetStr
#define xget_encrystr	dxmlGetEncryStr
#define xset_str		dxmlSetStr
#define xset_file		dxmlSetFile
#define xget_int		dxmlGetInt
#define xset_int		dxmlSetInt
#define xget_row		dxmlGetRow
#define xget_noe		dxmlGetNoe
#define xdel 			dxmlDel

#define PATH_RtErConnStaInfo		"/runtime/easyroaming/connstainfo"


#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

char *strstrip(char *s)
{
	size_t size;
	char *end;
	size = strlen(s);
	if (!size)
		return s;
	end = s + size - 1;
	while (end >= s && isspace(*end))
		end--;
	*(end + 1) = '\0';
	while (*s && isspace(*s))
		s++;
	return s;
}

int get_mac(char *ifname, char* mac)
{
	int sockfd;
	struct ifreq tmp;   

	if (ifname == NULL || mac == NULL)
		return -1;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0)
	{
		perror("get_mac: create socket fail\n");
		return -1;
	}

	memset(&tmp,0,sizeof(struct ifreq));
	strncpy(tmp.ifr_name,ifname,sizeof(tmp.ifr_name)-1);
	if( (ioctl(sockfd,SIOCGIFHWADDR,&tmp)) < 0 )
	{
		printf("get_mac: ioctl error\n");
		close(sockfd);
		return -1;
	}

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)tmp.ifr_hwaddr.sa_data[0],
            (unsigned char)tmp.ifr_hwaddr.sa_data[1],
            (unsigned char)tmp.ifr_hwaddr.sa_data[2],
            (unsigned char)tmp.ifr_hwaddr.sa_data[3],
            (unsigned char)tmp.ifr_hwaddr.sa_data[4],
            (unsigned char)tmp.ifr_hwaddr.sa_data[5]
            );

	close(sockfd);
	
	return 0;
}

char *get_list_value(const char *list, int index)
{
	static char tmp[256];
	char *p = NULL;
	int i = 0;

	strncpy(tmp, list, 255);
	p = strtok(tmp, ";");
	while (p) {
		if (i == index)
			break;
		else {
			p = strtok(NULL, ";");
			i++;
		}
	}
	if (p)
		return p;
	else {
		strcpy(tmp, "");	
		return tmp;
	}
	
}

static char *strlwr(char *str)
{
	char *p = str;

	#define lowercase(ch) ((ch >= 'A' && (ch) <= 'Z') ? (ch) - ('A' - 'a') : (ch))
	
	while (*p != '\n' && *p != '\r') {
		*p = lowercase(*p);
		p++;
	}

	return str;
}

void load_nvram_to_dxml(FLAG_T flag)
{
	char val[256];


	if (strcpy(val, nvram_get(RT2860_NVRAM, "lan0_ipaddr")) && strlen(val) > 0)
		dxmlSetStr(val, "/lan/ip");
	sprintf(val, "%d", time((time_t*)NULL));
	dxmlSetStr(val, "/runtime/onetouch/systime");
	if (strcpy(val, nvram_get(RT2860_NVRAM, "http_passwd")) /*&& strlen(val) > 0*/)	//can be blank
		dxmlSetStr(val, "/runtime/onetouch/password");

	memset(val, 0, sizeof(val));
	get_mac("br0",val);
	dxmlSetStr(val, "/fwupgradeinfo/devicemac");
	memset(val, 0, sizeof(val));
	get_mac("ra0",val);
	dxmlSetStr(val, "/onetouch/apcloninginfo/connmaster/mac");

	if (flag & ONETOUCH) { 
		if (strcpy(val,nvram_get(RT2860_NVRAM, "OneTouch_ConnmasterMAC")) && strlen(val) > 0)
			dxmlSetStr(val, "/onetouch/apcloninginfo/connmaster/mac");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "OneTouch_ConnmasterSeq")) && strlen(val) > 0)
			dxmlSetStr(val, "/onetouch/apcloninginfo/connmaster/seq");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "OneTouch_ConnmasterSeq5")) && strlen(val) > 0)
			dxmlSetStr(val, "/onetouch/apcloninginfo/connmaster/seq5");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "OneTouch_EasyroamingInfo")) && strlen(val) > 0)
			dxmlSetStr(val, "/onetouch/apcloninginfo/connmaster/easyroaminginfo");
	}

	if (flag & SYS) { 
		if (strcpy(val,nvram_get(RT2860_NVRAM, "OneTouch_FriendlyName")) && strlen(val) > 0)
			dxmlSetStr(val, "/sys/friendlyname");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "IsDefaultLogin")) && strlen(val) > 0)
			dxmlSetStr(val, "/sys/factorydefault");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "http_passwd")) /*&& strlen(val) > 0*/)	//can be blank
			dxmlSetStr(val, "/sys/user:1/password");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "router_disable")) && strlen(val) > 0) {
			if (!strcmp(val, "1"))
				dxmlSetStr("0", "/sys/routerenbl");
			else
				dxmlSetStr("3", "/sys/routerenbl");
		}
	}
	
	if (flag & WIRELESS_2G) {
		printf("@@@@ flag WIRELESS_2G !\n");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "Radio24On")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless2.4:1/wlenbl");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "SSID1")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless2.4:1/wirelessname");
		memset(val, 0, sizeof(val));
		get_mac("ra0",val);
		dxmlSetStr(val, "/lan/wireless2.4:1/wirelessbssid");

		if (strcpy(val,nvram_get(RT2860_NVRAM, "WPAPSK1")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless2.4:1/encryptionkey");

		if (strcpy(val,get_list_value(nvram_get(RT2860_NVRAM, "AuthMode"),0)) && strlen(val) > 0) {
			if (!strcmp(val, "WPAPSKWPA2PSK") || !strcmp(val, "WPAPSK") || !strcmp(val, "WPA2PSK") || !strcmp(val, "WPA1WPA2"))
				dxmlSetStr("WPAWPA2", "/lan/wireless2.4:1/encryptionmode");
			else if (!strcmp(val, "WPA"))
				dxmlSetStr("WPA", "/lan/wireless2.4:1/encryptionmode");
			else if (!strcmp(val, "WPA2"))
				dxmlSetStr("WPA2", "/lan/wireless2.4:1/encryptionmode");
			else if (!strcmp(val, "WEPAUTO")) {
				dxmlSetStr("WEP", "/lan/wireless2.4:1/encryptionmode");
				dxmlSetStr("AUTO", "/lan/wireless2.4:1/encryptiontype");
			}
			else if (!strcmp(val, "OPEN")) {
				printf("@@@@ set WIRELESS_2G AuthMode None!\n");
				dxmlSetStr("None", "/lan/wireless2.4:1/encryptionmode");
				dxmlSetStr("AUTO", "/lan/wireless2.4:1/encryptiontype");
				dxmlSetStr("", "/lan/wireless2.4:1/encryptionkey");
			}
			else if (!strcmp(val, "SHARED")) {
				dxmlSetStr("WEP", "/lan/wireless2.4:1/encryptionmode");
				dxmlSetStr("SHARED", "/lan/wireless2.4:1/encryptiontype");
			}
			else {
				dxmlSetStr("None", "/lan/wireless2.4:1/encryptionmode");
				dxmlSetStr("AUTO", "/lan/wireless2.4:1/encryptiontype");
				dxmlSetStr("", "/lan/wireless2.4:1/encryptionkey");
			}
			
		}
		if (strcpy(val,get_list_value(nvram_get(RT2860_NVRAM, "EncrypType"),0)) && strlen(val) > 0) {
			if (!strcmp(val, "TKIPAES"))
				dxmlSetStr("AUTO", "/lan/wireless2.4:1/encryptiontype");
			else if (!strcmp(val, "TKIP"))
				dxmlSetStr("TKIP", "/lan/wireless2.4:1/encryptiontype");
			else if (!strcmp(val, "AES"))
				dxmlSetStr("AES", "/lan/wireless2.4:1/encryptiontype");
			else if (!strcmp(val, "WEP")) {
				;
			}
			else
				dxmlSetStr("AUTO", "/lan/wireless2.4:1/encryptiontype");
		}

		if (strcpy(val,nvram_get(RT2860_NVRAM, "CountryCode")) && strlen(val) > 0) {
			if (!strcmp(val, "NA"))
				strcpy(val, "US");
			dxmlSetStr(val, "/lan/wireless2.4:1/country");
		}
		if (strcpy(val,nvram_get(RT2860_NVRAM, "Channel")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless2.4:1/channel");
		if (!strcmp(val, "0")) //0=auto
			dxmlSetInt(get_current_channel("ra0"), "/lan/wireless2.4:1/channelusing");
		else
			dxmlSetStr(val, "/lan/wireless2.4:1/channelusing");
		
	}
	
	if (flag & WIRELESS_5G) {
		printf("@@@@ flag WIRELESS_5G !\n");
		if (strcpy(val,nvram_get(RTDEV_NVRAM, "Radio58On")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless5:1/wlenbl");
		if (strcpy(val,nvram_get(RTDEV_NVRAM, "SSID1")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless5:1/wirelessname");
		memset(val, 0, sizeof(val));
		get_mac("rai0",val);
		dxmlSetStr(val, "/lan/wireless5:1/wirelessbssid");

		if (strcpy(val,nvram_get(RTDEV_NVRAM, "WPAPSK1")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless5:1/encryptionkey");

		if (strcpy(val,get_list_value(nvram_get(RTDEV_NVRAM, "AuthMode"),0)) && strlen(val) > 0) {
			if (!strcmp(val, "WPAPSKWPA2PSK") || !strcmp(val, "WPAPSK") || !strcmp(val, "WPA2PSK") || !strcmp(val, "WPA1WPA2"))
				dxmlSetStr("WPAWPA2", "/lan/wireless5:1/encryptionmode");
			else if (!strcmp(val, "WPA"))
				dxmlSetStr("WPA", "/lan/wireless5:1/encryptionmode");
			else if (!strcmp(val, "WPA2"))
				dxmlSetStr("WPA2", "/lan/wireless5:1/encryptionmode");
			else if (!strcmp(val, "WEPAUTO")) {
				dxmlSetStr("WEP", "/lan/wireless5:1/encryptionmode");
				dxmlSetStr("AUTO", "/lan/wireless5:1/encryptiontype");
			}
			else if (!strcmp(val, "OPEN")) {
				printf("@@@@ set WIRELESS_5G AuthMode None!\n");
				dxmlSetStr("None", "/lan/wireless5:1/encryptionmode");
				dxmlSetStr("AUTO", "/lan/wireless5:1/encryptiontype");
				dxmlSetStr("", "/lan/wireless5:1/encryptionkey");
			}
			else if (!strcmp(val, "SHARED")) {
				dxmlSetStr("WEP", "/lan/wireless5:1/encryptionmode");
				dxmlSetStr("SHARED", "/lan/wireless5:1/encryptiontype");
			}
			else {
				dxmlSetStr("None", "/lan/wireless5:1/encryptionmode");
				dxmlSetStr("AUTO", "/lan/wireless5:1/encryptiontype");
				dxmlSetStr("", "/lan/wireless5:1/encryptionkey");
			}
		}
		if (strcpy(val,get_list_value(nvram_get(RTDEV_NVRAM, "EncrypType"),0)) && strlen(val) > 0) {
			if (!strcmp(val, "TKIPAES"))
				dxmlSetStr("AUTO", "/lan/wireless5:1/encryptiontype");
			else if (!strcmp(val, "TKIP"))
				dxmlSetStr("TKIP", "/lan/wireless5:1/encryptiontype");
			else if (!strcmp(val, "AES"))
				dxmlSetStr("AES", "/lan/wireless5:1/encryptiontype");
			else if (!strcmp(val, "WEP")) {
				;
			}
			else
				dxmlSetStr("AUTO", "/lan/wireless5:1/encryptiontype");
		}
		
		if (strcpy(val,nvram_get(RTDEV_NVRAM, "CountryCode")) && strlen(val) > 0) {
			if (!strcmp(val, "NA"))
				strcpy(val, "US");
			dxmlSetStr(val, "/lan/wireless5:1/country");
		}
		if (strcpy(val,nvram_get(RTDEV_NVRAM, "Channel")) && strlen(val) > 0)
			dxmlSetStr(val, "/lan/wireless5:1/channel");
		if (!strcmp(val, "0")) //0=auto
			dxmlSetInt(get_current_channel("rai0"), "/lan/wireless5:1/channelusing");
		else
			dxmlSetStr(val, "/lan/wireless5:1/channelusing");
		
	}

	if (flag & UPLINK) {
		if (strcpy(val,nvram_get(RT2860_NVRAM, "OneTouch_HardwareType")) && strlen(val) > 0)
			dxmlSetStr(val, "/uplink/hardwaretype");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_proto")) && strlen(val) > 0)
			dxmlSetStr(val, "/uplink/conntype");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_pppoe_username")) && strlen(val) > 0)
			dxmlSetStr(val, "/uplink/loginusername");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_pppoe_passwd")) && strlen(val) > 0)
			dxmlSetStr(val, "/uplink/loginpassword");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_ipaddr")) /*&& strlen(val) > 0*/)
			dxmlSetStr(strstrip(val), "/uplink/ip");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_netmask")) /*&& strlen(val) > 0*/)
			dxmlSetStr(strstrip(val), "/uplink/netmask");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_gateway")) /*&& strlen(val) > 0*/)
			dxmlSetStr(strstrip(val), "/uplink/gateway");
		if (strcpy(val,nvram_get(RT2860_NVRAM, "wan_wan0_dns")) /*&& strlen(val) > 0*/)
			dxmlSetStr(strstrip(val), "/uplink/dns");
	}

}

void save_dxml_to_nvram(FLAG_T flag)
{
	char tmp[256], tmp2[256];
	char val[256];
	int wep_auth_mode;

	if (flag & ONETOUCH) {
		nvram_bufset(RT2860_NVRAM, "OneTouch_ConnmasterMAC", dxmlGetStr(tmp, "/onetouch/apcloninginfo/connmaster/mac"));
		nvram_bufset(RT2860_NVRAM, "OneTouch_ConnmasterSeq", dxmlGetStr(tmp, "/onetouch/apcloninginfo/connmaster/seq"));
		nvram_bufset(RT2860_NVRAM, "OneTouch_ConnmasterSeq5", dxmlGetStr(tmp, "/onetouch/apcloninginfo/connmaster/seq5"));
		nvram_bufset(RT2860_NVRAM, "OneTouch_EasyroamingInfo", dxmlGetStr(tmp, "/onetouch/apcloninginfo/connmaster/easyroaminginfo"));
	}
	
	if (flag & SYS) {
		nvram_bufset(RT2860_NVRAM, "OneTouch_FriendlyName", dxmlGetStr(tmp, "/sys/friendlyname"));
		nvram_bufset(RT2860_NVRAM, "IsDefaultLogin", dxmlGetStr(tmp, "/sys/factorydefault"));
		nvram_bufset(RT2860_NVRAM, "http_passwd", dxmlGetStr(tmp, "/sys/user:1/password"));		
		strcpy(val, dxmlGetStr(tmp, "/sys/routerenbl"));
		if (!strcmp(val, "0"))
			nvram_bufset(RT2860_NVRAM, "router_disable", "1");	
		else
		nvram_bufset(RT2860_NVRAM, "router_disable", "0");
	}
	
	if (flag & WIRELESS_2G) {		
		nvram_bufset(RT2860_NVRAM, "Radio24On", dxmlGetStr(tmp, "/lan/wireless2.4:1/wlenbl"));
		nvram_bufset(RT2860_NVRAM, "SSID1", dxmlGetStr(tmp, "/lan/wireless2.4:1/wirelessname"));
		//nvram_bufset(RT2860_NVRAM, "OneTouch_WirelessBSSID", dxmlGetStr(tmp, "/lan/wireless2.4:1/wirelessbssid"));

		strcpy(tmp2, dxmlGetStr(tmp, "/lan/wireless2.4:1/encryptiontype"));
		strcpy(tmp, get_list_value(nvram_get(RT2860_NVRAM, "EncrypType"),1));
		wep_auth_mode = 0;		
		if (!strcmp(tmp2, "TKIP"))	
			sprintf(val, "%s;%s", "TKIP", strlen(tmp)==0?"TKIP":tmp);
		else if (!strcmp(tmp2, "AES"))
			sprintf(val, "%s;%s", "AES", strlen(tmp)==0?"AES":tmp);
		else if (!strcmp(tmp2, "AUTO"))
			sprintf(val, "%s;%s", "TKIPAES", strlen(tmp)==0?"TKIPAES":tmp);
		else if (!strcmp(tmp2, "OPEN")) {
			sprintf(val, "%s;%s", "WEP", strlen(tmp)==0?"WEP":tmp);
			wep_auth_mode = 1;
		}
		else if (!strcmp(tmp2, "SHARED")) {
			sprintf(val, "%s;%s", "WEP", strlen(tmp)==0?"WEP":tmp);
			wep_auth_mode = 2;
		}
		else
			sprintf(val, "%s;%s", "NONE", strlen(tmp)==0?"NONE":tmp);
		nvram_bufset(RT2860_NVRAM, "EncrypType", val);

		strcpy(tmp2, dxmlGetStr(tmp, "/lan/wireless2.4:1/encryptionmode"));
		strcpy(tmp, get_list_value(nvram_get(RT2860_NVRAM, "AuthMode"),1));
		if (!strcmp(tmp2, "WPAWPA2"))
			sprintf(val, "%s;%s", "WPAPSKWPA2PSK", strlen(tmp)==0?"WPAPSKWPA2PSK":tmp);
		else if (!strcmp(tmp2, "WPA"))
			sprintf(val, "%s;%s", "WPAPSK", strlen(tmp)==0?"WPAPSK":tmp);
		else if (!strcmp(tmp2, "WPA2"))
			sprintf(val, "%s;%s", "WPA2PSK", strlen(tmp)==0?"WPA2PSK":tmp);
		else if (!strcmp(tmp2, "WEP")) {
			if (wep_auth_mode == 1) 
				sprintf(val, "%s;%s", "OPEN", strlen(tmp)==0?"OPEN":tmp);
			else if (wep_auth_mode == 2) 
				sprintf(val, "%s;%s", "SHARED", strlen(tmp)==0?"SHARED":tmp);
			else
				sprintf(val, "%s;%s", "WEPAUTO", strlen(tmp)==0?"WEPAUTO":tmp);
		}
		else
			sprintf(val, "%s;%s", "OPEN", strlen(tmp)==0?"OPEN":tmp);
		nvram_bufset(RT2860_NVRAM, "AuthMode", val);

		nvram_bufset(RT2860_NVRAM, "WPAPSK1", dxmlGetStr(tmp, "/lan/wireless2.4:1/encryptionkey"));
		//nvram_bufset(RT2860_NVRAM, "CountryCode", dxmlGetStr(tmp, "/lan/wireless2.4:1/country"));
		nvram_bufset(RT2860_NVRAM, "Channel", dxmlGetStr(tmp, "/lan/wireless2.4:1/channel"));
	}

	if (flag & WIRELESS_5G) {
		nvram_bufset(RTDEV_NVRAM, "Radio58On", dxmlGetStr(tmp, "/lan/wireless5:1/wlenbl"));
		nvram_bufset(RTDEV_NVRAM, "SSID1", dxmlGetStr(tmp, "/lan/wireless5:1/wirelessname"));
		//nvram_bufset(RTDEV_NVRAM, "OneTouch_WirelessBSSID", dxmlGetStr(tmp, "/lan/wireless5:1/wirelessbssid"));

		strcpy(tmp2, dxmlGetStr(tmp, "/lan/wireless5:1/encryptiontype"));
		strcpy(tmp, get_list_value(nvram_get(RTDEV_NVRAM, "EncrypType"),1));
		wep_auth_mode = 0;
		if (!strcmp(tmp2, "TKIP"))
			sprintf(val, "%s;%s", "TKIP", strlen(tmp)==0?"TKIP":tmp);
		else if (!strcmp(tmp2, "AES"))
			sprintf(val, "%s;%s", "AES", strlen(tmp)==0?"AES":tmp);
		else if (!strcmp(tmp2, "AUTO"))
			sprintf(val, "%s;%s", "TKIPAES", strlen(tmp)==0?"TKIPAES":tmp);
		else if (!strcmp(tmp2, "OPEN")) {
			sprintf(val, "%s;%s", "WEP", strlen(tmp)==0?"WEP":tmp);
			wep_auth_mode = 1;
		}
		else if (!strcmp(tmp2, "SHARED")) {
			sprintf(val, "%s;%s", "WEP", strlen(tmp)==0?"WEP":tmp);
			wep_auth_mode = 2;
		}
		else
			sprintf(val, "%s;%s", "NONE", strlen(tmp)==0?"NONE":tmp);
		nvram_bufset(RTDEV_NVRAM, "EncrypType", val);

		strcpy(tmp2, dxmlGetStr(tmp, "/lan/wireless5:1/encryptionmode"));
		strcpy(tmp, get_list_value(nvram_get(RTDEV_NVRAM, "AuthMode"),1));
		if (!strcmp(tmp2, "WPAWPA2"))
			sprintf(val, "%s;%s", "WPAPSKWPA2PSK", strlen(tmp)==0?"WPAPSKWPA2PSK":tmp);
		else if (!strcmp(tmp2, "WPA"))
			sprintf(val, "%s;%s", "WPAPSK", strlen(tmp)==0?"WPAPSK":tmp);
		else if (!strcmp(tmp2, "WPA2"))
			sprintf(val, "%s;%s", "WPA2PSK", strlen(tmp)==0?"WPA2PSK":tmp);
		else if (!strcmp(tmp2, "WEP")) {
			if (wep_auth_mode == 1) 
				sprintf(val, "%s;%s", "OPEN", strlen(tmp)==0?"OPEN":tmp);
			else if (wep_auth_mode == 2) 
				sprintf(val, "%s;%s", "SHARED", strlen(tmp)==0?"SHARED":tmp);
			else
				sprintf(val, "%s;%s", "WEPAUTO", strlen(tmp)==0?"WEPAUTO":tmp);
		}
		else
			sprintf(val, "%s;%s", "OPEN", strlen(tmp)==0?"OPEN":tmp);
		nvram_bufset(RTDEV_NVRAM, "AuthMode", val);

		nvram_bufset(RTDEV_NVRAM, "WPAPSK1", dxmlGetStr(tmp, "/lan/wireless5:1/encryptionkey"));
		//nvram_bufset(RTDEV_NVRAM, "CountryCode", dxmlGetStr(tmp, "/lan/wireless5:1/country"));
		nvram_bufset(RTDEV_NVRAM, "Channel", dxmlGetStr(tmp, "/lan/wireless5:1/channel"));

		nvram_commit(RTDEV_NVRAM);

		strcpy(tmp,get_list_value(nvram_get(RT2860_NVRAM, "EncrypType"),0));
		strcpy(tmp2,get_list_value(nvram_get(RT2860_NVRAM, "AuthMode"),0));
		if (strcmp(nvram_get(RT2860_NVRAM, "SSID1"),nvram_get(RTDEV_NVRAM, "SSID1"))
			|| strcmp(nvram_get(RT2860_NVRAM, "WPAPSK1"),nvram_get(RTDEV_NVRAM, "WPAPSK1"))			
			|| strcmp(tmp, get_list_value(nvram_get(RTDEV_NVRAM, "EncrypType"),0))
			|| strcmp(tmp2, get_list_value(nvram_get(RTDEV_NVRAM, "AuthMode"),0))
			) 
		{
			nvram_bufset(RT2860_NVRAM, "BandSteering", "0");
		}
	}
		
	if (flag & UPLINK) {
		nvram_bufset(RT2860_NVRAM, "OneTouch_HardwareType", dxmlGetStr(tmp, "/uplink/hardwaretype"));

		dxmlGetStr(tmp, "/uplink/conntype");
		if (!strcmp(tmp, "PPPoE")) {
			nvram_bufset(RT2860_NVRAM, "wan_wan0_dns_manual", "0");
			nvram_bufset(RT2860_NVRAM, "wan_wan0_proto", "pppoe");

			nvram_bufset(RT2860_NVRAM, "wan_wan0_pppoe_username", dxmlGetStr(tmp, "/uplink/loginusername"));
			nvram_bufset(RT2860_NVRAM, "wan_wan0_pppoe_passwd", dxmlGetStr(tmp, "/uplink/loginpassword"));	
		}
		else if (!strcmp(tmp, "Static")) {
			nvram_bufset(RT2860_NVRAM, "wan_wan0_dns_manual", "1");
			nvram_bufset(RT2860_NVRAM, "wan_wan0_proto", "static");
			
			nvram_bufset(RT2860_NVRAM, "wan_wan0_ipaddr", dxmlGetStr(tmp, "/uplink/ip"));
			nvram_bufset(RT2860_NVRAM, "wan_wan0_netmask", dxmlGetStr(tmp, "/uplink/netmask"));
			nvram_bufset(RT2860_NVRAM, "wan_wan0_gateway", dxmlGetStr(tmp, "/uplink/gateway"));
			nvram_bufset(RT2860_NVRAM, "wan_wan0_dns", dxmlGetStr(tmp, "/uplink/dns"));
		}
		else {
			nvram_bufset(RT2860_NVRAM, "wan_wan0_dns_manual", "0");
			nvram_bufset(RT2860_NVRAM, "wan_wan0_proto", "dhcp");
		}	
	}

	nvram_commit(RT2860_NVRAM);
}

#include "wireless.h"
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define RTPRIV_IOCTL_SET (SIOCIWFIRSTPRIV + 0x02)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT (SIOCIWFIRSTPRIV + 0x1F)
#define RTPRIV_IOCTL_GSITESURVEY (SIOCIWFIRSTPRIV + 0x0D)
#define MAX_NUMBER_OF_MAC 75
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef unsigned int UINT32;
typedef unsigned short USHORT;
typedef short SHORT;
typedef union _HTTRANSMIT_SETTING {
	struct {
		USHORT MCS:6;
		USHORT ldpc:1;
		USHORT BW:2;
		USHORT ShortGI:1;
		USHORT STBC:1;
		USHORT eTxBF:1;
		USHORT iTxBF:1;
		USHORT MODE:3;
	} field;
	USHORT word;
} HTTRANSMIT_SETTING, *PHTTRANSMIT_SETTING;
typedef struct _RT_802_11_MAC_ENTRY {
	UCHAR ApIdx;
	UCHAR Addr[6];
	UCHAR Aid;
	UCHAR Psm;      /* 0:PWR_ACTIVE, 1:PWR_SAVE */
	UCHAR MimoPs;       /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	CHAR AvgRssi0;
	CHAR AvgRssi1;
	CHAR AvgRssi2;
	UINT32 ConnectedTime;
	HTTRANSMIT_SETTING TxRate;
	UINT32 LastRxRate;
	SHORT StreamSnr[3];             /* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
	SHORT SoundingRespSnr[3];           /* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
} RT_802_11_MAC_ENTRY, *PRT_802_11_MAC_ENTRY;
typedef struct _RT_802_11_MAC_TABLE {
	ULONG Num;
	RT_802_11_MAC_ENTRY Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE;
struct dlinkreq {
	RT_802_11_MAC_TABLE data;
};
int get_sta_info(char *ifname)
{
	int skfd, i;
	struct iwreq wrq;
	struct dlinkreq dreq;
	RT_802_11_MAC_TABLE *p;


	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd == -1)
		return -1;

	memset(&dreq, 0, sizeof(struct dlinkreq));
	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);

	wrq.u.data.pointer = (void *)&dreq;
	wrq.u.data.length = (sizeof(struct dlinkreq));

	if (ioctl(skfd, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &wrq) >= 0)
	{
		p = (RT_802_11_MAC_TABLE *)&dreq;
		printf("STA Num=%d\n", p->Num);
		for (i=0; i<16; i++) {			
			printf("--> sta=%02x:%02x:%02x:%02x:%02x:%02x  ", p->Entry[i].Addr[0],
															  p->Entry[i].Addr[1],
															  p->Entry[i].Addr[2],
															  p->Entry[i].Addr[3],
															  p->Entry[i].Addr[4],
															  p->Entry[i].Addr[5]);
			printf("rssi=%d\n", MAX(MAX(p->Entry[i].AvgRssi0, p->Entry[i].AvgRssi1), p->Entry[i].AvgRssi2));
		}
	} else {
		close(skfd);
		return -1;
	}

	close(skfd);
	return 0;
}

int check_sta_info(char *ifname, char *mac, int rssi)
{
	char xmac[128],xifname[128];
	char path[256];
	int sta_count = xget_int(PATH_RtErConnStaInfo"/sta#");
	int i;

	if (rssi == 0) {
		printf("!! drop invalid sta info: ifname=%s, mac=%s rssi=%d\n", ifname, mac, rssi);
		return -1;
	}
	
	for (i=0;i<sta_count;i++) {
		sprintf(path,PATH_RtErConnStaInfo"/sta:%d/mac", i+1);
		xget_str(xmac, path);
		sprintf(path,PATH_RtErConnStaInfo"/sta:%d/ifname", i+1);
		xget_str(xifname, path);
		if (!strcmp(mac, xmac) && !strcmp(ifname, xifname)) {
			printf("!! drop duplicated sta info: ifname=%s, mac=%s rssi=%d\n", ifname, mac, rssi);
			return -1;
		}
	}

	printf("starter: client sta checked - (%s %s %d)\n", ifname, mac, rssi);

	return 1;
}

int update_sta_info(char *ifname)
{
	int skfd, i, idx;
	struct iwreq wrq;
	struct dlinkreq dreq;
	RT_802_11_MAC_TABLE *p;
	char mac[256],path[256];
	int rssi;
	int base_idx;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd == -1)
		return -1;

	memset(&dreq, 0, sizeof(struct dlinkreq));
	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);

	wrq.u.data.pointer = (void *)&dreq;
	wrq.u.data.length = (sizeof(struct dlinkreq));

	if (ioctl(skfd, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &wrq) >= 0)
	{
		base_idx = xget_int(PATH_RtErConnStaInfo"/sta#") + 1;
		
		p = (RT_802_11_MAC_TABLE *)&dreq;
		idx = 0;
		for (i=0; i<p->Num; i++) {
			memset(mac,sizeof(mac),0);
			sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x", p->Entry[i].Addr[0],
														 p->Entry[i].Addr[1],
														 p->Entry[i].Addr[2],
														 p->Entry[i].Addr[3],
														 p->Entry[i].Addr[4],
														 p->Entry[i].Addr[5]);
			rssi = MAX(MAX(p->Entry[i].AvgRssi0, p->Entry[i].AvgRssi1), p->Entry[i].AvgRssi2);
			if (check_sta_info(ifname, mac, rssi)<0)
				continue;

			sprintf(path,PATH_RtErConnStaInfo"/sta:%d/mac", base_idx+idx);
			xset_str(mac, path);
			
			sprintf(path,PATH_RtErConnStaInfo"/sta:%d/rssi", base_idx+idx);			
			xset_int(rssi, path);

			sprintf(path,PATH_RtErConnStaInfo"/sta:%d/ifname", base_idx+idx);
			xset_str(ifname, path);
			
			idx++;
		}
		
	} else {
		close(skfd);
		return -1;
	}

	close(skfd);
	return 0;
}

typedef struct {
	unsigned int channel;
	unsigned int rssi;
	unsigned char ssid[33];
	unsigned char bssid[18];
	unsigned char security[33];
} SITE_SURVEY;
int do_site_survey(char *ifname)
{
	int skfd, i;
	struct iwreq wrq;
	char data[255];

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd == -1)
		return -1;
	
	memset(data, 0x00, 255);
	strcpy(data, "SiteSurvey=1");
	strcpy(wrq.ifr_name, ifname);
	wrq.u.data.length=strlen(data)+1;
	wrq.u.data.pointer=data;
	wrq.u.data.flags=0;
	if (ioctl(skfd, RTPRIV_IOCTL_SET, &wrq) < 0)
		perror("SiteSurvey fail!\n");

	close(skfd);
	return 0;
}

int update_site_survey(char *ifname)
{
	int skfd, i, j, k;
	struct iwreq wrq;
	SITE_SURVEY SiteSurvey[64];
	char data[4096];
	char path[256];
	int base_idx;
	char Line[64][128]={0};
	int apTotal = 0;


	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd == -1)
		return -1;

	memset(&wrq, 0, sizeof(wrq));
	memset(data, 0, sizeof(data));
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);	
	wrq.u.data.pointer = data;
	wrq.u.data.length = sizeof(data);
	wrq.u.data.flags = 0;

	if (ioctl(skfd, RTPRIV_IOCTL_GSITESURVEY, &wrq) >= 0)
	{
		if (wrq.u.data.length > 0)
		{
			char *pMsg = wrq.u.data.pointer;
			char *pToken = NULL;

			pMsg = strtok(pMsg, "\n");
			i = 0;
			while(pMsg) {
				strncpy(Line[i], pMsg, 128-1); 
				if (++i>=64)
					break;
				pMsg = strtok(NULL, "\n");		
			}
			apTotal = i-2;
			printf("apTotal=%d\n", apTotal);

			
			for (j=2;j<i;j++)
			{
				pToken = strtok(Line[j], " ");
				k = 0;
				while(pToken) {
					switch (k) {
						case 1:
							sscanf(pToken, "%d", (int *)&SiteSurvey[j-2].channel);
							printf("channel=%d\n",SiteSurvey[j-2].channel);
							break;
						case 2:
							strncpy(SiteSurvey[j-2].ssid, pToken, 32);
							printf("ssid=%s\n",SiteSurvey[j-2].ssid);
							break;
						case 3:
							strncpy(SiteSurvey[j-2].bssid, pToken, 17);
							printf("bssid=%s\n",SiteSurvey[j-2].bssid);
							break;
						case 4:
							strncpy(SiteSurvey[j-2].security, pToken, 32);
							printf("security=%s\n",SiteSurvey[j-2].security);
							break;
						case 5:
							sscanf(pToken, "%d", (int *)&SiteSurvey[j-2].rssi);
							printf("rssi=%d\n",SiteSurvey[j-2].rssi);
							break;
						default:
							break;
					}
					k++;
					pToken = strtok(NULL, " ");
				}
				printf("\n");
			}				
		}		
	} else {
		close(skfd);
		return -1;
	}

	base_idx = xget_int("/wirelesssitesurvey/site#") + 1;
				
	for (i=0; i<apTotal; i++) {

		sprintf(path,"/wirelesssitesurvey/site:%d/wirelesstype", i + base_idx);
		if (!strcmp(ifname, "ra0")) 	
			xset_str("2.4", path);
		else
			xset_str("5", path);
			
		sprintf(path,"/wirelesssitesurvey/site:%d/wirelessname", i + base_idx);
		xset_str(SiteSurvey[i].ssid, path);

		sprintf(path,"/wirelesssitesurvey/site:%d/bssid", i + base_idx);
		xset_str(SiteSurvey[i].bssid, path);

		sprintf(path,"/wirelesssitesurvey/site:%d/encryptionmode", i + base_idx);
		xset_str(SiteSurvey[i].security, path);
		
		sprintf(path,"/wirelesssitesurvey/site:%d/wirelessstrength", i + base_idx);
		xset_int(SiteSurvey[i].rssi, path);
	}

	close(skfd);
	return 0;

}

double
freq2float(const struct iw_freq *in)
{
	int i;
	double res = (double) in->m;
	for(i = 0; i < in->e; i++)
		res *= 10;
	return(res);
}

int get_current_channel(char *ifname)
{
	int sockfd;
	struct iwreq tmp;
	double freq;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0)
	{
		perror("get_channel: create socket fail\n");
		return -1;
	}
	
	memset(&tmp,0,sizeof(struct iwreq));
	strncpy(tmp.ifr_name,ifname,sizeof(tmp.ifr_name)-1);
	if( (ioctl(sockfd,SIOCGIWFREQ,&tmp)) < 0 )
	{
		printf("get_channel: ioctl error\n");
		close(sockfd);
		return -1;
	}

	freq = freq2float(&(tmp.u.freq));
		
	close(sockfd);
	printf("channed=%d\n", (int)freq);
	return (int)freq;
}


#define EASYROAMING_V2		0

void autozoing_starter()
{

	system("onetouch &");
	system("goahead &");
#if (EASYROAMING_V2)
	system("dxmlc -s /onetouch/easyroaming 0");
	system("easyroaming -m 1 -d 1 &");
#else
	system("dxmlc -s /onetouch/easyroaming 1");
#endif
	exit(0);
}

void starter_run(void)
{
	static int is_run = 0;
	int i;

	if (is_run)
		return;
	else
		is_run = 1;
		
	if (fork() > 0){
		//parent
		exit(0);
	}
	else {
		//child - postpone 40 seconds after device boot up.
		for (i=0;i<40;i++)
		{
			sleep(1);	
		}
		load_nvram_to_dxml(ALL);
		autozoing_starter();
	}
}

void starter_help(void)
{
	printf("Help:	starter \n");
	printf("			run up autozoing utilities.\n");
	printf("		starter [load|save] [all|wireless2g|wireless5g|onetouch|uplink|sys]\n");
	printf("			load - load from nvram to dxml.\n");
	printf("			save - save from dxml to nvram.\n");
	printf("		starter [restart] [wan|wlan2g|wlan5g]\n");

}

int main(int argc, char *argv[])
{
	//enable autozoing only "AU"
	if(strncmp("AU",nvram_safe_get("CountryCode"),2) != 0)
		return 0;
	
	switch (argc) {
		case 1:
			starter_run();
			break;
		case 2:
			if (!strcmp(argv[1], "load"))
				load_nvram_to_dxml(ALL);
			else if (!strcmp(argv[1], "save"))
				save_dxml_to_nvram(ALL);
			else if (!strcmp(argv[1], "getsta"))
				get_sta_info("ra0");
			else if (!strcmp(argv[1], "updatesta")) {
				if (xget_int(PATH_RtErConnStaInfo "/sta#")>0)
	    	        xdel(PATH_RtErConnStaInfo);
		        update_sta_info("ra0");
		        update_sta_info("rai0");
			}
			else if (!strcmp(argv[1], "dosurvey")) {
				do_site_survey("ra0");
			}
			else if (!strcmp(argv[1], "updatesurvey")) {
				if (xget_int("/wirelesssitesurvey/site#")>0)
		            xdel("/wirelesssitesurvey");
				do_site_survey("ra0");
				do_site_survey("rai0");
				sleep(4);
        		update_site_survey("ra0");
				update_site_survey("rai0");
				xset_int(0,"/uplink/extendersite/survey");
			}
			else if (!strcmp(argv[1], "get_mac")) {
				char mac[32];
				get_mac("br0", mac);
				printf("br0 mac=%s\n", mac);
				get_mac("ra0", mac);
				printf("ra0 mac=%s\n", mac);
			}
			else if (!strcmp(argv[1], "get_channel")) {
				get_current_channel("ra0");
				get_current_channel("rai0");
			}
			else
				starter_help();
			break;
		case 3:
			if (!strcmp(argv[1], "load")) {
				if (!strcmp(argv[2], "all"))
					load_nvram_to_dxml(ALL);
				else if (!strcmp(argv[2], "wireless2g"))
					load_nvram_to_dxml(WIRELESS_2G);
				else if (!strcmp(argv[2], "wireless5g"))
					load_nvram_to_dxml(WIRELESS_5G);
				else if (!strcmp(argv[2], "uplink"))
					load_nvram_to_dxml(UPLINK);
				else if (!strcmp(argv[2], "sys"))
					load_nvram_to_dxml(SYS);
				else if (!strcmp(argv[2], "onetouch"))
					load_nvram_to_dxml(ONETOUCH);
			}
			else if (!strcmp(argv[1], "save")) {
				if (!strcmp(argv[2], "all"))
					save_dxml_to_nvram(ALL);
				else if (!strcmp(argv[2], "wireless2g"))
					save_dxml_to_nvram(WIRELESS_2G);
				else if (!strcmp(argv[2], "wireless5g"))
					save_dxml_to_nvram(WIRELESS_5G);
				else if (!strcmp(argv[2], "uplink"))
					save_dxml_to_nvram(UPLINK);
				else if (!strcmp(argv[2], "sys"))
					save_dxml_to_nvram(SYS);
				else if (!strcmp(argv[2], "onetouch"))
					save_dxml_to_nvram(ONETOUCH);
				else if (!strcmp(argv[2], "device")) {
					save_dxml_to_nvram(ONETOUCH);
					save_dxml_to_nvram(SYS);
				}
			}
			else if (!strcmp(argv[1], "restart")) {
				if (!strcmp(argv[2], "wan")) {
					save_dxml_to_nvram(UPLINK);
					notify_rc("restart_wan");
				}
				else if (!strcmp(argv[2], "wlan2g")) {
					save_dxml_to_nvram(WIRELESS_2G);					
					system("killall -SIGUSR1 onetouch"); //notify onetouch
					system("sleep 5"); //wait 5 sec before wlan restart for onetouch to done the clone
					notify_rc("restart_wlan");
				}
				else if (!strcmp(argv[2], "wlan5g")) {
					save_dxml_to_nvram(WIRELESS_5G);
					system("killall -SIGUSR1 onetouch"); //notify onetouch
					system("sleep 5"); //wait 5 sec before wlan restart for onetouch to done the clone
					notify_rc("restart_wlan");
				}
			}			
			else
				starter_help();
			break;
		default:
			starter_help();
	}
		
	return 0;
}



