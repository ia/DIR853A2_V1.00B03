/*
 * main.c -- Main program for the GoAhead WebServer (LINUX version)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2010. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 */

/******************************** Description *********************************/

/*
 *	Main program for for the GoAhead WebServer.
 */

/********************************* Includes ***********************************/

#include	"../uemf.h"
#include	"../wsIntrn.h"
#include	<signal.h>
#include	<unistd.h> 
#include	<sys/types.h>

/* DLINK add/s */
#include "rss.h"
#include "auth.h"
#include "dhttpd.h"
#include "xml_parser.h"
/* DLINK add/e */

#ifdef WEBS_SSL_SUPPORT
#include	"../websSSL.h"
#endif

#ifdef USER_MANAGEMENT_SUPPORT
#include	"../um.h"
void	formDefineUserMgmt(void);
#endif

/* DLINK add/s [Carlos: 2014/09/05] */
static int webConfigGet(int eid, webs_t wp, int argc, char_t **argv);
static int webConfigGetUrlPath(int eid, webs_t wp, int argc, char_t **argv);
static int webConfigGetArray(int eid, webs_t wp, int argc, char_t **argv);
static int webGenerateDKey(int eid, webs_t wp, int argc, char_t **argv);
static int webConfigRssGet(int eid, webs_t wp, int argc, char_t **argv);
/* DLINK add/e */

/*********************************** Locals ***********************************/
/*
 *	Change configuration here
 */

static char_t		*rootWeb = T("www");			/* Root web directory */
static char_t		*demoWeb = T("wwwdemo");		/* Root web directory */
static char_t		*password = T("");				/* Security password */
static int			port = WEBS_DEFAULT_PORT;		/* Server port */
static int			retries = 5;					/* Server port retries */
static int			finished = 0;					/* Finished flag */

/****************************** Forward Declarations **************************/

static int 	initWebs(int demo);
static int	aspTest(int eid, webs_t wp, int argc, char_t **argv);
static void formTest(webs_t wp, char_t *path, char_t *query);
static void formUploadFileTest(webs_t wp, char_t *path, char_t *query); // add by gyr 2011.10.15
/* DLINK add/s */
static void rssfwupgd(webs_t wp, char_t *path, char_t *query);
/* DLINK add/e */
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
				int arg, char_t *url, char_t *path, char_t *query);
static void	sigintHandler(int);
#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif

/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */

int main(int argc, char** argv)
{
/* DLINK add/s */
	//Init Debug Log
	//D_LOGLEVEL = LOG_LEVEL_NOTICE;
    D_LOGLEVEL = xget_int(_OT_Path "/debuglevel");
    if (D_LOGLEVEL<=0) D_LOGLEVEL = 99;
    strcpy(D_LOGHEADER, _OT_LogHeader); 
    // D_LOGPATH -> use default value: temp path defined in xmldb
    // D_LOGFILESIZE_K -> use default value: 250
    D_LOGFILESIZE_K = xget_int(_OT_Path "/debuglogsize_k");
    if (D_LOGFILESIZE_K<0) D_LOGFILESIZE_K = 300;
/* DLINK add/s */

	int i, j, newport, demo = 0;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-demo") == 0) {
			demo++;
		}
	}

/* DLINK add/s [Laurel: 2014/02/12] */
    // get port setting
    newport = xget_int(_OT_Path "/rsshttpport");
    if (newport > 0)
        port = newport;
	for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0)
        {
			msgPrintf("Usage: %s [OPTIONS]\n", argv[0]);
			msgPrintf("  Info: ver:%s, " _OT_Starter "Lib ver:%s, RssSpec ver:%s\n", 
					  DAEMON_VER, starterLibVersion(), rssSpecVersion());
			msgPrintf("  -h 				show this help message.\n");
			msgPrintf("  -p <port_no>		set the port number used by daemon, default: %d\n", port);
            return 0;  
        }
        if (strcmp(argv[i], "-p") == 0)
        {
            j = atoi(argv[i+1]);
            if (j > 0)
              port = j;  
        }
    }
    LOG_MSG("RSS-HTTP Port=%d, ver:%s\n", port, DAEMON_VER);
    if (newport != port) 
        xset_int(port, _OT_Path "/rsshttpport");
	// Save VerInfo
	xset_str(DAEMON_VER, PATH_RtVersionInfo "/Web");
    // Save Default ID & PW
    xset_str(uiDefaultUserID(), PATH_RtSys "/default_id");
    xset_str(uiDefaultUserPW(), PATH_RtSys "/default_pw");
