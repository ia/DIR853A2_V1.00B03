#ifndef __DHTTPD_H__
#define __DHTTPD_H__

#include "rss.h"
#include "auth.h"

typedef struct DHttpd_websRec {
	// Prepared Data
	char	*webDefaultDir;		/* Connecting ipaddress */
	char	*ipaddr;			/* Connecting ipaddress */
	char	*url;				/* Full request url */
	char	*cookie;			/* Cookie string */
	char	*postData;          /* Post data */
	int		lenPostData;        /* Post data length */
	// Flag 0/1:Disable/Enable check URL: "/rssui/", "/rsstest/", "/tmp/rss" 
	int		isChkPath;        	
	// Flag 0/1:Disable/Enable bypass login page when device is default setting 
	int		isByPassLogin;        	
	// Result Data
	char 	*ret_data;
	int 	isNoCache;
	int 	isNewUser;
	CLIENTAUTHLISTPTR 	currAuthUser;
} DHttpd_WebsRec, *DHttpd_WebsRecPtr;


#define RssUI_UrlRootPath		"/"		
/* then dhttpd.c will base on this to define follow path:
#define RssUI_Path			RssUI_UrlRootPath "rssui/"	
#define RssUI_TestPath		RssUI_UrlRootPath "rsstest/"	
#define RssUI_PublicPath		RssUI_UrlRootPath "public"	
*/

int checkUserAuth(DHttpd_WebsRecPtr dwp, char **ptrSID);
void free_DHttpdWebsRec(DHttpd_WebsRecPtr dwp);
int dhttpd_WebsHandler(DHttpd_WebsRecPtr dwp);
char *generateLoginUrl();
int chkByPassLoginPage();

#endif // __DHTTPD_H__
