#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <assert.h>
#include "iwlib.h"
#include "mytool.h"
#include "platform.h"

#define DLINK_MAX_STA_COUNT     128
#define IS_MTK	1
#define IS_QCA	0

#if IS_MTK

#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT (SIOCIWFIRSTPRIV + 0x1F)
#define MAX_NUMBER_OF_MAC 128
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

#endif	// IS_MTK
#if IS_QCA

#define IEEE80211_IOCTL_DBGREQ		(SIOCIWFIRSTPRIV+24)
#define DLINK_CTRL_GETSTAINFO		0x84

#endif	// IS_QCA

struct dlinkreq {
#if IS_MTK
	RT_802_11_MAC_TABLE data;
#endif
#if IS_QCA
	unsigned char cmd;
	union {
		void *data_addr;
	} data;
#endif
};

typedef struct {
    unsigned char    mac[6];
    char      rssi;
    unsigned short   txrate;
    unsigned short   rxrate;
    unsigned long   uptime;
} sta_t;

typedef struct {
    unsigned char    sta_count;
    sta_t       sta[DLINK_MAX_STA_COUNT];
} stainfo_t;

int GetStaInfo(char *ifname, stainfo_t *stainfo)
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

#if IS_MTK
	if (ioctl(skfd, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &wrq) >= 0)
	{
		p = (RT_802_11_MAC_TABLE *)&dreq;
		if (p->Num < 0 || p->Num > DLINK_MAX_STA_COUNT)
			stainfo->sta_count = DLINK_MAX_STA_COUNT;
		else
			stainfo->sta_count = p->Num;
		for (i=0; i<stainfo->sta_count/*p->Num*/; i++) {
			memcpy(stainfo->sta[i].mac, p->Entry[i].Addr, 6);
			stainfo->sta[i].rssi = MAX(MAX(p->Entry[i].AvgRssi0, p->Entry[i].AvgRssi1), p->Entry[i].AvgRssi2);
			stainfo->sta[i].txrate = p->Entry[i].LastRxRate;
			stainfo->sta[i].rxrate = p->Entry[i].LastRxRate;
			stainfo->sta[i].uptime = p->Entry[i].ConnectedTime;
			MSG_DEBUG("%s(%s) MAC=" MACSTR ", rssi=%d, uptime=%lu\n", __func__, \
				ifname, MAC2STR(stainfo->sta[i].mac), stainfo->sta[i].rssi, stainfo->sta[i].uptime);
		}
	} else {
		close(skfd);
		return -1;
	}
#endif	// IS_MTK
#if IS_QCA
	dreq.cmd = DLINK_CTRL_GETSTAINFO;
	memset(stainfo, 0, sizeof(stainfo_t));
	dreq.data.data_addr = stainfo;
	if(ioctl(skfd, IEEE80211_IOCTL_DBGREQ, &wrq) < 0)
	{
		close(skfd);
		return -1;
	}
#endif	// IS_QCA

	close(skfd);
	return 0;
}

/*
@ifname: The interface you would like to gather information from
@stapool: A storage that feed by calling function which used to store data for using

To port the function Platform_Get_Connected_STA which response to gather RSSI of connected clients, you should check is similar feature provided in your target platform or not. If it is available, you may just need to gather information by shell script. If there is no such kind of feature, you may need to implement a suitable one to make client¡¦s RSSI can be gathered from kernel space. The interface and a Connected_STA_t data structure used to store gathering data should be feed to make this function work.
*/
int Platform_Get_Connected_STA(char *ifname, Connected_STA_t *stapool)
{
	int ret, i;
	stainfo_t stainfo;
	
	ret = GetStaInfo(ifname, &stainfo);
	if(!ret)
	{
		stapool->count = stainfo.sta_count;
		for(i = 0; i < stapool->count; i++)
		{
			memcpy(stapool->sta[i].mac, stainfo.sta[i].mac, 6);
#if IS_MTK
			stapool->sta[i].rssi = stainfo.sta[i].rssi;
#endif
#if IS_QCA
			stapool->sta[i].rssi = stainfo.sta[i].rssi - 95;
#endif
		}
	}
	
	return ret;
}

