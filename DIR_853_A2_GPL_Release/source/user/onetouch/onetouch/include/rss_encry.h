#ifndef __RSSENCRY_HEADER_H__
#define __RSSENCRY_HEADER_H__

#define RssEncryptHeader    "Rss>"
#define RssEncryptError     "**RssEncryERROR**"
#define RssEncryptKeyPath   "/runtime/rssencryptkey"
#define DEV_MAC_XMLPATH		PATH_FwUpgInfo "/devicemac"
#define ENCRY_KEYBUF_SIZE	33

char *getRssEncryptKey();
char *setRssEncryptKey(char *newKey);
char *rssStrEncrypt(unsigned char *desStr, unsigned char *srcStr, char *keyStr);
char * strncpy_RssEncode_(char *desStr, char *srcStr, int desSize, char *encryKey);
char * strncpy_RssDecode_(char *desStr, char *srcStr, int desSize, char *encryKey);
char * strncpy_RssEncode(char *desStr, char *srcStr, int desSize);
char * strncpy_RssDecode(char *desStr, char *srcStr, int desSize);
char * rssDataEncode_(char *srcStr, char *encryKey);
char * rssDataDecode_(char *srcStr, char *encryKey);
char * rssDataEncode(char *srcStr);
char * rssDataDecode(char *srcStr);

#endif // __RSSENCRY_HEADER_H__
