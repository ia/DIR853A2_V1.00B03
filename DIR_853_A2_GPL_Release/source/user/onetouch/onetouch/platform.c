/****************************************************************************/
/*                                                                          */
/* PLATFORM.C                                                               */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines platform denpent APIs                               */
/*                                                                          */
/****************************************************************************/
#include <stdarg.h>
#include <assert.h>
#include "idxml_api.h"
#include "idxml.h"

#include "rss.h"
#include "xml_parser.h"
#include "platform.h"
#include <assert.h>

char *rssActionTypeStr[] = {"RssStartUp", "RssShutDown", "CloneToSlaves", "SlaveRegister",  
	"SlaveUnregister", "ConnectMaster", "DisconnectMaster", "?","?","?"};
char *rssActionStatStr[] = {"Before", "Done", "Exception", "?","?","?"};
char *rssExceptionTypeStr[] = {"", "CloneFail", "RejectAction", "ExpiredConnect", "?","?","?"};

#define EXTCALL_WanResync_UseSec           20
#define EXTCALL_WlanResync_UseSec          30
#define EXTCALL_Wlan5gResync_UseSec        30
#define EXTCALL_DeviceResync_UseSec        5
#define EXTCALL_CfgSaveToFlash_UseSec      5
#define EXTCALL_WlanSiteSurvey_UseSec      5
#define EXTCALL_FwUpgrade_UseSec           180
#define EXTCALL_UpdateStaRssi_UseSec       3
#define EXTCALL_DeauthSta_UseSec           1

#define DEVICE_Reboot_UseSec               90

// Todo: format print used to print out "NECESSARY" message to console
void msgPrintf(char *fmt,...)
{
	va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return; 
}

// Format print used to print out "DEBUG" message to console
// If you want to turn off all debug message, you can empty this function. 
void dbgPrintf(char *fmt,...)
{
#if 0  //close console debug
	va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return; 
#endif
}

char *uiDefaultUserID() {	return "admin";}
char *uiDefaultUserPW() {	return "admin";}

// Todo: Define the API use time (sec) after API return.
int get_ExternalCall_UseSec(int sync_id)
{
    int ws = 0, enbl;
    char buf[32];
    
    switch (sync_id) {
        case EXTCALL_WanResync:
            ws = EXTCALL_WanResync_UseSec;
            break;
        case EXTCALL_WlanResync:
            ws = EXTCALL_WlanResync_UseSec;
            break;
        case EXTCALL_Wlan5gResync:
            ws = EXTCALL_Wlan5gResync_UseSec;
            break;
        case EXTCALL_DeviceResync:
            ws = EXTCALL_DeviceResync_UseSec;
            // Todo
            /* if change routerenbl value, device need to reboot */
            // for example,
            // xget_str(buf, "/sys/routerenbl");
            // enbl = atoi(buf);
            // if compare_routerenbl_change() 
            //    ws += DEVICE_Reboot_UseSec;  
            break;
        case EXTCALL_CfgSaveToFlash:
            ws = EXTCALL_CfgSaveToFlash_UseSec;
            break;
        case EXTCALL_WlanSiteSurvey:
            ws = EXTCALL_WlanSiteSurvey_UseSec;
            break;
        case EXTCALL_FwUpgrade:
            ws = EXTCALL_FwUpgrade_UseSec;
            break;
        case EXTCALL_UpdateStaRssi:
            ws = EXTCALL_UpdateStaRssi_UseSec;
            break;
        case EXTCALL_DeauthSta:
            ws = EXTCALL_DeauthSta_UseSec;
            break;
		case EXTCALL_DataValidate:
		case EXTCALL_FwUpload:
			ws = 0;
			break;
        default:
	        LOG_WARNING("[%s]Unknow sync_id=%d\n",__FUNCTION__,sync_id);
            break;
    }
    return ws; 
}