/* DLINK add/e */

/*
 *	Initialize the memory allocator. Allow use of malloc and start 
 *	with a 60K heap.  For each page request approx 8KB is allocated.
 *	60KB allows for several concurrent page requests.  If more space
 *	is required, malloc will be used for the overflow.
 */
	bopen(NULL, (60 * 1024), B_USE_MALLOC);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sigintHandler);
	signal(SIGTERM, sigintHandler);

/*
 *	Initialize the web server
 */
	if (initWebs(demo) < 0) {
		printf("initWebs error\n");	// added by gyr 2011.09.17
		return -1;
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLOpen();
/*	websRequireSSL("/"); */	/* Require all files be served via https */
#endif

/*
 *	Basic event loop. SocketReady returns true when a socket is ready for
 *	service. SocketSelect will block until an event occurs. SocketProcess
 *	will actually do the servicing.
 */
	finished = 0;
	while (!finished) {
		if (socketReady(-1) || socketSelect(-1, 1000)) {
			socketProcess(-1);
		}
		websCgiCleanup();
		emfSchedProcess();
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLClose();
#endif

#ifdef USER_MANAGEMENT_SUPPORT
	umClose();
#endif

/*
 *	Close the socket module, report memory leaks and close the memory allocator
 */
	websCloseServer();
	socketClose();
#ifdef B_STATS
	memLeaks();
#endif
	bclose();
	return 0;
}

/*
 *	Exit cleanly on interrupt
 */
static void sigintHandler(int unused)
{
	finished = 1;
}

/******************************************************************************/
/*
 *	Initialize the web server.
 */

static int initWebs(int demo)
{
	struct hostent	*hp;
	struct in_addr	intaddr;
	char			host[128], dir[128], webdir[128];
	char			*cp;
	char_t			wbuf[128];

/*
 *	Initialize the socket subsystem
 */
	socketOpen();

#ifdef USER_MANAGEMENT_SUPPORT
/*
 *	Initialize the User Management database
 */
	umOpen();
	umRestore(T("umconfig.txt"));
#endif

/*
 *	Set ../web as the root web. Modify this to suit your needs
 *	A "-demo" option to the command line will set a webdemo root
 */
	getcwd(dir, sizeof(dir));// Get the name of the current working directory, note | gyr 2011.09.17
	if ((cp = strrchr(dir, '/'))) {
		*cp = '\0';
	}
	if (demo) {
		sprintf(webdir, "%s/%s", dir, demoWeb);
	} else {
/* DLINK mod/s [James: 2013/12/20: to make the webdir to /www/]*/
		//sprintf(webdir, "%s/%s", dir, rootWeb);
		xget_str(webdir, _OT_Path "/rsshttproot");
		if (*webdir == '\0')
			sprintf(webdir, "/%s", rootWeb);
		else if (*webdir != '/') {
			char buf[128];
			sprintf(buf, "/%s", webdir);
			strcpy(webdir, buf);
		}
		xset_str(webdir, _OT_Path "/rsshttproot");
/* DLINK mod/e */
	}

/*
 *	Configure the web server options before opening the web server
 */
	websSetDefaultDir(webdir);
	cp = inet_ntoa(intaddr);
	ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
	websSetIpaddr(wbuf);
	ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
	websSetHost(wbuf);

/*
 *	Configure the web server options before opening the web server
 */
	websSetDefaultPage(T("default.asp"));
	websSetPassword(password);

/* 
 *	Open the web server on the given port. If that port is taken, try
 *	the next sequential port for up to "retries" attempts.
 */
	websOpenServer(port, retries);

/*
 * 	First create the URL handlers. Note: handlers are called in sorted order
 *	with the longest path handler examined first. Here we define the security 
 *	handler, forms handler and the default web page handler.
 */
	websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler, 
		WEBS_HANDLER_FIRST);
	websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
	websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
	websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler, 
		WEBS_HANDLER_LAST); 

/*
 *	Now define two test procedures. Replace these with your application
 *	relevant ASP script procedures and form functions.
 */
	websAspDefine(T("aspTest"), aspTest);
	websFormDefine(T("formTest"), formTest);
	websFormDefine(T("formUploadFileTest.xgi"), formUploadFileTest);// add by gyr 2011.10.15
/* DLINK add/s */
	websFormDefine(T("rssfwupgd.xgi"), rssfwupgd);//
/* DLINK add/e */