//myiw_get_priv_info is a modified iw_get_priv_info that uses customized memory management and free allocated memory when return is 0.
int myiw_get_priv_info(int skfd, const char *ifname, iwprivargs **ppriv)
{
	struct iwreq wrq;
	iwprivargs *priv = NULL;	/* Not allocated yet */
	int	maxpriv = 16;	/* Minimum for compatibility WE<13 */
	iwprivargs *newpriv;

	/* Some driver may return a very large number of ioctls. Some
	* others a very small number. We now use a dynamic allocation
	* of the array to satisfy everybody. Of course, as we don't know
	* in advance the size of the array, we try various increasing
	* sizes. Jean II */
	do
	{
		/* (Re)allocate the buffer */
		newpriv = myrealloc(priv, maxpriv * sizeof(priv[0]));
		if(newpriv == NULL)
		{
			fprintf(stderr, "%s: Allocation failed\n", __FUNCTION__);
			break;
		}
		priv = newpriv;

	/* Ask the driver if it's large enough */
	wrq.u.data.pointer = (caddr_t) priv;
	wrq.u.data.length = maxpriv;
	wrq.u.data.flags = 0;
	if(iw_get_ext(skfd, ifname, SIOCGIWPRIV, &wrq) >= 0)
	{
		/* Success. Pass the buffer by pointer */
		*ppriv = priv;
		/* Return the number of ioctls */
		if(wrq.u.data.length == 0)
			myfree(priv);
		return(wrq.u.data.length);
	}

	/* Only E2BIG means the buffer was too small, abort on other errors */
	if(errno != E2BIG)
	{
		/* Most likely "not supported". Don't barf. */
		break;
	}

	/* Failed. We probably need a bigger buffer. Check if the kernel
	* gave us any hints. */
	if(wrq.u.data.length > maxpriv)
		maxpriv = wrq.u.data.length;
	else
		maxpriv *= 2;
	}
	while(maxpriv < 1000);

	/* Cleanup */
	if(priv)
		myfree(priv);
	*ppriv = NULL;

	return(-1);
}

/*
@ifname: The interface that kicking client connected
@mac:: The MAC address of kicking client

The function Platform_Deauth_STA is used to deauthenticate a specific client. It uses command "kickmac" that QCA provides to complete this operation. 
*/
int Platform_Deauth_STA(char *ifname, unsigned char *mac)
{
#if IS_MTK
	int ret;
	char cmd[80] = {0};

	sprintf(cmd, "iwpriv %s set DisConnectSta=" MACSTR, ifname, MAC2STR(mac));
	ret = system(cmd);
	MSG_DEBUG("[%s]: %s, return %d\n", __func__, cmd, ret);

	return ret;
#endif	// IS_MTK
#if IS_QCA
	int skfd;
	iwprivargs *priv;
	struct iwreq wrq;
	unsigned char buffer[IFNAMSIZ];
	int ncmd, i;
	char macstr[18];

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd == -1)
		return -1;
	
	if((ncmd = myiw_get_priv_info(skfd, ifname, &priv)) <= 0)
	{
		MSG_DEBUG("%s not support private ioctl!\n", ifname);
		close(skfd);
		return -1;
	}
	
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
	wrq.u.data.length = 1;
	i = -1;

	while((++i < ncmd) && strcmp(priv[i].name, "kickmac"));
	if(i == ncmd)
	{
		MSG_DEBUG("%s doesn't support 'kickmac'!\n", ifname);
		goto exit;
	}
	if(strchr(mac, ':'))
		strcpy(macstr, mac);
	else
		sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	if(iw_in_addr(skfd, ifname, macstr, (struct sockaddr *)buffer) < 0)
	{
		MSG_DEBUG("invalid address\n");
		goto exit;
	}
	memcpy(wrq.u.name, buffer, IFNAMSIZ);
	if(ioctl(skfd, priv[i].cmd, &wrq) < 0)
	{
		MSG_DEBUG("%s doesn't accept 'kickmac'!\n", ifname);
		goto exit;
	}
	
	myfree(priv);
	close(skfd);
	return 0;
	
exit:
	myfree(priv);
	close(skfd);
	return -1;
#endif	// IS_QCA
}

/*
 * Platform dependent code.
 * This is the way how openwrt access wireless configuration.
 */
