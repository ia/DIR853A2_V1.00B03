/* 
 *	wsIntrn.h -- Internal GoAhead Web server header
 *
 * Copyright (c) GoAhead Software Inc., 1992-2010. All Rights Reserved.
 *
 *	See the file "license.txt" for information on usage and redistribution
 *
 */
 
#ifndef _h_WEBS_INTERNAL
#define _h_WEBS_INTERNAL 1

/******************************** Description *********************************/

/* 
 *	Internal GoAhead Web Server header. This defines the Web private APIs
 *	Include this header when you want to create URL handlers.
 */

/*********************************** Defines **********************************/

/********************************** Includes **********************************/

#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdarg.h>

#ifdef NETWARE
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<io.h>
#endif

#ifdef WIN
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<io.h>
#define localtime_r(A, B)	localtime_s(B,A)
	#include	<share.h>
#define snprintf			_snprintf
#endif

#ifdef NW
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#ifdef SCOV5
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
#endif

#ifdef LYNX
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
#endif

#ifdef UNIX
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
#endif

#ifdef QNX4
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
	#include	<unix.h>
#endif

#ifdef UW
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#ifdef VXWORKS
	#include	<vxWorks.h>
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#ifdef SOLARIS
	#include	<macros.h>
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#include	"uemf.h"
#include	"ejIntrn.h"
#include	"webs.h"

/* DLINK add/s */
#include 	"dhttpd.h"
#include 	"auth.h"
#include    "debug_log.h"
/* DLINK add/e */

/********************************** Defines ***********************************/
/* 
 *	Read handler flags and state
 */
#define WEBS_BEGIN			0x1			/* Beginning state */
#define WEBS_HEADER			0x2			/* Ready to read first line */
#define WEBS_POST			0x4			/* POST without content */
#define WEBS_POST_CLEN		0x8			/* Ready to read content for POST */
#define WEBS_PROCESSING		0x10		/* Processing request */
#define WEBS_KEEP_TIMEOUT	15000		/* Keep-alive timeout (15 secs) */
#define WEBS_TIMEOUT		60000		/* General request timeout (60) */

#define PAGE_READ_BUFSIZE	512			/* bytes read from page files */
#define MAX_PORT_LEN		10			/* max digits in port number */
#define WEBS_SYM_INIT		64			/* initial # of sym table entries */

/*
 *	URL handler structure. Stores the leading URL path and the handler
 *	function to call when the URL path is seen.
 */ 
typedef struct {
	int		(*handler)(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
			char_t *url, char_t *path, 
			char_t *query);					/* Callback URL handler function */
	char_t	*webDir;						/* Web directory if required */
	char_t	*urlPrefix;						/* URL leading prefix */
	int		len;							/* Length of urlPrefix for speed */
	int		arg;							/* Argument to provide to handler */
	int		flags;							/* Flags */
} websUrlHandlerType;

/* 
 *	Webs statistics
 */
typedef struct {
	long			errors;					/* General errors */
	long			redirects;
	long			net_requests;
	long			activeNetRequests;
	long			activeBrowserRequests;
	long 			timeouts;
	long			access;					/* Access violations */
	long 			localHits;
	long 			remoteHits;
	long 			formHits;
	long 			cgiHits;
	long 			handlerHits;
} websStatsType;

extern websStatsType websStats;				/* Web access stats */

/* 
 *	Error code list
 */
typedef struct {
	int		code;							/* HTTP error code */
	char_t	*msg;							/* HTTP error message */
} websErrorType;

/* 
 *	Mime type list
 */
typedef struct {
	char_t	*type;							/* Mime type */
	char_t	*ext;							/* File extension */
} websMimeType;

/*
 *	File information structure.
 */
typedef struct {
	unsigned long	size;					/* File length */
	int				isDir;					/* Set if directory */
	time_t			mtime;					/* Modified time */
} websStatType;

/*
 *	Compiled Rom Page Index
 */
typedef struct {
	char_t			*path;					/* Web page URL path */
	unsigned char	*page;					/* Web page data */
	int				size;					/* Size of web page in bytes */
	int				pos;					/* Current read position */
} websRomPageIndexType;

/*
 *	Defines for file open.
 */
#ifndef CE
#define	SOCKET_RDONLY	O_RDONLY
#define	SOCKET_BINARY	O_BINARY
#else /* CE */
#define	SOCKET_RDONLY	0x1
#define	SOCKET_BINARY	0x2
#endif /* CE */

extern websRomPageIndexType	websRomPageIndex[];
extern websMimeType		websMimeList[];		/* List of mime types */
extern sym_fd_t			websMime;			/* Set of mime types */
extern webs_t*			webs;				/* Session list head */
extern int				websMax;			/* List size */
extern char_t			websHost[64];		/* Name of this host */
extern char_t			websIpaddr[64];		/* IP address of this host */
extern char_t			*websHostUrl;		/* URL for this host */
extern char_t			*websIpaddrUrl;		/* URL for this host */
extern int				websPort;			/* Port number */