/* DLINK add/s [Carlos: 2014/09/05] */
    websAspDefine(T("ConfigGet"), webConfigGet);
	websAspDefine(T("ConfigGetArray"), webConfigGetArray);
	websAspDefine(T("ConfigGetPath"), webConfigGetUrlPath);
	websAspDefine(T("Generate_Key"), webGenerateDKey);
	websAspDefine(T("ConfigRssGet"), webConfigRssGet);
/* DLINK add/e */

/*
 *	Create the Form handlers for the User Management pages
 */
//Carlos cancel mark
//#ifdef USER_MANAGEMENT_SUPPORT
	formDefineUserMgmt();
//#endif

/*
 *	Create a handler for the default home page
 */
	websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0); 
	return 0;
}

/******************************************************************************/
/*
 *	Test Javascript binding for ASP. This will be invoked when "aspTest" is
 *	embedded in an ASP page. See web/asp.asp for usage. Set browser to 
 *	"localhost/asp.asp" to test.
 */

static int aspTest(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t	*name, *address;
	//printf(" aspTest argc = %d, argv = %s\n", argc, argv);//carlos debug

	if (ejArgs(argc, argv, T("%s %s"), &name, &address) < 2) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	return websWrite(wp, T("Name: %s, Address %s"), name, address);
}

/******************************************************************************/
/*
 *	Test form for posted data (in-memory CGI). This will be called when the
 *	form in web/forms.asp is invoked. Set browser to "localhost/forms.asp" to test.
 */

static void formTest(webs_t wp, char_t *path, char_t *query)
{
	char_t	*name, *address;
	//printf("formTest wp-url = %s, wp->query = %s\n", wp->url, wp->query);//carlos debug

	name = websGetVar(wp, T("name"), T("Joe Smith")); 
	address = websGetVar(wp, T("address"), T("1212 Milky Way Ave.")); 

	websHeader(wp);
	websWrite(wp, T("<body><h2>Name: %s, Address: %s</h2>\n"), name, address);
	websFooter(wp);
	websDone(wp, 200);
}

/* DLINK add/s [Carlos: 2013/12/10] */
static void quote_mark(char *buf)
{
    char temp[255] = {0};
    int j = 0;
    size_t i = 0;

    for (i = 0; i < strlen(buf); i++)
    {
        if (buf[i] == '"')
        {
            temp[j++] = '\\';
            temp[j++] = '"';
        }
        else if (buf[i] == '\\')
        {
            temp[j++] = '\\';
            temp[j++] = '\\';
        }
        else
            temp[j++] = buf[i];
    }

    temp[j++] = '\0';
    strcpy(buf, temp);

    return;
}

static int webConfigGetUrlPath(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t	*path;
	char_t	value[128] = {0}, *ptr = value;

	if (ejArgs(argc, argv, T("%s"), &path) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return 0;
	}
	xget_str(value, path);
	if ((ptr=strstr(ptr, "://")) && (ptr=strstr(ptr+4, "/")))
		strcpy(value, ptr);
	
	if(value[0] != '\0')
	{
		quote_mark(value);
		websWrite(wp, T("%s"), strChgRssKeywords(value));
	}
	return 1;	
}	
static int webConfigGet(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t	*path;
	char_t	value[128] = {0};
//	printf("webConfigGet argc = %d, ***argv = %s\n", argc, ***argv);
//	printf("webConfigGet argc = %d, *argv = %s\n", argc, *argv);

	if (ejArgs(argc, argv, T("%s"), &path) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return 0;
	}

	xget_str(value, path);
	//LOG_MSG("path=%s, value=%s\n", path, value);

	if(value[0] != '\0')
	{
		quote_mark(value);
		websWrite(wp, T("%s"), value);
	}
	else
	{
		//printf("Error of xget_str(%s)\n", value); //Must close
	}
	return 1;	/* avoid warning */
}

#define WEB_BUF_SIZE 512

