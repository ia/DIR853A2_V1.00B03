#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "nvram.h"
#include "mdb.h"

#define NVRAM_WIFI1    RT2860_NVRAM
#define NVRAM_WIFI2    RTDEV_NVRAM
#define NVRAM_WIFI3    WIFI3_NVRAM
#define AP_LIST_MAX    100

static unsigned int wlan_config_doing = 0;
#if 0
#define DBG(fmt, arg...) printf("%s(%d) ===> "fmt"\n", __func__, __LINE__, ##arg)
#else
#define DBG(fmt, arg...)
#endif
#if 0
#define printf_unencode(fmt, arg...)  printf(""fmt"", ##arg)
#else
#define printf_unencode(fmt, arg...)
#endif

#define print_out(fmt, arg...) printf(""fmt"", ##arg)

/*
 * Convert Ethernet address string representation to binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @param	e	binary data
 * @return	TRUE if conversion was successful and FALSE otherwise
 */
int ether_atoe(const char *a, unsigned char *e)
{
    char *c = (char *) a;
    int i = 0;

    memset(e, 0, ETHER_ADDR_LEN);
    for (;;) {
        e[i++] = (unsigned char) strtoul(c, &c, 16);
        if (!*c++ || i == ETHER_ADDR_LEN)
            break;
    }
    return (i == ETHER_ADDR_LEN);
}

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code)
{
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str)
{
    char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
    if (buf == NULL)
        return NULL;
    while (*pstr) {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
            *pbuf++ = *pstr;
        else if (*pstr == ' ')
            *pbuf++ = '+';
        else
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_decode(const char *str)
{
    char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
    if (buf == NULL)
        return NULL;
    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        } else if (*pstr == '+') {
            *pbuf++ = ' ';
        } else {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

int get_nth_value(int cnt, const char *string, char *delimit, char *result)
{
    char str[128];
    char *pch;
    int i=1;

    strcpy(str, string);
    pch = strtok (str, delimit);

    while(pch != NULL ) {
        if( i == cnt ) {
            strcpy(result, pch);
            break;
        }
        pch = strtok (NULL, delimit);
        i++;
    }
    return 0;
}

char *set_nth_value(int Index, char *old_values, char *new_value)
{
    int i;
    char *p, *q;
    static char ret[2048];
    char buf[8][256];

    memset(ret, 0, 2048);
    for (i = 0; i < 8; i++)
        memset(buf[i], 0, 256);

    /*copy original values*/
    for ( i = 0, p = old_values, q = strchr(p, ';') ;
          i < 8 && q != NULL ;
          i++, p = q + 1, q = strchr(p, ';')) {
        strncpy(buf[i], p, q - p);
    }
    strcpy(buf[i], p); /*the last one*/

    /*replace buf[Index] with new_value*/
    strncpy(buf[Index], new_value, 256);

    /*calculate maximum Index*/
    Index = (i > Index)? i : Index;

    /*concatenate into a single string delimited by semicolons*/
    strcat(ret, buf[0]);
    for (i = 1; i <= Index; i++) {
        strncat(ret, ";", 2);
        strncat(ret, buf[i], 256);
    }

    return ret;
}

void set_nth_value_flash(int nvram, int cnt, char *flash_key, char *value)
{
    char *result = NULL;
    const char *tmp = nvram_get(nvram, flash_key);
    if(!tmp)
        tmp = "";
    result = set_nth_value(cnt, (char *)tmp, value);
    nvram_bufset(nvram, flash_key, result);
}


//==================================================
int get_cur_ip_info(const char *name, char *vbuf, int size)
{
    char verstr[128];
    int len;
    char *urlptr;

    len = 0;
    memset((void *)verstr, 0, sizeof(verstr));
    len += snprintf(verstr, sizeof(verstr), "I=%s", nvram_safe_get("wan_wan0_ipaddr"));
    len += snprintf(&verstr[len], sizeof(verstr) - len, "&N=%s", nvram_safe_get("wan_wan0_netmask"));
    len += snprintf(&verstr[len], sizeof(verstr) - len, "&G=%s", nvram_safe_get("wan_wan0_gateway"));
    snprintf(&verstr[len], sizeof(verstr) - len, "&D=%s", nvram_safe_get("wan_wan0_dns"));

    printf_unencode("%s\n", verstr);
    urlptr = url_encode(verstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_eth_cable_st(const char *name, char *vbuf, int size)
{
    char verstr[16];
    bool stat;
    char *urlptr;

    memset((void *)verstr, 0, sizeof(verstr));
    sprintf(verstr, "%s", nvram_safe_get("wan_wan0_status"));

    if(!strcmp(verstr,"CONNECTED")) {
        stat = 1;
    } else {
        stat = 0;
    }
    sprintf(verstr, "C=(%d)", stat);

    urlptr = url_encode(verstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int securit2dlink(char* tmp)
{
    if (0 == strcmp(tmp, "OPEN")) {
        return 0;
    } else if (0 == strcmp(tmp, "WPAPSK")) {
        return 3;
    } else if (0 == strcmp(tmp, "WPA2PSK")) {
        return 4;
    } else if (0 == strcmp(tmp, "WPAPSKWPA2PSK")) {
        return 5;
    }

    return 0;
}

char * dlink2securit(char* tmp)
{
    if (0 == strcmp(tmp, "0")) {
        return "OPEN";
    } else if (0 == strcmp(tmp, "3")) {
        return "WPAPSK";
    } else if (0 == strcmp(tmp, "4")) {
        return "WPA2PSK";
    } else if (0 == strcmp(tmp, "5")) {
        return "WPAPSKWPA2PSK";
    }

    return "";
}

int encrypt2dink(char* tmp)
{
    if (0 == strcmp(tmp, "NONE")) {
        return 0;
    } else if (0 == strcmp(tmp, "TKIP")) {
        return 3;
    } else if (0 == strcmp(tmp, "AES")) {
        return 2;
    } else if (0 == strcmp(tmp, "TKIPAES")) {
        return 4;
    }

    return 0;
}

char *dink2encrypt(char* tmp)
{
    if (0 == strcmp(tmp, "3")) {
        return "TKIP";
    } else if (0 == strcmp(tmp, "2")) {
        return "AES";
    } else if (0 == strcmp(tmp, "4")) {
        return "TKIPAES";
    }

    return "";
}

int get_wlan_ap_info(const char *name, char *vbuf, int size)
{
    char verstr[256];
    char *urlptr;
    int len =0;
    char sec_tmp[64], enc_tmp[64];

    get_nth_value(1, (char*)nvram_bufget(NVRAM_WIFI1, "AuthMode"), ";", sec_tmp);
    get_nth_value(1, (char*)nvram_bufget(NVRAM_WIFI1, "EncrypType"), ";", enc_tmp);

    memset((void *)verstr, 0, sizeof(verstr));
    len +=sprintf(verstr, "I=%s&M=%d&S=%d&E=%d&K=%s&O=%s",
                  nvram_bufget(NVRAM_WIFI1, "SSID1"),
                  0,
                  securit2dlink(sec_tmp),
                  encrypt2dink(enc_tmp),
                  nvram_bufget(NVRAM_WIFI1, "WPAPSK1"),
                  nvram_bufget(NVRAM_WIFI1, "Radio24On") );

#if 0
    len +=sprintf(&verstr[len], ";I=%s&M=%d&S=%d&E=%d&K=%d&O=%s",
                  nvram_bufget(NVRAM_WIFI1, "SSID2"),
                  0,
                  securit2dlink(nvram_bufget(NVRAM_WIFI1, "AuthMode")),
                  encrypt2dink(nvram_bufget(NVRAM_WIFI1, "EncrypType")),
                  nvram_bufget(NVRAM_WIFI1, "WPAPSK2")
                  nvram_bufget(NVRAM_WIFI1, "Radio24Vap1En"));

    len +=sprintf(&verstr[len],";I=%s&M=%d&S=%d&E=%d&K=%d&O=%s",
                  nvram_bufget(NVRAM_WIFI2, "SSID1"),
                  0,
                  securit2dlink(nvram_bufget(NVRAM_WIFI2, "AuthMode")),
                  encrypt2dink(nvram_bufget(NVRAM_WIFI2, "EncrypType")),
                  nvram_bufget(NVRAM_WIFI2, "WPAPSK1"),
                  nvram_bufget(NVRAM_WIFI1, "Radio58On"));

    len +=sprintf(&verstr[len],";I=%s&M=%d&S=%d&E=%d&K=%d&O=%s",
                  nvram_bufget(NVRAM_WIFI2, "SSID2"),
                  0,
                  securit2dlink(nvram_bufget(NVRAM_WIFI2, "AuthMode")),
                  encrypt2dink(nvram_bufget(NVRAM_WIFI2, "EncrypType")),
                  nvram_bufget(NVRAM_WIFI2, "WPAPSK2") ,
                  nvram_bufget(NVRAM_WIFI1, "Radio58Vap1En"));
#endif

    urlptr = url_encode(verstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }
    return MDB_SUCC;
}

char *str_value_get(char *str, char *key)
{
    char *str_copy, *str_p;
    char *str_ptr;
    static char ret[128] = "";

    if(NULL == str)
        return ret;

    str_copy = strdup(str);
    str_p = str_copy;

    while ((str_ptr = strsep(&str_copy, "&")) != NULL) {
        //DBG("str_ptr='%s', str_copy='%s'", str_ptr, str_copy);
        if(!strncmp(str_ptr, key, 2)) {
            //ret = str_ptr + 2;
            strcpy(ret, str_ptr + 2);
            break;
        }
    }
    DBG("key(%s) goto return (%s)\n",key, ret);
    free(str_p);
    return ret;
}

int set_wlan_ap_info(const char *name, const char *value)
{
    char *urlptr;
    int ret = MDB_FAIL;
    char *tmp, *tmp_ptr, *sec_tmp;

    if(value == NULL) {
        printf("%d\n",ERR_BAD_ATTR_FORMAT);
        return ERR_BAD_ATTR_FORMAT;
    }

    tmp_ptr = url_decode((char *)value);

    tmp = str_value_get(tmp_ptr, "I=");
    if(strcmp(tmp, "")) {
        nvram_bufset(NVRAM_WIFI1, "SSID1", tmp);
        nvram_bufset(NVRAM_WIFI2, "SSID1", tmp);//5G
        DBG("SSID1 = %s\n",tmp);
        ret = MDB_SUCC;
    }
    //mode : TODO

    sec_tmp = dlink2securit(str_value_get(tmp_ptr, "S="));
    if(strcmp(sec_tmp, "")) {
        set_nth_value_flash(NVRAM_WIFI1,0,"AuthMode", sec_tmp);
        set_nth_value_flash(NVRAM_WIFI2,0,"AuthMode", sec_tmp);
        DBG("AuthMode = %s\n",sec_tmp);
        ret = MDB_SUCC;
    }

    tmp = dink2encrypt(str_value_get(tmp_ptr, "E="));
    if(strcmp(tmp, "")) {
        set_nth_value_flash(NVRAM_WIFI1,0,"EncrypType", tmp);
        set_nth_value_flash(NVRAM_WIFI2,0,"EncrypType", tmp);
        DBG("EncrypType = %s\n",tmp);
        ret = MDB_SUCC;
    }

    tmp = str_value_get(tmp_ptr, "K=");
    if(strcmp(sec_tmp, "OPEN") && strlen(tmp)>=8) {
        DBG("WPAPSK1 = %s\n",tmp);
        nvram_bufset(NVRAM_WIFI1,"WPAPSK1", tmp);
        nvram_bufset(NVRAM_WIFI2,"WPAPSK1", tmp);//5G
        ret = MDB_SUCC;
    } else if (!strcmp(sec_tmp, "OPEN")) {
        DBG("WPAPSK1 = ""\n");
        nvram_bufset(NVRAM_WIFI1,"WPAPSK1", "");
        nvram_bufset(NVRAM_WIFI2,"WPAPSK1", "");
        ret = MDB_SUCC;
    }

    if(!strcmp(str_value_get(tmp_ptr, "O="), "1")) {
        nvram_bufset(NVRAM_WIFI1, "Radio24On", "1");
        nvram_bufset(NVRAM_WIFI2, "Radio58On", "1");
        DBG("RadioOn = 1\n");
        ret = MDB_SUCC;
    } else if(!strcmp(str_value_get(tmp_ptr, "O="), "0")) {
        nvram_bufset(NVRAM_WIFI1, "Radio24On", "0");
        nvram_bufset(NVRAM_WIFI2, "Radio58On", "0");
        DBG("RadioOn = 0\n");
        ret = MDB_SUCC;
    }
    set_nth_value_flash(NVRAM_WIFI1, 0, "BandSteering", "1");
    set_nth_value_flash(NVRAM_WIFI2, 0, "BandSteering", "1");

    nvram_commit(NVRAM_WIFI1);
    nvram_commit(NVRAM_WIFI2);
	wlan_config_doing = 1;
    printf("%d",ret);
    return ret;
}

int ap_list_scan(char *ifname, char *pbuf, int size)
{
    int n=0;
    char cmd_s[64],cmd_g[64];
    char tmp[512], index[32], ch[32], ssid[32], bssid[32], sec[32], enc[32], sig[32];
    char *p;
    sprintf(cmd_s,"iwpriv %s set SiteSurvey=1 1>/dev/null 2>&1",ifname);
    sprintf(cmd_g,"iwpriv %s get_site_survey",ifname);

    system(cmd_s);
    sleep(2);

    FILE *fd = popen(cmd_g, "r");
    if(!fd) {
        DBG("%s, error!\n",cmd_g);
        return -1;
    }

    int i=0;
    while(fgets(tmp, sizeof(tmp), fd) != NULL) {
        if(i<3) {
            i++;
        } else if(n < size) {
            if((p=strchr(tmp,'/'))!=NULL) {
                *p = ' ';
            }
            if(sscanf(tmp, "%s %s %s %s %s %s %s", index, ch, ssid, bssid, sec, enc, sig) == 7) {
                //DBG("--->%s %s %s %s %s %s %s\n", index, ch, ssid, bssid, sec, enc, sig);
                n += sprintf(pbuf+n, "I=%s&M=%d&S=%d&E=%d&P=%s;", ssid, 0, securit2dlink(sec), encrypt2dink(enc), sig);
            }
        }
    }
    pclose(fd);
    return n;
}

int get_wlan_ap_list(const char *name, char *vbuf, int size)
{
    char verstr[256*AP_LIST_MAX];
    char *urlptr;
    int len =0;

    memset((void *)verstr, 0, sizeof(verstr));

    len += ap_list_scan("ra0", verstr, sizeof(verstr));
    len += ap_list_scan("rai0", &verstr[len], sizeof(verstr)-len);

    if(len < 0) {
        DBG("get error!\n");
        return MDB_FAIL;
    }

    urlptr = url_encode(verstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }
    return MDB_SUCC;
}

int get_fw_version(const char *name, char *vbuf, int size)
{
    char verstr[63];
    int len;
    char *urlptr;

    len = 0;
    memset((void *)verstr, 0, sizeof(verstr));
    len += snprintf(verstr, sizeof(verstr), "%s", nvram_safe_get("sw_ex_version"));
    snprintf(&verstr[len], sizeof(verstr) - len, "%s", nvram_safe_get("sw_in_version"));

    urlptr = url_encode(verstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }
    return MDB_SUCC;
}

int get_dev_model(const char *name, char *vbuf, int size)
{
    char modstr[31];
    char *urlptr;

    memset((void *)modstr, 0, sizeof(modstr));
    strcpy(modstr, nvram_safe_get("model_name"));
    urlptr = url_encode(modstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }
    return MDB_SUCC;
}

int get_dev_name(const char *name, char *vbuf, int size)
{
    char modstr[31];
    int len;
    char *urlptr;

    len = 0;
    memset((void *)modstr, 0, sizeof(modstr));
    len = snprintf(modstr, sizeof(modstr), "%s", "D-Link+");
    len += snprintf(&modstr[len], sizeof(modstr) - len, "%s", nvram_safe_get("model_name"));

    urlptr = url_encode(modstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_admin_passwd(const char *name, char *vbuf, int size)
{
    char modstr[31];
    char *urlptr;

    memset((void *)modstr, 0, sizeof(modstr));
    strncpy(modstr, nvram_safe_get("http_passwd"), sizeof(modstr));
    urlptr = url_encode(modstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int set_admin_passwd(const char *name, const char *value)
{
    char *urlptr;
    int ret = MDB_FAIL;

    urlptr = url_decode(value);

    if(urlptr) {
        nvram_safe_set("http_passwd", (char *)urlptr);
        nvram_commit(RT2860_NVRAM);
        free(urlptr);
        ret = MDB_SUCC;
    }

    printf("%d",ret);
    return ret;

    return MDB_SUCC;
}

int get_http_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "80");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_https_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "443");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_sp_http_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8181");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_sp_https_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "4433");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_cam_conn(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "0");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_wc_http_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8185");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}
int get_wc_https_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8186");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_ext_ip(const char *name, char *vbuf, int size)
{
    char tmpstr[64] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", nvram_safe_get("wan_wan0_ipaddr"));

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_ex_info_http_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8182");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_ex_sp_http_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8181");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_ex_sp_https_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "4433");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_ex_wc_http_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8185");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_ex_wc_https_port(const char *name, char *vbuf, int size)
{
    char tmpstr[32] = "\0";
    char *urlptr;

    sprintf(tmpstr, "%s", "8186");

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_register_st(const char *name, char *vbuf, int size)
{
    snprintf(vbuf, size, "%s", nvram_get(WIFI3_NVRAM, "mydlink_register_status"));
	printf("%s", nvram_get(WIFI3_NVRAM, "mydlink_register_status"));
	return MDB_SUCC;
}

int set_register_st(const char *name, const char *value)
{
	if (strcmp(value, "1") == 0)
		nvram_set(WIFI3_NVRAM, "Mydlink_AccountStatus", "true");
	else
		nvram_set(WIFI3_NVRAM, "Mydlink_AccountStatus", "false");
	
	nvram_bufset(WIFI3_NVRAM, "mydlink_register_status", (char*)value);
	nvram_commit(WIFI3_NVRAM);
	
    printf("%d",MDB_SUCC);
	return MDB_SUCC;
}

int get_mac_addr(const char *name, char *vbuf, int size)
{
    unsigned char hwaddr[6];
    char macstr[31];
    char *urlptr;
    int i;

    memset((void *)hwaddr, 0, sizeof(hwaddr));
    memset((void *)macstr, 0, sizeof(macstr));
    ether_atoe(nvram_safe_get("lan0_hwaddr"), hwaddr);
    snprintf(macstr, sizeof(macstr), "%02x%02x%02x%02x%02x%02x",
             hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);

	for (i = 0; i < strlen(macstr); i++)
		macstr[i] = toupper(macstr[i]);
	if (size >= strlen(macstr))
		memcpy(vbuf, macstr, strlen(macstr));
	else
	{
		printf("%s", macstr);
		return ERR_BUF_TOO_SHORT;
	}
	printf("%s", macstr);

    return MDB_SUCC;
}

int get_attributes(const char *name, char *vbuf, int size)
{
    char tmpstr[256] = "";
    char *urlptr;

    memset((void *)tmpstr, 0, sizeof(tmpstr));
    strncpy(tmpstr, nvram_get(WIFI3_NVRAM,(char *)name), sizeof(tmpstr));

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }

    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }
    return MDB_SUCC;
}

int set_attributes(const char *name, const char *value)
{
    char *urlptr;
    int ret = MDB_FAIL;

    urlptr = url_decode(value);

    if(urlptr) {
        nvram_set(WIFI3_NVRAM, (char *)name, urlptr);
        nvram_commit(WIFI3_NVRAM);
        free(urlptr);
        ret = MDB_SUCC;
    }
    printf("%d",ret);
    return ret;
}

int get_dev_uid(const char *name, char *vbuf, int size)
{
    char tmpstr[20];
    char *urlptr;

    memset((void *)tmpstr, 0, sizeof(tmpstr));
    strncpy(tmpstr, nvram_bufget(WIFI3_NVRAM, "mydlink_uid"), sizeof(tmpstr));

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }
    return MDB_SUCC;
}

int get_pin_code(const char *name, char *vbuf, int size)
{
    char tmpstr[10];
    char *urlptr;

    memset((void *)tmpstr, 0, sizeof(tmpstr));
    strncpy(tmpstr, nvram_get(CONFIG2_NVRAM, "pin_code"), sizeof(tmpstr));

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_mydlink_no(const char *name, char *vbuf, int size)
{
    char tmpstr[20];
    char *urlptr;

    memset((void *)tmpstr, 0, sizeof(tmpstr));
    strncpy(tmpstr, nvram_bufget(WIFI3_NVRAM, "mydlink_number"), sizeof(tmpstr));

    urlptr = url_encode(tmpstr);
    if (vbuf && size > 0 && urlptr) {
        if (strlen(urlptr) <= size)
            strncpy(vbuf, urlptr, size);
        else {
            print_out("%s", urlptr);
            free(urlptr);
            return ERR_BUF_TOO_SHORT;
        }
    }
    if (urlptr) {
        print_out("%s", urlptr);
        free(urlptr);
    }

    return MDB_SUCC;
}

int get_mdb_st(const char *name, char *vbuf, int size)
{
    char tmpstr[20] = "";
    char *urlptr;

    sprintf(tmpstr, "%s", "1");

    if (vbuf && size > 0 ) {
        if (strlen(urlptr) <= size)
            strcpy(vbuf, tmpstr);
        else {
            printf("%s", tmpstr);
            return ERR_BUF_TOO_SHORT;
        }
    }

    printf("%s", tmpstr);
    return MDB_SUCC;
}
//========================================================
MDB_LIST  mdblists[] = {
    {"fw_version", get_fw_version, NULL, NULL},
    {"dev_model", get_dev_model, NULL, NULL},
    {"dev_name", get_dev_name, NULL, NULL},
    {"admin_passwd", get_admin_passwd, set_admin_passwd, NULL},
    {"http_port", get_http_port, NULL, NULL},
    {"https_port", get_https_port, NULL, NULL},
    {"register_st", get_register_st, set_register_st, NULL},
    {"mac_addr", get_mac_addr, NULL, NULL},
    {"attr_0", get_attributes, set_attributes, NULL},
    {"attr_1", get_attributes, set_attributes, NULL},
    {"attr_2", get_attributes, set_attributes, NULL},
    {"attr_3", get_attributes, set_attributes, NULL},
    {"attr_4", get_attributes, set_attributes, NULL},
    {"attr_5", get_attributes, set_attributes, NULL},
    {"attr_6", get_attributes, set_attributes, NULL},
    {"attr_7", get_attributes, set_attributes, NULL},
    {"attr_8", get_attributes, set_attributes, NULL},
    {"attr_9", get_attributes, set_attributes, NULL},
    {"mdb_st", get_mdb_st, NULL, NULL},
    {"dev_uid", get_dev_uid, NULL, NULL},
    {"pin_code", get_pin_code, NULL, NULL},
    {"mydlink_no", get_mydlink_no, NULL, NULL},
    {"wlan_ap_info", get_wlan_ap_info, set_wlan_ap_info, NULL},
    {"wlan_ap_list", get_wlan_ap_list, NULL, NULL},
    {"cur_ip_info", get_cur_ip_info, NULL, NULL},
    {"eth_cable_st", get_eth_cable_st, NULL, NULL},
    {"sp_http_port", get_sp_http_port, NULL, NULL},
    {"sp_https_port", get_sp_https_port, NULL, NULL},
    {"cam_conn", get_cam_conn, NULL, NULL},
    {"wc_http_port", get_wc_http_port, NULL, NULL},
    {"wc_https_port", get_wc_https_port, NULL, NULL},
    {"ext_ip", get_ext_ip, NULL, NULL},
    {"ex_info_http_port", get_ex_info_http_port, NULL, NULL},
    {"ex_sp_http_port", get_ex_sp_http_port, NULL, NULL},
    {"ex_sp_https_port", get_ex_sp_https_port, NULL, NULL},
    {"ex_wc_http_port", get_ex_wc_http_port, NULL, NULL},
    {"ex_wc_https_port", get_ex_wc_https_port, NULL, NULL},
    {NULL, NULL, NULL, NULL},
};

MDB_LIST *find_mdbcmd(const char *name)
{
    MDB_LIST *l;

    for (l = mdblists; l && l->name; l++) {
        if (strcmp(l->name, name) == 0)
            return l;
    }
    return NULL;
}
void list_cmds(void)
{
    MDB_LIST *l;

    for (l = mdblists; l && l->name; l++)
        printf("  %s\n", l->name);
}

int mdb_get (const char *attr, char *val_buf, size_t buf_sz)
{
    MDB_LIST *l;

    l = find_mdbcmd(attr);
    if (l == NULL)
        return ERR_UNKNOWN_ATTR;
    if (l->get == NULL)
        return ERR_BAD_CMD;
    return l->get(attr, val_buf, buf_sz);
}

int mdb_set (const char *attr, const char *attr_val)
{
    MDB_LIST *l;

    l = find_mdbcmd(attr);
    if (l == NULL)
        return ERR_UNKNOWN_ATTR;
    if (l->set == NULL)
        return ERR_BAD_CMD;
    return l->set(attr, attr_val);
}

int mdb_apply ()
{
    //fprintf(stderr, "Nothing to do now.\n");
    if (wlan_config_doing == 1)
    {
    	notify_rc("restart_wlan");
		wlan_config_doing = 0;
    }
    return MDB_SUCC;
}