/******************************** Prototypes **********************************/

extern int		 websAspOpen();
extern void		 websAspClose();
extern void		 websFormOpen();
extern void		 websFormClose();
extern int		 websAspWrite(int ejid, webs_t wp, int argc, char_t **argv);

extern void  	 websDefaultOpen();
extern void  	 websDefaultClose();
#ifdef WEBS_WHITELIST_SUPPORT
#define WHITELIST_SSL       0x001   /* File only accessible through https */
#define WHITELIST_CGI       0x002   /* Node is in the cgi-bin dir */
extern int		websBuildWhitelist(void);
extern int		websWhitelistCheck(char *path);
extern void		websDeleteWhitelist(void);
#endif /* WEBS_WHITELIST_SUPPORT */
extern int 		 websDefaultHandler(webs_t wp, char_t *urlPrefix, 
					char_t *webDir, int arg, char_t *url, char_t *path, 
					char_t *query);
extern int 		 websFormHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, char_t *url, char_t *path, char_t *query);
extern int 		 websCgiHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
					int arg, char_t *url, char_t *path, char_t *query);
extern void		 websCgiCleanup();
extern int		 websCheckCgiProc(int handle);
extern char_t	 *websGetCgiCommName();

extern int		 websLaunchCgiProc(char_t *cgiPath, char_t **argp,
					char_t **envp, char_t *stdIn, char_t *stdOut);
extern int 		 websOpen(int sid);
extern void 	 websResponse(webs_t wp, int code, char_t *msg, 
					char_t *redirect);
extern int 		 websJavaScriptEval(webs_t wp, char_t *script);
extern int 		 websPageReadData(webs_t wp, char *buf, int nBytes);
extern int		 websPageOpen(webs_t wp, char_t *lpath, char_t *path, int mode,
					int perm);
extern void		 websPageClose(webs_t wp);
extern void		 websPageSeek(webs_t wp, long offset);
extern int 	 	 websPageStat(webs_t wp, char_t *lpath, char_t *path,
					websStatType *sbuf);
extern int		 websPageIsDirectory(char_t *lpath);
extern int 		 websRomOpen();
extern void		 websRomClose();
extern int 		 websRomPageOpen(webs_t wp, char_t *path, int mode, int perm);
extern void 	 websRomPageClose(int fd);
extern int 		 websRomPageReadData(webs_t wp, char *buf, int len);
extern int 	 	 websRomPageStat(char_t *path, websStatType *sbuf);
extern long		 websRomPageSeek(webs_t wp, long offset, int origin);
extern void 	 websSetRequestSocketHandler(webs_t wp, int mask, 
					void (*fn)(webs_t wp));
extern int 		 websSolutionHandler(webs_t wp, char_t *urlPrefix,
					char_t *webDir, int arg, char_t *url, char_t *path, 
					char_t *query);
extern void 	 websUrlHandlerClose();
extern int 		 websUrlHandlerOpen();
extern int 		 websOpenServer(int port, int retries);
extern void 	 websCloseServer();
extern char_t*	 websGetDateString(websStatType* sbuf);

extern int		strcmpci(char_t* s1, char_t* s2);

#ifdef CE
extern int writeUniToAsc(int fid, void *buf, unsigned int len);
extern int readAscToUni(int fid, void **buf, unsigned int len);
#endif

/* DLINK add/s */
#define xget_str 		dxmlGetStr
#define xget_encrystr	dxmlGetEncryStr
#define xset_str		dxmlSetStr
#define xset_file		dxmlSetFile
#define xget_int		dxmlGetInt
#define xset_int		dxmlSetInt
#define xget_row		dxmlGetRow
#define xget_noe		dxmlGetNoe
#define xdel 			dxmlDel

#ifndef __IDXML_API_H__ 
extern char *dxmlGetStr(char *buf, const char *format, ...);
extern char *dxmlGetEncryStr(char *buf, const char *format, ...);
extern int dxmlSetStr(char *buf, const char *format, ...);
extern int dxmlSetFile(char *filename);
extern signed int dxmlGetInt(const char *format, ...);
extern int dxmlSetInt(int set_value, const char *format, ...);
extern int dxmlGetRow(unsigned short *rowcount, const char *format, ...);
extern int dxmlGetNoe(const char * format, ...);
extern int dxmlDel(const char *format, ...);
#endif /* __IDXML_API_H__ */
/* DLINK add/e */


#endif /* _h_WEBS_INTERNAL */

/******************************************************************************/