static int mConfigGetArrayElement(char *argv, webs_t wp)
{
	u_int16_t rowcount;
	char cp1[256] = {0}, cp2[256] = {0};
	char value[WEB_BUF_SIZE] = {0};
	char num[24] = {0}, num1[8] = {0}, num2[8] = {0};
	char *ptr = NULL, *ptr2 = NULL;
	int init=0;

	//xprintf ("mConfigGetArrayElement(%s)\n", argv);

	/* case 1 */
	if ((ptr = strchr(argv, '#')))
	{
		if (ptr[1] == '\0')
		{
			sscanf(argv, "%[^#]#", cp1);
			cp2[0] = '\0';
		}
		else
			sscanf(argv, "%[^#]#%s", cp1, cp2);

		init = 1;
		xget_row(&rowcount, cp1);
	}
	/* case 2 */
	else if ((ptr = strchr(argv, ':')))
	{
		if (ptr[1] != '\0')
			sscanf(argv, "%[^:]:%s", cp1, cp2);
		else
		{
			printf("This is wrong format! ConfigGetArrayElement(%s)!!\n", argv);
			return 0;
		}

		/*
		 * get number like ConfigGetArrayElement(/aaa/bbb:1) or 
		 * ConfigGetArrayElement(/aaa/bbb:3-5)
		 * ConfigGetArrayElement(/aaa/bbb:num)
		 */
		if ((ptr2 = strchr(cp2, '/')))
		{
			strncpy(num, cp2, abs(ptr2 - cp2));
			if (ptr2[1] == '\0')
				cp2[0] = '\0';
			else
				strcpy(cp2, ptr2);
		}
		else
		{
			strcpy(num, cp2);
			cp2[0] = '\0';
		}

		/* get num1, num2 form num */
		if (strchr(num, '-'))
		{
			sscanf(num, "%[^-]-%s", num1, num2);
			init = atoi(num1);
			rowcount = atoi(num2);
		} else
			init = rowcount = atoi(num);
	}
	/* case 3 */
	else
	{
		printf("This is wrong format! ConfigGetArrayElement(%s)!!\n", argv);
		return 0;
	}

	if (init > rowcount)
	{
		//xprintf ("It should xget_str no data!!!!\n");
		return 0;
	}

	/* call xget_str function */
	for (; init <= rowcount; init++)
	{
		xget_str(value, "%s:%d%s", cp1, init, cp2);
		quote_mark(value);
		//xprintf("xget_str(%s, %s:%d%s)\n", value, cp1, init, cp2);

		if (init != rowcount)
			//fprintf(fd, "\"%s\",", value);
			websWrite(wp, T("\"%s\","), value);
		else
			//fprintf(fd, "\"%s\"", value);
			websWrite(wp, T("\"%s\""), value);
	}

	return 1;	/* avoid warning */
}

static int webGenerateDKey(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("%s"), generate_DKey(wp->ipaddr));
}

static int webConfigRssGet(int eid, webs_t wp, int argc, char_t **argv)
{
	//printf("=== RssVar Get = %s\n", *argv);

	websWrite(wp, T("%s"), getRssVar_Encode(*argv, 2));
}

