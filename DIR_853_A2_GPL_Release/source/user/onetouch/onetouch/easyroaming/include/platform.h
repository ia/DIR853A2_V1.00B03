#define MAX_STA_PER_IFACE	128
#define MAX_VAP				16

typedef struct {
	unsigned char	mac[6];
	char			rssi;	//This value should be the decibel, the unit is db.
} STA_RSSI_t;

typedef struct {
	unsigned char	count;
	STA_RSSI_t		sta[MAX_STA_PER_IFACE];
} Connected_STA_t;

typedef enum { 
	Pencryption_AES = 1,
	Pencryption_TKIP = 2,
	Pencryption_AESTKIP = 3
} PEncryption_t;

typedef enum { 
	PwepAuth_OPEN = 1,
	PwepAuth_SHAREDKEY = 2
} PwepAuth_t;

typedef enum { 
	PwepEncryption_64 = 1,
	PwepEncryption_128 = 2
} PwepEncryption_t;

typedef enum { 
	PwepKeyFormat_HEX = 1,
	PwepKeyFormat_ASCII = 2
} PwepKeyFormat_t;

typedef enum {
	PWIFI_RADIO_2G = 0,
	PWIFI_RADIO_5G_1,
	PWIFI_RADIO_5G_2,
	PWIFI_RADIO_NUM
} RadioEnum_t;

typedef struct {
	char	ifname[16];
	char 	ssid[33];
	char 	password[65];
	char 	security[8]; //OPEN,WPA,WPA2,WPA+WPA2,WEP
	PEncryption_t 	encryption;
	PwepAuth_t 		wepAuth;
	PwepEncryption_t wep_encryption;
	PwepKeyFormat_t	wep_keyformat;
	RadioEnum_t radio_type;
} Config_t;

typedef struct {
	unsigned char	count;
	Config_t		config[MAX_VAP];
} AP_Wireless_t;

//return 0 for success, others for fail
int Platform_Get_Connected_STA(char *ifname, Connected_STA_t *stapool);
int Platform_Deauth_STA(char *ifname, unsigned char *mac);
int Platform_Get_Wireless_Cofnig(AP_Wireless_t *appool);
