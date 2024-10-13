#ifndef __AUTH_H__
#define __AUTH_H__

#include <sys/time.h>

#define COOKIE_MAXLEN 32 
#define AUTHLIST_MAXNO 100 
typedef struct clientauthlist {
	char clientip[128];
	char username[64];
	char userpwd[64];
	char sessionID[COOKIE_MAXLEN];
	unsigned long expireTime;
	struct clientauthlist *nextPtr;
	int FailTimes;
} CLIENTAUTHLIST, *CLIENTAUTHLISTPTR;

#define SESSIONID_HEADER 		"SESSIONID="
#define SESSIONID_DKEYUSER 		"_DKEY_"
#define SESSIONID_LOGINUSER 	"_LOGIN_"
#define SESSIONID_UNAUTHUSER 	"_UnAuth_"
#define SESSIONID_AUTHUSER 		"_Auth_"

extern CLIENTAUTHLISTPTR cur_authinfo;	// predefined in auth.c

int clientAuthListAdd(char *clientip, char *clientuser, char *clientpwd, char *cookie);
int clientAuthListDel(char *clientip, char *clientuser, char *clientpwd, char *cookie);
CLIENTAUTHLISTPTR findUnAuthClient(char *clientip);
CLIENTAUTHLISTPTR findAuthClient(char *clientip, char *clientuser, char *clientpwd, char *cookie);
int getLoginClientKey(char *clientip, char *key);
int checkLoginClientKey(char *clientip, char *key);
int removeLoginClientKey(char *clientip, char *key);
int updateUnAuthClient(char *clientip);
void ShowAuthClientList();
int checkClientAuth(char *clientip, char *user, char *pwd, char *cookie);
int starter_auth_check(char *request_uri, char *remote_ip, char *cookie);
long getLockExpireTime(char *remote_ip);
char *generate_StarterKey(char *remote_ip);
char *generate_DKey(char *remote_ip);

#endif // __AUTH_H__