static int webConfigGetArray(int eid, webs_t wp, int argc, char_t **argv)
{
	//int rowcount;
	unsigned short rowcount;
	char cp1[256] = {0}, cp2[256] = {0};
	char xmlpath[256] = {0};
	char value[WEB_BUF_SIZE] = {0};
	char num[24] = {0}, num1[8] = {0}, num2[8] = {0};
	char *ptr = NULL, *ptr2 = NULL;
	int init=0;
	int i, j;
	char *path;

	//printf("argc = %d, *argv = %s\n", argc, *argv);
	//if (argc < 2)
	if (ejArgs(argc, argv, T("%s"), &path) < 1) 
	{
		//printf("argc = %d, ***argv = %s\n", argc, ***argv);
		websError(wp, 400, T("Insufficient2 args\n"));
		return 0;
	}
	//printf("path = %s\n", path);
	//printf("argv[0] = %s, argv[1] = %s, argv[2] = %s\n", argv[0], argv[1], argv[2]);

  /****************************************************************/
	/* Mutiple entry case ConfigGetArray                            */
	/* For example:                                                 */
	/* <?ConfigGetArray(/nat/dmzsrv#/,dmzenable,dmzip,dmzmask)?>    */
	/* We can decide if it is mutiple or single entry by char '#'.  */
  /****************************************************************/
	if ((ptr = strchr(argv[0], '#')) != NULL)
	{
		//printf("ptr = %s\n", ptr);
		//printf("111111\n");
		if (ptr[1] == '\0')
		{
			sscanf(argv[0], "%[^#]#", cp1);
			cp2[0] = '\0';
		}
		else
			sscanf(argv[0], "%[^#]#%s", cp1, cp2);
		//printf("cp1 = %s, cp2 = %s\n", cp1, cp2);

		/* avoid forget char '/' in the first argument(argv[1]). */
		if (cp2[strlen(cp2) - 1] != '/')
			strcat(cp2, "/");
		xget_row(&rowcount, cp1);
		//printf("rowcount = %d\n", rowcount);

		if (rowcount >= 1000)
			rowcount = 1000;
		/* no this entry, return null string array. */
		if (rowcount == 0)
		{
			//fprintf(fd, "[");
			websWrite(wp, T("["));
			//for (j = 2; j < argc; j++)
			for (j = 1; j < argc; j++)
			{
				if (j != argc - 1)
				{
					if (strchr(argv[j], '#'))
						//fprintf(fd, "[\"\"],");
						websWrite(wp, T("[\"\"]"));
					else
						//fprintf(fd, "\"\",");
						websWrite(wp, T("\"\","));
				}
				else
				{
					if (strchr(argv[j], '#'))
						//fprintf(fd, "[\"\"]");
						websWrite(wp, T("[\"\"]"));
					else
						//fprintf(fd, "\"\"");
						websWrite(wp, T("\"\""));
				}
			}
			
			//fprintf(fd, "]");
			websWrite(wp, T("]"));
			//printf ("It should xget_str no data!!!!\n");
			return 0;
		}

		/* if rowcount >= 1 return multi-entries */
		for (i = 1; i <= rowcount; i++)
		{
			//fprintf(fd, "[");
			websWrite(wp, T("["));
			//for (j = 2; j < argc; j++)
			for (j = 1; j < argc; j++)
			{
				snprintf(xmlpath, WEB_BUF_SIZE, "%s:%d%s%s", cp1, i, cp2, argv[j]);
				//printf("cp1 = %s, i = %d, cp2 = %s, argv[j] = %s", cp1, i, cp2, argv[j]);
				//printf("xmlpath = %s\n", xmlpath);
				/* xprintf("xmlpath = %s, argc = %d\n", xmlpath, argc); */

				if ((ptr = strchr(xmlpath, '#')))
				{
					//fprintf(fd, "[");
					websWrite(wp, T("["));
					//xprintf ("mConfigGetArrayElement(%s)\n", xmlpath);
					if (mConfigGetArrayElement(xmlpath, wp) ==  0)
						//fprintf(fd, "\"\"");
						websWrite(wp, T("\"\""));
					//fprintf(fd, "]");
					websWrite(wp, T("]"));
					if (j != argc - 1)
						//fprintf(fd, ",");
						websWrite(wp, T(","));
				}
				else
				{
					xget_str(value, xmlpath);
					quote_mark(value);
					//xprintf("xget_str(%s, %s)\n", value, xmlpath);

					if (j != argc - 1)
						//fprintf(fd, "\"%s\",", value);
						websWrite(wp, T("\"%s\","),value);
					else
						//fprintf(fd, "\"%s\"", value);
						websWrite(wp, T("\"%s\""),value);
				}
			}

			if (i != rowcount)
				//fprintf(fd, "],\n");
				websWrite(wp, T("],\n"));
			else
				//fprintf(fd, "]\n");
				websWrite(wp, T("]\n"));
		}
	}
  /****************************************************************/
	/* Mutiple entry case ConfigGetArray                            */
	/* For example:                                                 */
	/* <?ConfigGetArray(/nat/dmzsrv:1/,dmzenable,dmzip,dmzmask)?>   */
	/* <?ConfigGetArray(/nat/dmzsrv:3-5/,dmzenable,dmzip,dmzmask)?> */
	/* We can decide if it is mutiple or single entry by char '#'.  */
  /****************************************************************/
	else if ((ptr = strchr(argv[0], ':')) != NULL)
	{
		//printf("2222\n");
		if (ptr[1] != '\0')
			sscanf(argv[0], "%[^:]:%s", cp1, cp2);
		else
		{
			printf("This is wrong format! ConfigGetArray(%s)!!\n", argv[0]);
			return 0;
		}

		if ((ptr2 = strchr(cp2, '/')))
		{
			strncpy(num, cp2, abs(ptr2 - cp2));
			strcpy(cp2, ptr2);
		}
		else
		{
			strcpy(num, cp2);
			cp2[0] = '/';	/* avoid forget char '/' in the first argument(argv[1]). */
		}

		/* get num1, num2 form num */
		if (strchr(num, '-'))
		{
			sscanf(num, "%[^-]-%s", num1, num2);
			init = atoi(num1);
			rowcount = atoi(num2);
		}
		else
			init = rowcount = atoi(num);

		/* no this entry, return null string array. */
		if (init > rowcount)
		{
			//fprintf(fd, "[");
			websWrite(wp, T("["));
			for (j = 2; j < argc; j++)
			{
				if (j != argc - 1)
					websWrite(wp, T("\"\","));
					//fprintf(fd, "\"\",");
				else
					//fprintf(fd, "\"\"");
					websWrite(wp, T("\"\""));
			}
			//fprintf(fd, "]");
			websWrite(wp, T("]"));
			//printf ("It should xget_str no data!!!!\n");
			return 0;
		}

		/* if rowcount >= 1 return multi-entries */
		for (; init <= rowcount; init++)
		{
			//fprintf(fd, "[");
			websWrite(wp, T("["));
			for (j = 2; j < argc; j++)
			{
				xget_str(value, "%s:%d%s%s", cp1, init, cp2, argv[j]);
				quote_mark(value);
			//	xprintf("xget_str(%s, %s:%d%s%s)\n", value, cp1, init, cp2, argv[j]);

				if (j != argc - 1)
					//fprintf(fd, "\"%s\",", value);
					websWrite(wp, T("\"%s\","), value);
				else
					//fprintf(fd, "\"%s\"", value);
					websWrite(wp, T("\"%s\""), value);
			}
			
			if (init != rowcount)
				//fprintf(fd, "],\n");
				websWrite(wp, T("],\n"));
			else
				//fprintf(fd, "]\n");
				websWrite(wp, T("]\n"));
		}
	}
  /****************************************************************/
	/* single entry case ConfigGetArray                             */
	/* For example:                                                 */
	/* <?ConfigGetArray(/lan/ethernet/,ip,netmask,gateway)?>        */
  /****************************************************************/
	else
	{
	//printf("33333\n");
		//fprintf(fd, "[");
		websWrite(wp, T("["));
		for (j = 2; j < argc; j++)
		{
			if (argv[0][strlen(argv[0]) - 1] == '/')
				snprintf(xmlpath, sizeof(xmlpath), "%s%s", argv[0], argv[j]);
			else	/* avoid forget char '/' in the first argument(argv[1]). */
				snprintf(xmlpath, sizeof(xmlpath), "%s/%s", argv[0], argv[j]);

			xget_str(value, xmlpath);
			quote_mark(value);
			/* xprintf("xget_str(%s, %s)\n", value, xmlpath); */

			if (j != argc - 1)
				//fprintf(fd, "\"%s\",", value);
				websWrite(wp, T("\"%s\","), value);
			else
				//fprintf(fd, "\"%s\"", value);
				websWrite(wp, T("\"%s\""), value);
		}
		
		//fprintf(fd, "]");
		websWrite(wp, T("]"));
	}
	/* avoid warning */
	return 0;
}
/* DLINK add/e */

