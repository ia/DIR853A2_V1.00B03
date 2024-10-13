#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdarg.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>



#define SERVER_NAME "httpd"
//#define SERVER_PORT 80
#define PROTOCOL "HTTP/1.1"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define TIMEOUT	6

/* A multi-family sockaddr. */
typedef union {
    struct sockaddr sa;
    struct sockaddr_in sa_in;
} usockaddr;

typedef FILE * webs_t;

#define DEFAULT_HTTP_PORT 8182


void handle_request(int conn_fd);


static int
initialize_listen_socket( usockaddr* usaP )
{
    int listen_fd;
    int i;

    memset( usaP, 0, sizeof(usockaddr) );
    usaP->sa.sa_family = AF_INET;
   // usaP->sa_in.sin_addr.s_addr = htonl( INADDR_ANY );
    usaP->sa_in.sin_port = htons( DEFAULT_HTTP_PORT );

    listen_fd = socket( usaP->sa.sa_family, SOCK_STREAM, 0 );
    if ( listen_fd < 0 )
   {
	perror( "socket" );
	return -1;
   }
    //wwzh 2008-01-10(void) fcntl( listen_fd, F_SETFD, 1 );
    i = 1;
    if ( setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i) ) < 0 )
	{
	close(listen_fd);
	perror( "setsockopt" );
	return -1;
	}
	i = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&i, sizeof(i)) < 0)
	{
		close(listen_fd);
		perror( "setsockopt" );
		return -1;
	}
	
    if ( bind( listen_fd, &usaP->sa, sizeof(struct sockaddr_in) ) < 0 )
	{
	close(listen_fd);
	perror( "bind" );
	return -1;
	}
    if ( listen( listen_fd, 1024 ) < 0 )
	{
	close(listen_fd);
	perror( "listen" );
	return -1;
	}
    return listen_fd;
}
int
wfprintf(FILE *fp, char *fmt,...)
{
	va_list args;
	char buf[10240];
	int ret;
	
	va_start(args, fmt);	
	vsnprintf(buf, sizeof(buf), fmt, args);

	ret = fprintf(fp, "%s", buf);	
        va_end(args);
	return ret;
}
int
wfflush(FILE *fp)
{

	return fflush(fp);
}

int
wfclose(FILE *fp)
{

	return fclose(fp);
}


char *
wfgets(char *buf, int len, FILE *fp)
{
	return  fgets(buf, len, fp);
}


int
httpwaitfor(int fd)
{
	fd_set rfds;
	struct timeval tv = { 0, 20000 };
		
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	return select(fd + 1, &rfds, NULL, NULL, &tv);
}



static void
send_headers(webs_t conn_fp, int status, char* title, char* extra_header, char* mime_type, int length )
{
	time_t now;
	char timebuf[100];

	
	wfprintf( conn_fp, "%s %d %s\r\n", PROTOCOL, status, title );
	wfprintf( conn_fp, "Server: %s\r\n", SERVER_NAME );
	now = time( (time_t*) 0 );
	strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
	wfprintf( conn_fp, "Date: %s\r\n", timebuf );
	wfprintf(conn_fp, "Accept-Ranges: bytes\r\n");
	wfprintf(conn_fp, "Keep-Alive: timeout=60\r\n");
	
	//if ( extra_header != (char*) 0 )
	if ( extra_header != (char *)0 && strlen(extra_header) > 6)
	wfprintf( conn_fp, "%s\r\n", extra_header );
	if ( mime_type != (char*) 0 )
		wfprintf( conn_fp, "Content-Type: %s\r\n",  mime_type );

	if( length > 0) //for backup
	{
		//wfprintf( conn_fp, "Content-Length: %d\r\n", sizeof(IMG_HEADER) + CONFIG_SIZE);
		wfprintf( conn_fp, "Content-Length: %d\r\n", length);
	}
	//wfprintf(conn_fp, "Last-Modified: Wed, 06 Jun 2012 17:32:24 GMT\r\n");
	
	wfprintf( conn_fp, "\r\n" );
}


static void
send_error(webs_t conn_fp, int status, char* title, char* extra_header, char* text )
{
	send_headers(conn_fp, status, title, extra_header, "text/html" , 0);
	(void) wfprintf( conn_fp, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\n<BODY BGCOLOR=\"#cc9999\"><H4>%d %s</H4>\n", status, title, status, title );
	(void) wfprintf( conn_fp, "%s\n", text );
	(void) wfprintf( conn_fp, "</BODY></HTML>\n" );
	(void) wfflush( conn_fp );
} 
static char *query;	/* URL after '?' */

static void
unescape(char *s)
{
	unsigned int c;

	while ((s = strpbrk(s, "%+"))) {
		/* Parse %xx */
		if (*s == '%') {
			sscanf(s + 1, "%02x", &c);
			*s++ = (char) c;
			strncpy(s, s + 2, strlen(s) + 1);
		}
		/* Space is special */
		else if (*s == '+')
			*s++ = ' ';
	}
}	   