#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN	(32-4)		/* Image Name Length		*/
#define PRODUCT_LEN  64
#define VERSION_LEN 16
typedef struct image_header {
	unsigned int	ih_magic;	/* Image Header Magic Number	*/
	unsigned int	ih_hcrc;	/* Image Header CRC Checksum	*/
	unsigned int	ih_time;	/* Image Creation Timestamp	*/
	unsigned int	ih_size;	/* Image Data Size		*/
	unsigned int	ih_load;	/* Data	 Load  Address		*/
	unsigned int	ih_ep;		/* Entry Point Address		*/
	unsigned int	ih_dcrc;	/* Image Data CRC Checksum	*/
	unsigned char		ih_os;		/* Operating System		*/
	unsigned char		ih_arch;	/* CPU architecture		*/
	unsigned char		ih_type;	/* Image Type			*/
	unsigned char		ih_comp;	/* Compression Type		*/
	unsigned char		ih_name[IH_NMLEN];	/* Image Name		*/
	unsigned int	ih_ksz;		/* Kernel Part Size		*/
	unsigned char     product[PRODUCT_LEN];
	unsigned char     sw_version[VERSION_LEN];
	unsigned char     hw_version[VERSION_LEN];
} image_header_t;

static int check_fw_validity(const char *fw_path)
{
    unsigned char *buf_fw = 0;
    unsigned int udSize = 0;
    image_header_t *kernel_header = 0;
    unsigned int kernel_size = 0;
    unsigned long hcrc = 0;
    unsigned long hcrc_read = 0;
    unsigned long dcrc = 0;
    int ret = 0; // default Fail
    unsigned int hcrc_bkup = 0;
    
    FILE *file = fopen(fw_path, "rb");
    if (file == 0)
        goto end;

    buf_fw = (unsigned char*)malloc(16*1024*1024);
    if (buf_fw == 0)
        goto end;
        
    fseek(file, 0, SEEK_END);
    udSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(buf_fw, udSize, 1, file);

    kernel_header = (image_header_t*)(buf_fw);
	kernel_size = ntohl(kernel_header->ih_size);

    // check magic number
    if ( IH_MAGIC != ntohl(kernel_header->ih_magic) )
        goto end;

    // check header crc
    hcrc_bkup = kernel_header->ih_hcrc;
    hcrc_read = ntohl(kernel_header->ih_hcrc); // backup hcrc
    kernel_header->ih_hcrc = 0;
    if (crc32( 0, buf_fw, sizeof(image_header_t) ) != hcrc_read)
        goto end;
    //kernel_header->ih_hcrc = hcrc_bkup; // if you want to write buf_fw, this step should not be ignored

    // check data crc
    if( crc32 (0, buf_fw + sizeof(image_header_t), kernel_size) != ntohl(kernel_header->ih_dcrc) )
        goto end;

    ret = 1; // Pass

end:
    if (file != 0) {fclose(file);}
    if (buf_fw != 0) {free(buf_fw);}
    return ret;
}

#define DL_FW	"/var/tmp/upld.bin"

static int do_fw_upgrade(void)
{
	if (check_fw_validity(DL_FW) == 0)
		return 0;

	// Set WAN LED to "Orange Blinking" when start to firmware upgrade
	system("killall -USR1 timer"); // Disable WanLedControl in timer
#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1)
	system("gpio l 16 0 4000 0 1 4000");
	system("gpio l 15 1 1 1 1 4000");
#else
	system("gpio l 3 0 4000 0 1 4000");
	system("gpio l 4 1 1 1 1 4000");
#endif

	system("mtd_write -r -w write "DL_FW" Kernel &");

	return 1;
}