/******************************************************************************/
/*
 *	Home page handler
 */

static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
	int arg, char_t *url, char_t *path, char_t *query)
{
/*
 *	If the empty or "/" URL is invoked, redirect default URLs to the home page
 */
	if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
		websRedirect(wp, WEBS_DEFAULT_HOME);
		return 1;
	}
	return 0;
}

/******************************************************************************/

#ifdef B_STATS
static void memLeaks() 
{
	int		fd;

	if ((fd = gopen(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0) {
		bstats(fd, printMemStats);
		close(fd);
	}
}

/******************************************************************************/
/*
 *	Print memory usage / leaks
 */

static void printMemStats(int handle, char_t *fmt, ...)
{
	va_list		args;
	char_t		buf[256];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	write(handle, buf, strlen(buf));
}
#endif

/* DLINK add/s [Laurel: 2014/02/12] */
#ifdef DNOS_NETWORK_STARTER
void write_fwxml(webs_t wp,int result, int sec)
{
	LOG_DEBUG_TIME("result:%d sec:%d\n",result,sec);
	//websHeader2(wp);
	websWrite(wp, T("<?xml version=\"1.0\" ?>\n"));
	websWrite(wp, T("<root>\n"));
    websWrite(wp, T("<FwUpgdResult>%d</FwUpgdResult>\n"),result);
    websWrite(wp, T("<RssGetWaitSec>%d</RssGetWaitSec>\n"),sec);
    websWrite(wp, T("</root>\n"));
	//websDone(wp, 200);
	//LOG_DEBUG_TIME("done\n");
	
    /* DLINK add/s [Jack: 2014/10/29, generate xmlFile used by rssUI] */
	char buf[2][10];
	sprintf(buf[0], "%d", result);
	sprintf(buf[1], "%d", sec);
	write_TmpXmlFile(FwUpgdResultFileName, "FwUpgdResult", 	buf[0], 0);
	write_TmpXmlFile(FwUpgdResultFileName, "RssGetWaitSec", buf[1], 1);
    /* DLINK add/e [Jack] */
}

do_fwupgrade(webs_t wp)
{
	LOG_DEBUG_TIME("\n");
 //	write_fwxml(wp,0,get_ExternalCall_UseSec(EXTCALL_FwUpgrade));
	if (exe_ExternalCall(EXTCALL_FwUpgrade) == 0)
		write_fwxml(wp,0,get_ExternalCall_UseSec(EXTCALL_FwUpgrade));
	else
		write_fwxml(wp,1,0);
	xset_int(0, PATH_RtFwUpgInfo "/state");// DLINK add [Vincent: 2015/06/10]
	LOG_DEBUG_TIME("done\n");
}

/* DLINK add/e [Jack: 2015/04/29, prepare memory used to store FwUpgrade file] */
void do_prepare_fwupload(webs_t wp)
{
	exe_ExternalCall(EXTCALL_FwUpload);
	xset_int(1, PATH_RtFwUpgInfo "/state");// DLINK add [Vincent: 2015/06/10]
}
/* DLINK add/e [Jack: 2015/04/29] */

static void rssfwupgd(webs_t wp, char_t *path, char_t *query)
{
    FILE *       fp;
    char_t *     fn;
    char_t *     bn = NULL;
    int          locWrite;
    int          numLeft;
    int          numWrite;

	LOG_DEBUG_TIME("wp->url:%s, wp->cookie:'%s'\n", wp->url, wp->cookie);
	char tmp[64] = {0};
	char fwfn[128] = {0};

    /* DLINK add [Jack: 2014/10/29, remove old xmlFile used by rssUI] */
	remove_TmpXmlFile(FwUpgdResultFileName);

    a_assert(websValid(wp));
    websXmlHeader(wp);

	/* DLINK add/s [Jack: 2014/08/20, add Auth Check] */
	// Check Auth of Access
	DHttpd_WebsRec	dWebsRec;
	char *sID = NULL; 
	
	init_DHttpd_WebsRec(wp, &dWebsRec);
	if (!chkByPassLoginPage() && checkUserAuth(&dWebsRec, &sID)) {
		write_fwxml(wp, 2, 0);
		goto rssfwupgd_done;
	}
	/* DLINK add/e [Jack: 2014/08/20] */
/* DLINK mod/s [Vincent: 2015/03/25: For FW upgrading]*/
#if 0
	xget_str(fwfn, PATH_OtTempDir);
	if (fwfn[strlen(fwfn)] != '/') 
        strcat(fwfn, "/");
	xget_str(tmp, PATH_FwUpgInfo "/fwupgradefn");
	if (tmp[0] == '\0') {
		strcpy(tmp, "upld.bin");
        xset_str(tmp, PATH_FwUpgInfo "/fwupgradefn");
    }
	strcat(fwfn, tmp);
	
	LOG_DEBUG("\nsavedfn='%s' ....................................\n\n", fwfn);

    fn = websGetVar(wp, T("filename"), T(""));
    if (fn != NULL && *fn != '\0') {
        if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
            if ((int)(bn = gstrrchr(fn, '\\') + 1) == 1) {
                bn = fn;
            }
        }
    }

	LOG_DEBUG("fn='%s', bn='%s' .............\n", fn, bn);

//    websWrite(wp, T("Filename = %s<br>Size = %d bytes<br>"), bn, wp->lenPostData);

//    if ((fp = fopen((bn == NULL ? "upldForm.bin" : bn), "w+b")) == NULL) {
    if ((fp = fopen(fwfn, "w+b")) == NULL) {
		write_fwxml(wp,1,0);
		//websWrite(wp, T("File open failed!<br>"));
    } else {
        locWrite = 0;
        numLeft = wp->lenPostData;
        while (numLeft > 0) {
            numWrite = fwrite(&(wp->postData[locWrite]), sizeof(*(wp->postData)), numLeft, fp);
            if (numWrite < numLeft) {
				write_fwxml(wp,1,0);
                //websWrite(wp, T("File write failed.<br>  ferror=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes<br>"), ferror(fp), locWrite, numLeft, numWrite, wp->lenPostData);
            break;
            }
            locWrite += numWrite;
            numLeft -= numWrite;
        }

        if (numLeft == 0) {
            if (fclose(fp) != 0) {
				write_fwxml(wp,1,0);
            	//websWrite(wp, T("File close failed.<br>  errno=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes<br>"), errno, locWrite, numLeft, numWrite, wp->lenPostData);
            } else {
                //websWrite(wp, T("File Size Written = %d bytes<br>"), wp->lenPostData);
				//LOG_DEBUG_LINE("");
				//system("/usr/script/starter/fwupgrade.sh");
				do_fwupgrade(wp);
            }
        } else {
            	//websWrite(wp, T("numLeft=%d locWrite=%d Size=%d bytes<br>"), numLeft, locWrite, wp->lenPostData);
				//LOG_DEBUG_LINE(); 
				do_fwupgrade(wp);
				//system("/usr/script/starter/fwupgrade.sh");
        }
    }
#endif
	do_fwupgrade(wp);
/* DLINK mod/e */

rssfwupgd_done:	/* DLINK add [Jack: 2014/08/20, add Auth Check]  */
	//LOG_DEBUG("Done");
//    websFooter(wp);
	free_DHttpdWebsRec(&dWebsRec);
    websDone(wp, 200);
	//websRedirect(wp,RssFwResultFile);
}
#endif
/* DLINK add/e */

/******************************************************************************/
/*
 * for test html upload file to web server
 * add by gyr 2011.10.15
 */

static void formUploadFileTest(webs_t wp, char_t *path, char_t *query)
{
    FILE *       fp;
    char_t *     fn;
    char_t *     bn = NULL;
    int          locWrite;
    int          numLeft;
    int          numWrite;
/*
	char tmp[64];
	char fwfn[128];
	xget_str(fwfn, PATH_OtTempDir);
	strcat(fwfn, "/");
	xget_str(tmp, _OT_Path "/fwupgradefn");
	strcat(fwfn, tmp);
*/

	LOG_DEBUG_TIME("\n");
	char tmp[64] = {0};
    char fwfn[128] = {0};
    xget_str(fwfn, PATH_OtTempDir);
    if (fwfn[strlen(fwfn)] != '/')
        strcat(fwfn, "/");
    xget_str(tmp, PATH_FwUpgInfo "/fwupgradefn");
    if (tmp[0] == '\0') {
        strcpy(tmp, "upld.bin");
        xset_str(tmp, PATH_FwUpgInfo "/fwupgradefn");
    }
    strcat(fwfn, tmp);
	
	LOG_DEBUG("savedfn='%s'...........................\n\n", fwfn);

    a_assert(websValid(wp));
    websHeader(wp);

    fn = websGetVar(wp, T("filename"), T(""));
    if (fn != NULL && *fn != '\0') {
        if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
            if ((int)(bn = gstrrchr(fn, '\\') + 1) == 1) {
                bn = fn;
            }
        }
    }

	LOG_DEBUG("fn=%s, bn=%s  .......\n", fn, bn);

    websWrite(wp, T("Filename = %s<br>Size = %d bytes<br>"), bn, wp->lenPostData);

//    if ((fp = fopen((bn == NULL ? "upldForm.bin" : bn), "w+b")) == NULL) {
    if ((fp = fopen(fwfn, "w+b")) == NULL) {
        websWrite(wp, T("File open failed!<br>"));
    } else {
        locWrite = 0;
        numLeft = wp->lenPostData;
        while (numLeft > 0) {
            numWrite = fwrite(&(wp->postData[locWrite]), sizeof(*(wp->postData)), numLeft, fp);
            if (numWrite < numLeft) {
                websWrite(wp, T("File write failed.<br>  ferror=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes<br>"), ferror(fp), locWrite, numLeft, numWrite, wp->lenPostData);
            break;
            }
            locWrite += numWrite;
            numLeft -= numWrite;
        }

        if (numLeft == 0) {
            if (fclose(fp) != 0) {
                websWrite(wp, T("File close failed.<br>  errno=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes<br>"), errno, locWrite, numLeft, numWrite, wp->lenPostData);
            } else {
                websWrite(wp, T("File Size Written = %d bytes<br>"), wp->lenPostData);
            }
        } else {
            websWrite(wp, T("numLeft=%d locWrite=%d Size=%d bytes<br>"), numLeft, locWrite, wp->lenPostData);
        }
    }

	do_fwupgrade(wp);

    websFooter(wp);
    websDone(wp, 200);
}

/******************************************************************************/