void
init_cgi(char *q)
{
	int len;
	
	query = q;
	if (!query) {
		len = 0;
		return;
	}
	len = strlen(query);

	/* Parse into individual assignments */
	while (strsep(&q, "&;"));

	/* Unescape each assignment */
	for (q = query; q < (query + len);) {
		unescape(q);
		for (q += strlen(q); q < (query + len) && !*q; q++);
	}
}


static int
match_one( const char* pattern, int patternlen, const char* string )
{
	const char* p;

	for ( p = pattern; p - pattern < patternlen; ++p, ++string )
	{
	if ( *p == '?' && *string != '\0' )
	    continue;
	if ( *p == '*' )
	    {
	    int i, pl;
	    ++p;
	    if ( *p == '*' )
		{
		/* Double-wildcard matches anything. */
		++p;
		i = strlen( string );
		}
	    else
		/* Single-wildcard matches anything but slash. */
		i = strcspn( string, "/" );
	    pl = patternlen - ( p - pattern );
	    for ( ; i >= 0; --i )
		if ( match_one( p, pl, &(string[i]) ) )
		    return 1;
	    return 0;
	    }
	if ( *p != *string )
	    return 0;
	}
	if ( *string == '\0' )
		return 1;
	return 0;
}


/* Simple shell-style filename matcher.  Only does ? * and **, and multiple
** patterns separated by |.  Returns 1 or 0.
*/
int
match( const char* pattern, const char* string )
{
	const char* or;

	for (;;)
	{
	or = strchr( pattern, '|' );
	if ( or == (char*) 0 )
	    return match_one( pattern, strlen( pattern ), string );
	if ( match_one( pattern, or - pattern, string ) )
	    return 1;
	pattern = or + 1;
	}
}

	

int main(int argc, char **argv)
{
	usockaddr usa;
	int listen_fd;
	int conn_fd;
	
	socklen_t sz = sizeof(usa);
	
	signal(SIGPIPE, SIG_IGN);

	/* Initialize listen socket */
	if ((listen_fd = initialize_listen_socket(&usa)) < 0) {
		printf("Cann't bind to any address\n");
		exit(errno);
	}
#if 1
	if (daemon(0, 0) == -1) {
		perror("daemon");
		exit(errno);
	}
#endif
	
	/* Loop forever handling requests */
	for (;;) {

		int onkeep;
		int flags;
		int sndbuf = 65535;
		
		if ((conn_fd = accept(listen_fd, &usa.sa, &sz)) < 0) {
			perror("accept");
			return errno;
		}
		
		
		onkeep = 1;
		//setsockopt(conn_fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&onkeep, sizeof(onkeep));
		setsockopt(conn_fd, SOL_SOCKET,SO_SNDBUF,&sndbuf,sizeof(sndbuf));
		
		handle_request(conn_fd);
		
		
	}

	shutdown(listen_fd, 2);
	close(listen_fd);
}

void response_infocgi(webs_t wp)
{
	char infodata[512];
	int len;
	char macstr[31];
	int i;
	
	memset((void *)infodata, 0, sizeof(infodata));
	memset((void *)macstr, 0, sizeof(macstr));
	strncpy(macstr, nvram_safe_get("lan0_hwaddr"), sizeof(macstr));

	len = snprintf(infodata, sizeof(infodata), "model=%s\n", nvram_safe_get("model_name"));
	len += snprintf(&infodata[len], sizeof(infodata) - len, "version=%s%s\n", 
		      nvram_safe_get("sw_ex_version"), nvram_safe_get("sw_in_version"));
	len += snprintf(&infodata[len], sizeof(infodata) - len, "macaddr=%s\n", macstr);
	len += snprintf(&infodata[len], sizeof(infodata) - len, "cam_conn=0\n");

	send_headers(wp, 200, "OK", NULL, "text/plain", len);
	wfprintf(wp, "%s", infodata);
}


static char http_getdata[4095];
static int http_head_offset = 0;

int 
wfheadergets(char *buf, int len, FILE *fp)
{
	char *ptr;
	int nn = 0;
	
	ptr = strstr(&http_getdata[http_head_offset], "\r\n");
	if (ptr)
	{
		nn = ptr - &http_getdata[http_head_offset] + 2;
		if (nn > len)
			nn = len;
		memcpy(buf, &http_getdata[http_head_offset], nn - 2);
		buf[nn - 1] = '\0';
		buf[nn - 2] = '\0';
		http_head_offset += nn;
		if (http_head_offset >= sizeof(http_getdata))
			http_head_offset = sizeof(http_getdata) - 1;
		return http_head_offset;
	}
	return 0;
	
}