#define UCIBUF_SIZE 80
static char uci_ret[UCIBUF_SIZE];
static int uci_cmd(char *cmd)
{
	int ret = 0;
	char *p;
	FILE *fp;

	fp = popen(cmd, "r");
	memset(uci_ret, 0, UCIBUF_SIZE);

	if (fp) {
		fgets(uci_ret, UCIBUF_SIZE, fp);
		p = uci_ret;
		while(*p!=0) {
			if (*p=='\n') *p=0;
			p++;
		}
	} else {
		ret = -1;
	}

	pclose(fp);
	return ret;
}
#include "nvram.h"
extern const char *nvram_get(int index, char *name);
#define UCI_GET_WLANCFG "uci get wireless.@wifi-iface[%d].%s"
/*
@stapool: A storage that feed by calling function which used to store data for using
The function Platform_Get_Wireless_Cofnig which response to gather wireless configuration.
If the function is combined with cloning, this function is not required.
*/
int Platform_Get_Wireless_Cofnig(AP_Wireless_t *appool)
{
	int maxvap = 16;
	int idx;
	int is_wep=0, is_psk=0;
	char cmd[UCIBUF_SIZE] = {0};
	char tmp_enc[UCIBUF_SIZE] = {0};
	char wep_keyid[2] = {0};
	int wep_keylen, offset=0;

	memset(appool, 0, sizeof(AP_Wireless_t));
#if 0
	for (idx=0; idx<maxvap; idx++) {
		sprintf(cmd, UCI_GET_WLANCFG, idx, "ifname");
		if (uci_cmd(cmd)!=-1 && strlen(uci_ret)>0) {
			strcpy(appool->config[idx].ifname, uci_ret);
			if (strncmp(uci_ret, "rai", 3)==0)
				appool->config[idx].radio_type = PWIFI_RADIO_5G_1;
			else
				appool->config[idx].radio_type = PWIFI_RADIO_2G;
		} else {
			break;
		}

		sprintf(cmd, UCI_GET_WLANCFG, idx, "ssid");
		uci_cmd(cmd);
		strcpy(appool->config[idx].ssid, uci_ret);

		sprintf(cmd, UCI_GET_WLANCFG, idx, "key");
		uci_cmd(cmd);
		strcpy(appool->config[idx].password, uci_ret);

		sprintf(cmd, UCI_GET_WLANCFG, idx, "encryption");
		uci_cmd(cmd);
		strcpy(tmp_enc, uci_ret);
		if (strcmp(tmp_enc, "none")==0) {
			strcpy(appool->config[idx].security, "OPEN");
			appool->config[idx].password[0] = 0;
		} else if (strcmp(tmp_enc, "wep-open")==0) {
			strcpy(appool->config[idx].security, "WEP");
			appool->config[idx].wepAuth = PwepAuth_OPEN;
			is_wep = 1;
		} else if (strcmp(tmp_enc, "wep-shared")==0) {
			strcpy(appool->config[idx].security, "WEP");
			appool->config[idx].wepAuth = PwepAuth_SHAREDKEY;
			is_wep = 1;
		} else if (strncmp(tmp_enc, "psk-mixed", 9)==0) {
			strcpy(appool->config[idx].security, "WPA+WPA2");
			is_psk = 1;
		} else if (strncmp(tmp_enc, "psk2", 4)==0) {
			strcpy(appool->config[idx].security, "WPA2");
			is_psk = 1;
		} else if (strncmp(tmp_enc, "psk", 3)==0) {
			strcpy(appool->config[idx].security, "WPA");
			is_psk = 1;
		} else {
			assert(1);	//FIXME: if it happen
		}

		if (is_wep) {
			//get wep key id
			sprintf(cmd, UCI_GET_WLANCFG, idx, "key");
			uci_cmd(cmd);
			strncpy(wep_keyid, uci_ret, 1);

			//get wep key value
			sprintf(cmd, UCI_GET_WLANCFG "%s", idx, "key", wep_keyid);
			uci_cmd(cmd);

			wep_keylen = strlen(uci_ret);
			switch (wep_keylen) {
				case 7:		//shoud be 5 characters, but openWRT add extra 2 bytes
					appool->config[idx].wep_encryption = PwepEncryption_64;
					appool->config[idx].wep_keyformat = PwepKeyFormat_HEX;
					offset = 2;
					break;
				case 10:
					appool->config[idx].wep_encryption = PwepEncryption_64;
					appool->config[idx].wep_keyformat = PwepKeyFormat_HEX;
					break;
				case 15:	//shoud be 13 characters, but openWRT add extra 2 bytes
					appool->config[idx].wep_encryption = PwepEncryption_128;
					appool->config[idx].wep_keyformat = PwepKeyFormat_ASCII;
					offset = 2;
					break;
				case 26:
					appool->config[idx].wep_encryption = PwepEncryption_128;
					appool->config[idx].wep_keyformat = PwepKeyFormat_ASCII;
					break;
				default:
					assert(1);	//FIXME: if it happen
			}

			strcpy(appool->config[idx].password, uci_ret + offset);
		}

		if (is_psk) {
			if (strstr(tmp_enc, "tkip+ccmp"))
				appool->config[idx].encryption = Pencryption_AESTKIP;
			else if (strstr(tmp_enc, "tkip"))
				appool->config[idx].encryption = Pencryption_TKIP;
			else if (strstr(tmp_enc, "ccmp"))
				appool->config[idx].encryption = Pencryption_AES;
			else
				appool->config[idx].encryption = Pencryption_AES;	//MTK default using WPA2-PSK/AES, but config is only show "psk2"
		}
	}

	appool->count = idx;
#endif
	int nvram_block = RT2860_NVRAM;

	for (idx=0;idx<2;idx++)
	{
		if (idx==0) {
			strcpy(appool->config[idx].ifname, "ra0");
			appool->config[idx].radio_type = PWIFI_RADIO_2G;
			nvram_block = RT2860_NVRAM;
		}
		else {
			strcpy(appool->config[idx].ifname, "rai0");
			appool->config[idx].radio_type = PWIFI_RADIO_5G_1;
			nvram_block = RTDEV_NVRAM;
		}

		strncpy(appool->config[idx].ssid, nvram_get(nvram_block,"SSID1"), sizeof(appool->config[0].ssid)-1);
		strncpy(appool->config[idx].password, nvram_get(nvram_block,"WPAPSK1"), sizeof(appool->config[0].password)-1);
		strncpy(tmp_enc, nvram_get(nvram_block,"AuthMode"), sizeof(tmp_enc)-1);
		
		if (strncmp(tmp_enc, "OPEN", 4)==0) {
			strcpy(appool->config[idx].security, "OPEN");
			appool->config[idx].password[idx] = 0;
		} else if (strncmp(tmp_enc, "WEPAUTO", 7)==0) {
			strcpy(appool->config[idx].security, "WEP");
			appool->config[idx].wepAuth = PwepAuth_OPEN;
			is_wep = 1;
		} else if (strncmp(tmp_enc, "SHARED", 6)==0) {
			strcpy(appool->config[idx].security, "WEP");
			appool->config[idx].wepAuth = PwepAuth_SHAREDKEY;
			is_wep = 1;
		} else if (strncmp(tmp_enc, "WPAPSKWPA2PSK", 13)==0) {
			strcpy(appool->config[idx].security, "WPA+WPA2");
			is_psk = 1;
		} else if (strncmp(tmp_enc, "WPA2PSK", 7)==0) {
			strcpy(appool->config[idx].security, "WPA2");
			is_psk = 1;
		} else if (strncmp(tmp_enc, "WPAPSK", 6)==0) {
			strcpy(appool->config[idx].security, "WPA");
			is_psk = 1;
		} else {
			assert(1);	//FIXME: if it happen
		}

		if (is_wep) {
			//get wep key id
			strncpy(wep_keyid, nvram_get(nvram_block,"DefaultKeyID"), 1);

			//get wep key value
			if (!strcmp(wep_keyid, "1"))			
				strcpy(tmp_enc, nvram_get(nvram_block,"Key1"));
			else if (!strcmp(wep_keyid, "2"))
				strcpy(tmp_enc, nvram_get(nvram_block,"Key2"));
			else if (!strcmp(wep_keyid, "3"))
				strcpy(tmp_enc, nvram_get(nvram_block,"Key3"));
			else if (!strcmp(wep_keyid, "4"))
				strcpy(tmp_enc, nvram_get(nvram_block,"Key4"));

			wep_keylen = strlen(tmp_enc);
			
			switch (wep_keylen) {
				case 7: 	//shoud be 5 characters, but openWRT add extra 2 bytes
					appool->config[idx].wep_encryption = PwepEncryption_64;
					appool->config[idx].wep_keyformat = PwepKeyFormat_HEX;
					offset = 2;
					break;
				case 10:
					appool->config[idx].wep_encryption = PwepEncryption_64;
					appool->config[idx].wep_keyformat = PwepKeyFormat_HEX;
					break;
				case 15:	//shoud be 13 characters, but openWRT add extra 2 bytes
					appool->config[idx].wep_encryption = PwepEncryption_128;
					appool->config[idx].wep_keyformat = PwepKeyFormat_ASCII;
					offset = 2;
					break;
				case 26:
					appool->config[idx].wep_encryption = PwepEncryption_128;
					appool->config[idx].wep_keyformat = PwepKeyFormat_ASCII;
					break;
				default:
					assert(1);	//FIXME: if it happen
			}

			strcpy(appool->config[idx].password, tmp_enc + offset);
		}

		if (is_psk) {
			strcpy(tmp_enc, nvram_get(nvram_block,"EncrypType"));		
				
			if (strstr(tmp_enc, "TKIPAES"))
				appool->config[idx].encryption = Pencryption_AESTKIP;
			else if (strstr(tmp_enc, "TKIP"))
				appool->config[idx].encryption = Pencryption_TKIP;
			else if (strstr(tmp_enc, "AES"))
				appool->config[idx].encryption = Pencryption_AES;
			else
				appool->config[idx].encryption = Pencryption_AES;	//MTK default using WPA2-PSK/AES, but config is only show "psk2"
		}
	}

	appool->count = 2;


	return 0;
}
