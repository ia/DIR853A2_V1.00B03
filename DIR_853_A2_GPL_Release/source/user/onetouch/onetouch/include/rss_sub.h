/****************************************************************************/
/*                                                                          */
/* XML_PARSER.H                                                           */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines all APIs to receive the RssXML uploaded by Rss APP.  */
/*                                                                          */
/****************************************************************************/
#ifndef __STR_SUB_H__
#define __STR_SUB_H__

#include "debug_log.h"

#define MATCH_PREFIX(a, b)  (strncmp((a),(b),sizeof(b)-1)==0)
#define IMATCH_PREFIX(a, b)  (strncasecmp((a),(b),sizeof(b)-1)==0)

typedef void (*timer_callback)(unsigned long param);

typedef struct my_timer_t
{
	struct my_timer_t *next;
	unsigned long sec;
	unsigned long orgsec;
	unsigned long maxsec;
	unsigned long param;
	int flag;
	timer_callback callback;	
} my_timer;

/* TIMER action bit mask. Some maybe can't work together. */
#define TIMER_ONCE_TIMER		0x00	/* only run one time. */
#define TIMER_AUTO_REPEAT		0x01	/* repeat timer. */
#define TIMER_AUTO_FREE			0x02	/* auto help to free timer. */

my_timer *create_timer(unsigned long sec, timer_callback callback, unsigned long param, int flag, unsigned long maxsec);
void add_timer(my_timer *timer);

char *getOTName();
char *strncpy_HtmlDecode_(char *desStr, char *srcStr, int desSize, int srcSize);
char *strncpy_HtmlDecode(char *desStr, char *srcStr, int size);
char * strncpy_HtmlEncode_(char *desStr, char *srcStr, int desSize, int srcSize);
char *strncpy_HtmlEncode(char *desStr, char *srcStr, int size);
char *strncpy_XmlDecode_(char *desStr, char *srcStr, int desSize, int srcSize);
char *strncpy_XmlDecode(char *desStr, char *srcStr, int size);
char *strncpy_XmlEncode_(char *desStr, char *srcStr, int desSize, int srcSize);
char *strncpy_XmlEncode(char *desStr, char *srcStr, int size);

char *getDevCfgKey(char *skey, char *ip);
char *getXNode_(char* data, char *nodepath, char *defaultValue, int xmlCode);
char *getXNode(char* data, char *nodepath, char *defaultValue);
int StrCmpWithXStr(char *value, char *xStrKey, int chkOnly); 
int IntCmpWithXStr(int value, char *xStrKey, int chkOnly); 
int IntMaxCmpWithXStr(int value, char *xStrKey, int chkOnly, int chgmax, int *err);

char *strChgKeyword(char *des, char *src, char *keyWord, char *keyValue);
int strnWildcardCmp(const char *data, const char *pattern, int size);
int strWildcardCmp(const char *data, const char *pattern);
char* strstri(char* str, char* subStr);
char *strip_chars(char *str, char *reject);
char *trimTo(char *desstr, char *srcstr);
char *trim(char *srcstr);
int is_Valid_IP_CheckMask(char *netMask, char *theIP1, char *theIP2);
int is_Valid_IP(char *buf);
int is_Valid_Netmask(char *buf);
int is_Valid_Mac(char *buf);

int SearchKeyArray(char *key, char *keyArray[]);

typedef void (*XMLERRMSG_LostKey)(char *parent, char *keyword);
extern XMLERRMSG_LostKey XmlErrMsg_LostKey;
extern char *rsssub_EncryKey;

char *FindXmlKeyContain_(char *bb, int bblen, char *keyword, int *blen_ptr, char SearchOnly);
char *FindXmlKeyContain(char *bb, int bblen, char *keyword, int *blen_ptr);
char *SearchXmlKeyHeader(char *bb, int bblen, char *keyword);
int GetXmlKeyStrValue_(char *bb, int bblen, char *keyword, char *value, int vsize);
int GetXmlKeyIntValue_(char *bb, int bblen, char *keyword, int *value);
int GetXmlKeyStrValue(char *bb, int bblen, char *parent, char *keyword, char *value, int vsize);
int GetXmlKeyIntValue(char *bb, int bblen, char *parent, char *keyword, int *value);
char *prepareXmlBlockData(char *parent, char *data, int dlen, char *blockKey, char *newParentStr, int *blen_ptr);

long sys_uptime_seconds();
char *sys_uptime_sec_str();
long sys_info_(char *keyword);

#endif /* __STR_SUB_H__ */