void handle_request(int conn_fd)
{
	webs_t conn_fp;
	char line[8192];
	char *cur;
    char *method, *path, *protocol, *authorization, *boundary;
    char *cp;
    char *file;
	int ii;
    int len;
    int ret;
    int cl = 0, flags;
	char filename[254];
	char matchname[254];
	int body_length = 0;
		
	
	if (!(conn_fp = fdopen(conn_fd, "r+"))) {
		perror("fdopen");
		goto OUT2;
	}
	//if (waitfor(fileno(conn_fp), TIMEOUT) <= 0){
	if (httpwaitfor(fileno(conn_fp)) <= 0){
		fprintf(stderr, "waitfor timeout %d secs\n", TIMEOUT);
		//save2log("waitfor timeout HTTPD ...\n");
		//nvram_set("action_service", "restart_httpd");
		//kill(1, SIGUSR1);
		return;
	}
 	memset((void *)http_getdata, 0, sizeof(http_getdata));
	http_head_offset = 0;
	if ((flags = fcntl(fileno(conn_fp), F_GETFL)) != -1 &&
				fcntl(fileno(conn_fp), F_SETFL, flags | O_NONBLOCK) != -1) {
				    /* Read up to two more characters */
			while(1)
			{
				ret = recv(fileno(conn_fp), &http_getdata[http_head_offset], 1, 0);
				if (ret <= 0)
					break;
					
				http_head_offset++;
				if (http_head_offset >= 4)
				{
					if (http_getdata[http_head_offset - 1] == '\n'
						&& http_getdata[http_head_offset - 2] == '\r'
						&& http_getdata[http_head_offset - 3] == '\n'
						&& http_getdata[http_head_offset - 4] == '\r')
						break;
				}
				
				if (http_head_offset >= sizeof(http_getdata))
					break;
			}
			//printf("ret ====== %d\n", http_head_offset);
			//printf("%s\n", http_getdata);
			fcntl(fileno(conn_fp), F_SETFL, flags);
	} 
    http_head_offset = 0;

	memset((void *)line, 0, sizeof(line));
    /* Parse the first line of the request. */
    if ( wfheadergets( line, sizeof(line), conn_fp ) == 0 ) {	
	    send_error(conn_fp, 400, "Bad Request", (char*) 0, "No request found." );
	    return;
    }
	
	
    /* To prevent http receive https packets, cause http crash  */	
    if ( strncasecmp(line, "GET", 3) && strncasecmp(line, "POST", 4)) {
	fprintf(stderr, "Bad Request!\n");
	return;
    }

    method = path = line;
    strsep(&path, " ");
    if (!path) {	// Avoid http server crash, 
	send_error(conn_fp, 400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    while (*path == ' ') path++;
    protocol = path;
    strsep(&protocol, " ");
    if (!protocol) {	// Avoid http server crash,
	send_error(conn_fp,  400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    while (*protocol == ' ') protocol++;
    cp = protocol;
    strsep(&cp, " ");
    cur = protocol + strlen(protocol) + 1;
    
    /* Parse the rest of the request headers. */
 
    while ( wfheadergets( cur, line + sizeof(line) - cur, conn_fp ) != 0 )
    {
		
		if ( strcmp( cur, "\n" ) == 0 || strcmp( cur, "\r\n" ) == 0 ){
		    break;
		}
		else if ( strncasecmp( cur, "Authorization:", 14 ) == 0 )
		    {
		    cp = &cur[14];
		    cp += strspn( cp, " \t" );
		    authorization = cp;
		    cur = cp + strlen(cp) + 1;
		    }
		else if (strncasecmp( cur, "Content-Length:", 15 ) == 0) {
			cp = &cur[15];
			cp += strspn( cp, " \t" );
			cl = strtoul( cp, NULL, 0 );
		}
		
		else if ((cp = strstr( cur, "boundary=" ))) {
	            boundary = &cp[9];
		    for( cp = cp + 9; *cp && *cp != '\r' && *cp != '\n'; cp++ );
		    *cp = '\0';
		    cur = ++cp;
		}
		
		cur = &http_getdata[http_head_offset];
	}
    
	
    if ( strcasecmp( method, "get" ) != 0 && strcasecmp(method, "post") != 0 ) {
	send_error(conn_fp,  501, "Not Implemented", (char*) 0, "That method is not implemented." );
	return;
    }
	
    if ( path[0] != '/' ) {
	send_error(conn_fp,  400, "Bad Request", (char*) 0, "Bad filename." );
	return;
    }
	
    file = &(path[1]);
#if 0
    len = strlen( file );
	printf("file = %s\n", file);
    if ( file[0] == '/' || strcmp( file, ".." ) == 0 || strncmp( file, "../", 3 ) == 0 || strstr( file, "/../" ) != (char*) 0 || strcmp( &(file[len-3]), "/.." ) == 0 ) {
	send_error(conn_fp, 400, "Bad Request", (char*) 0, "Illegal filename." );
	return;
    }
	init_cgi(file);
#endif

 	if (strcmp(file, "common/info.cgi") == 0)
		response_infocgi(conn_fp);
	else
		send_error(conn_fp,  501, "Not Implemented", (char*) 0, "That method is not implemented." );
	

OUT:	
	wfflush(conn_fp); 
	wfclose(conn_fp); 
	
OUT2:
	close(conn_fd);
	
	
}