#if 0
#include "easyroaming/include/wireless.h"
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
//#define SIOCIWFIRSTPRIV	0x8BE0
#define RTPRIV_IOCTL_SET (SIOCIWFIRSTPRIV + 0x02)
#define RTPRIV_IOCTL_GSITESURVEY (SIOCIWFIRSTPRIV + 0x0D)
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
struct dlinkreq {
	RT_802_11_MAC_TABLE data;
};
int update_sta_info(char *ifname)
{
	int skfd, i;
	struct iwreq wrq;
	struct dlinkreq dreq;
	RT_802_11_MAC_TABLE *p;
	char val[256],path[256];
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
		for (i=0; i<p->Num; i++) {
			sprintf(path,PATH_RtErConnStaInfo"/sta:%d/ifname", i + base_idx);
			xset_str(ifname, path);
			
			sprintf(val,"%02x:%02x:%02x:%02x:%02x:%02x", p->Entry[i].Addr[0],
														 p->Entry[i].Addr[1],
														 p->Entry[i].Addr[2],
														 p->Entry[i].Addr[3],
														 p->Entry[i].Addr[4],
														 p->Entry[i].Addr[5]);
			sprintf(path,PATH_RtErConnStaInfo"/sta:%d/mac", i + base_idx);
			xset_str(val, path);
			
			sprintf(path,PATH_RtErConnStaInfo"/sta:%d/rssi", i + base_idx);
			xset_int(MAX(MAX(p->Entry[i].AvgRssi0, p->Entry[i].AvgRssi1), p->Entry[i].AvgRssi2), path);            
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
#endif

// Todo: execute external action according exeFlag value
// Return: 0: Sucess, -1: Fail
int exe_ExternalCall(int exeFlag)
{
	int  ret = 0, itmp;
	char scriptdir[64];
    char buf[256] = {0};
    xget_str(scriptdir, PATH_OtScriptDir);

    if (exeFlag & EXTCALL_WanResync) {
        LOG_INFO("[%s,WanResync]", __FUNCTION__);
        // snprintf(buf, sizeof(buf), "%s/uplink.sh", scriptdir);
		// system(buf);	
        // Todo Actions:
        // 1. Update the values of <uplink> block to device 
        // 2. Restart wan/uplink connection
		system("starter restart wan &");	
    }
    if (exeFlag & EXTCALL_WlanResync) {
        LOG_INFO("[%s,WlanResync]\n", __FUNCTION__);
		// Todo Actions:
        // 1. Update <wireless2.4> block values to device
        // 2. Restart wireless 2.4G driver
		system("starter restart wlan2g &");		
    }
    if (exeFlag & EXTCALL_Wlan5gResync) {
        LOG_INFO("[%s,Wlan5gResync]\n", __FUNCTION__);
        // Todo Actions:
        // 1. Update <wireless5> block values to device
        // 2. Restart wireless 5G driver
		system("starter restart wlan5g &");
    }
    if (exeFlag & EXTCALL_DeviceResync)  {
        LOG_INFO("[%s,DeviceResync]\n", __FUNCTION__);
        // Todo Actions:
        // 1. Update the values of <onetouch>, <sys> and <safety360> blocks to device
        // 2. Reboot device if necessary, for example, <routerenbl> tag value has been changed.
        system("starter save device &");
    }
    if (exeFlag & EXTCALL_CfgSaveToFlash) {
        LOG_INFO("[%s,CfgSaveToFlash]\n", __FUNCTION__);
        // Todo Actions:
        // 1. Save device setting to flash
        system("starter save all &");
    }
    if (exeFlag & EXTCALL_WlanSiteSurvey) {
        LOG_INFO("[%s,WlanSiteSurvey]\n", __FUNCTION__);
        // Todo Actions:
        // 1. Star Wlan Site Survey action
        // 2. Update Site Survey results into <wirelesssitesurvey> block
        // 3. Change value of <survey> tag as 0 after finish update of <wirelesssitesurvey>. 
        system("starter updatesurvey &");
    }
    if (exeFlag & EXTCALL_FwUpgrade) {
        LOG_INFO("[%s,FwUpgrade]\n", __FUNCTION__);
        // Todo Actions:
        // 1. One Touch had stored the uploaded FW into temp directory,
        //    defined by node <tempdirecory> in XmlDB and save as the file 
        //    which name is defined by <fwupgradefn> tag.
        // 2. FW verification: 
        //    a. If FW is valid
        //       i. create a timer: write uploaded firmware content into flash and reboot
        //       ii. return 0 (success).
        //    b. If FW is invalid, return -1 (fail)
        if (do_fw_upgrade())
			return 0;
		else
			return -1;
		
    }
	/* DLINK add/s [Jack: 2015/01/30, used for release memory for FwUpgrade] */
    if (exeFlag & EXTCALL_FwUpload) {
        LOG_INFO("[%s,FwUpload]\n", __FUNCTION__);
        // Todo Actions:  (used when device support FwUpgrader Feature）
        // 1. After this event, One Touch will store the uploaded FW into temp directory.
        // 2. If device need to prepare the disk/memory (kill some unused daemon) before upload,
        //     those prepare actions should be done in here.
    }
	/* DLINK add/e [Jack: 2015/01/30] */
    if (exeFlag & EXTCALL_UpdateStaRssi) {
        LOG_INFO("[%s,UpdateStaRssi]\n", __FUNCTION__);
        // Todo Actions:  (used when device support Roaming feature)
        // 1. clear old blocks
        //if (xget_int(PATH_RtErConnStaInfo "/sta#")>0)
            //xdel(PATH_RtErConnStaInfo);
        // 2. update all connected WLAN STA information into XmlDB,
        //    '/runtime/easyroaming/connstainfo' block   
        //        <sta id="1">
        //            <ifname>ra0</ifname>
    	//			  <mac>11:22:33:44:55:66</mac>
    	//			  <rssi>-35</rssi>
        //        </sta>
        //        ...
        //    Each <sta> block is used to stored each connected STA information.
        //update_sta_info("ra0");
        //update_sta_info("rai0");
        system("starter updatesta");
    }
    if (exeFlag & EXTCALL_DeauthSta) {
		xget_str(buf, PATH_RtErDeAuthSta "/ifname");
		xget_str(buf+128, PATH_RtErDeAuthSta "/mac");
        LOG_INFO("[%s,DeauthSta,If=%s,Mac=%s]\n", __FUNCTION__, buf, buf+128);
        // Todo Actions:  (used when device support Roaming feature)
        // 1. deauth Station. This STA's ifname and MAC are specified in XmlDB,
        //    '/runtime/easyroaming/deauthsta' block 
        //        <ifname>ra0</ifname>
		// 		  <mac>11:22:33:44:55:66</mac>

		// Skip slaves
		int i;
		char slave_mac[128]={0},path[128]={0};
		int slave_entries = xget_int(PATH_RtApcLivingSlave"/entry#");
		int slave_type = 0;
		unsigned int mac[6] = {0};
		for (i=0; i<slave_entries; i++) {
			
			snprintf(path, 127, PATH_RtApcLivingSlave"/entry:%d/uplwltype", i+1);
			slave_type = xget_int(path);
			LOG_INFO("EXTCALL_DeauthSta check slave type %d\n", slave_type);
			if (slave_type == 5)
				snprintf(path, 127, PATH_RtApcLivingSlave"/entry:%d/wl5bssid", i+1);
			else
				snprintf(path, 127, PATH_RtApcLivingSlave"/entry:%d/wl2.4bssid", i+1);
			xget_str(slave_mac, path);
			
//			sscanf(slave_mac, "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
//			sprintf(slave_mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0]+2,mac[1],mac[2],mac[3],mac[4],mac[5]);
			LOG_INFO("EXTCALL_DeauthSta check slave %s\n", slave_mac);
			if (!strcasecmp(buf+128+3, slave_mac+3)) {//if is slave. skip first mac section
				LOG_INFO("EXTCALL_DeauthSta Skip slave %s\n", slave_mac);
				return -1;
			}
		}
		
		char cmd[128];
		snprintf(cmd, 127, "iwpriv %s set DisConnectSta=%s", buf, buf+128);
		LOG_INFO("DeAuthSta: %s\n", cmd);
		system(cmd);
    }
	/* DLINK add/s [Jack: 2015/01/30, support Rss Data Validate] */
    if (exeFlag & EXTCALL_DataValidate) {
		xget_str(buf, PATH_RtRssValidate "/name");
		xget_str(buf+128, PATH_RtRssValidate "/value");
		xget_str(buf+64, PATH_RtRssValidate "/return_msg_language");
        LOG_INFO("[%s,DataValidate,N=%s,D='%s',L=%s]\n", __FUNCTION__, buf, buf+128, buf+64);
        int data_len = strlen(buf+128);
        // Todo Actions:  (used when device support RssCheck API)
        // 1. check Rss data validity. The Rss data's name, value and return_msg_language specified in XmlDB,
        //    '/runtime/rss_validate' block 
        //   	<name>rss_DeviceName</name>
		// 		<value>My Device Name</value>
		// 		<return_msg_language>en</return_msg_language>
		//     Avaible value of return_msg_language
		//         am ar bg bn ca cs da de el en en-GB en-US es es-419 et fi fil fr gu he hi hr hu id it ja kn 
		//         ko lt lv ml mr nl or pl pt pt-BR pt-PT ro ru sk sl sr sv sw ta te th tr uk vi zh zh-CN zh-TW       
		int langCode = 0;
		if ( (strcasecmp(buf+64, "zh")==0) || (strcasecmp(buf+64, "zh-cn")==0) || (strcasecmp(buf+64, "zh_cn")==0) )
			langCode = 1;
		else if ( (strcasecmp(buf+64, "zh-tw")==0) || (strcasecmp(buf+64, "zh_tw")==0) )
			langCode = 2;
        // 2. data validate: 
        //    a. If data is valid, return 0 (success).
        //    b. If data is invalid
        //        i. store error message to <RssChkMessage> node in '/runtime/rss_validate' block.
        //        ii. return -1 (fail)
        // 3. Avaible rssVar Name: 
        //   a. rss_DeviceName  		--> /sys/friendlyname
        //   b. rss_DevicePwd			--> /sys/user:1/password
        //   c. rss_Wl2_WirelessName	--> /lan/wireless2.4/wirelessname
        //   d. rss_Wl2_EncryptionKey	--> /lan/wireless2.4/encryptionkey
        //   e. rss_Wl5_WirelessName	--> /lan/wireless5/wirelessname
        //   f. rss_Wl5_EncryptionKey	--> /lan/wireless5/encryptionkey
        //   g. rss_Upl_Ets_WirelessName	--> /uplink/extendersite/wirelessname
        //   h. rss_Upl_Ets_EncryptionKey	--> /uplink/extendersite/encryptionkey
        //   i. rss_Upl_LoginUserName		--> /uplink/loginUsername
        //   j. rss_Upl_LoginPassword		--> /uplink/loginpassword
        // 4. Example: 
		// if (strcmp(buf, "rss_DeviceName")==0) {
        //    if ( (data_len<1) || (data_len>63) ) {
        //        if (langCode==1)
        //            strcpy(buf, "��λ����Ϊ����������볤��Ϊ63.");
		//        else if (langCode==2)
		//	         strcpy(buf, "��λ���ɞ�������ݔ���L�Ȟ�63.");
        //        else
        //            strcpy(buf, "Invalid input, Valid string length: 1~63.");
        //        xset_str(buf, PATH_RtRssValidate "/RssChkMessage");
        //        return -1;
        //    }
		// }
        xset_str("TODO", PATH_RtRssValidate "/RssChkMessage");
        return -1;  // due to TODO
    }
	/* DLINK add/e [Jack: 2015/01/30] */

    return ret; 
}

// Todo: process rss actions notification
// Return: 0: ok, 1: fail 
int processRssNotification(rssActionType_t act_ID, rssActionStat_t stat_ID, 
                                      rssExceptionType_t exc_ID, char *msg)
{
	assert (act_ID<RssActionType_Unknow);
	assert (stat_ID<RssActionStat_Unknow);
	assert (exc_ID<RssExceptionType_Unknow);

	int  ret = 0;
	//LOG_DEBUG("[%s,Act=%s,Stat=%s,%s,Msg=%s]\n", __FUNCTION__, 
	//	      rssActionTypeStr[act_ID], rssActionStatStr[stat_ID], rssExceptionTypeStr[exc_ID], msg);
    switch (act_ID) {
		
		case RssActionType_RssStartUp:
			LOG_MSG("Startup Services!\n"); 
			break;
			
		case RssActionType_RssShutDown:
			LOG_MSG("Shutdown Services!\n"); 
			break;
			
		case RssActionType_CloneToSlaves:
			switch (stat_ID) {
		        case RssActionStat_Before:
					// Todo: Actions before Master Clone to Slaves
					LOG_MSG("Start to clone config to some Slaves!\n"); 
					break;
				case RssActionStat_Done:
					// Todo: Actions after Master Clone to Slaves
					// msg=<slave1_ip>,<slave2_ip>,...
					LOG_MSG("Finished clone config to Slaves:'%s'!\n", msg); 
		            break;
		        case RssActionStat_Exception:
					// Todo: Actions when the Exception occured
					switch (exc_ID) {
						case RssExceptionType_None:
							break;
						case RssExceptionType_CloneFail:
							// msg=<slave1_ip>,<slave2_ip>,...
							LOG_MSG("Fail to clone config to Repeater Slaves:'%s'!\n", msg); 
							
							break;
						case RssExceptionType_RejectAction:
							LOG_MSG("Clone Reject due to '%s'!\n", msg);
							if (strcmp(msg, "DoingCloning")==0) {	
								// Reject due to clone is doing
								
							}
							else if (MATCH_PREFIX(msg, "SlaveMonitorFailList:")) {
								// Reject due to some monitor slaves was disconnected
								// msg formate: "SlaveMonitorFailList: <indexList>"

								//   for example: "SlaveMonitorFailList: 2,4", mean the slave:2 and slave:4 are disconnected
								// the monitor slaves would be defined in block "/runtime/apcloninginfo/monitorslave"
								// <monitorslave>
								//     <slave:1>uuid:3cfa7fb8-a2e7-4f19-ad41-ffa3aff6d2bd</slave>
								//     <slave:2>uuid:3cfa7fb8-a2e7-4f19-ad42-ffaa2003301</slave>
								//     ...
								// </monitorslave>
								
							}
							break;
						default:
							LOG_WARNING("[%s]Unknow exception id=%d\n",__FUNCTION__, exc_ID);
					}
					break;
				default:
					LOG_WARNING("[%s]Unknow stat id=%d\n",__FUNCTION__, stat_ID);
			}
			break;
			
		case RssActionType_SlaveRegister:
			switch (stat_ID) {
				case RssActionStat_Done:
					// msg=<slave_ip>,<slave_uuid>
					LOG_MSG("New Slave '%s' was connnected!\n", msg); 
		            break;
		        case RssActionStat_Exception:
					// Todo: Actions when the Exception occured
					switch (exc_ID) {
						case RssExceptionType_None:
							break;
						case RssExceptionType_CloneFail:
							// msg=<slave_ip>,<slave_uuid>
							LOG_MSG("Clone Config to Slave '%s' Fail!\n", msg); 
							break;
						case RssExceptionType_ExpiredConnect:	
							// msg=<slave_ip>,<slave_uuid>
							LOG_MSG("Slave '%s' connection was Expired!\n", msg); 
							break;
						default:
							LOG_WARNING("[%s]Unknow exception id=%d\n",__FUNCTION__, exc_ID);
					}
					break;
				default:
					LOG_WARNING("[%s]Unknow stat id=%d\n",__FUNCTION__, stat_ID);
			}
			break;
			
		case RssActionType_SlaveUnRegister:
			switch (stat_ID) {
				case RssActionStat_Done:
					// msg=<slave_ip>,<slave_uuid>
					LOG_MSG("Slave '%s' was disconnnected!\n", msg); 
		            break;
				default:
					LOG_WARNING("[%s]Unknow stat id=%d\n",__FUNCTION__, stat_ID);
			}
			break;
		case RssActionType_ConnectMaster:
			switch (stat_ID) {
				case RssActionStat_Done:
					// msg=<master_ip>,<master_uuid>
					LOG_MSG("Master '%s' was connnected!\n", msg); 
		            break;
		        case RssActionStat_Exception:
					// Todo: Actions when the Exception occured
					switch (exc_ID) {
						case RssExceptionType_None:
							break;
						case RssExceptionType_ExpiredConnect:	
							// msg=<master_ip>,<master_uuid>
							LOG_MSG("Master '%s' was Expired!\n", msg); 
							break;
						default:
							LOG_WARNING("[%s]Unknow exception id=%d\n",__FUNCTION__, exc_ID);
					}
					break;
				default:
					LOG_WARNING("[%s]Unknow stat id=%d\n",__FUNCTION__, stat_ID);
			}
			break;
			
		case RssActionType_DisconnectMaster:
			switch (stat_ID) {
				case RssActionStat_Done:
					// msg=<master_ip>,<master_uuid>
					LOG_MSG("Master '%s' was disconnnected!\n", msg); 
					break;
				default:
					LOG_WARNING("[%s]Unknow stat id=%d\n",__FUNCTION__, stat_ID);
			}
			break;
		default:
			//LOG_WARNING("[%s]Unknow action id=%d\n",__FUNCTION__, act_ID);
			break;
	}
	return ret; 
}

