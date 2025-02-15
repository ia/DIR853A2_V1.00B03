/* vi: set sw=4 ts=4: */
/*
 * Mini syslogd implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Copyright (C) 2000 by Karl M. Hegbloom <karlheg@debian.org>
 *
 * "circular buffer" Copyright (C) 2001 by Gennady Feldman <gfeldman@gena01.com>
 *
 * Maintainer: Gennady Feldman <gfeldman@gena01.com> as of Mar 12, 2001
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

/*
 * Done in syslogd_and_logger.c:
#include "libbb.h"
#define SYSLOG_NAMES
#define SYSLOG_NAMES_CONST
#include <syslog.h>
*/

#include <paths.h>
#include <sys/un.h>
#include <sys/uio.h>

#if ENABLE_FEATURE_REMOTE_LOG
#include <netinet/in.h>
#endif

#if ENABLE_FEATURE_IPC_SYSLOG
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#endif


#define DEBUG 0

/* MARK code is not very useful, is bloat, and broken:
 * can deadlock if alarmed to make MARK while writing to IPC buffer
 * (semaphores are down but do_mark routine tries to down them again) */
#undef SYSLOGD_MARK

enum {
	MAX_READ = 256,
	DNS_WAIT_SEC = 2 * 60,
};

/* Semaphore operation structures */
struct shbuf_ds {
	int32_t size;   /* size of data - 1 */
	int32_t tail;   /* end of message list */
	char data[1];   /* data/messages */
};

/* Allows us to have smaller initializer. Ugly. */
#define GLOBALS \
	const char *logFilePath;                \
	int logFD;                              \
	/* interval between marks in seconds */ \
	/*int markInterval;*/                   \
	/* level of messages to be logged */    \
	int logLevel;                           \
USE_FEATURE_ROTATE_LOGFILE( \
	/* max size of file before rotation */  \
	unsigned logFileSize;                   \
	/* number of rotated message files */   \
	unsigned logFileRotate;                 \
	unsigned curFileSize;                   \
	smallint isRegular;                     \
) \
USE_FEATURE_REMOTE_LOG( \
	/* udp socket for remote logging */     \
	int remoteFD;                           \
	len_and_sockaddr* remoteAddr;           \
) \
USE_FEATURE_IPC_SYSLOG( \
	int shmid; /* ipc shared memory id */   \
	int s_semid; /* ipc semaphore id */     \
	int shm_size;                           \
	struct sembuf SMwup[1];                 \
	struct sembuf SMwdn[3];                 \
)

struct init_globals {
	GLOBALS
};

struct globals {
	GLOBALS

#if ENABLE_FEATURE_REMOTE_LOG
	unsigned last_dns_resolve;
	char *remoteAddrStr;
#endif

#if ENABLE_FEATURE_IPC_SYSLOG
	struct shbuf_ds *shbuf;
#endif
	time_t last_log_time;
	/* localhost's name. We print only first 64 chars */
	char *hostname;

	/* We recv into recvbuf... */
	char recvbuf[MAX_READ * (1 + ENABLE_FEATURE_SYSLOGD_DUP)];
	/* ...then copy to parsebuf, escaping control chars */
	/* (can grow x2 max) */
	char parsebuf[MAX_READ*2];
	/* ...then sprintf into printbuf, adding timestamp (15 chars),
	 * host (64), fac.prio (20) to the message */
	/* (growth by: 15 + 64 + 20 + delims = ~110) */
	char printbuf[MAX_READ*2 + 128];
};

static const struct init_globals init_data = {
	.logFilePath = "/var/log/messages",
	.logFD = -1,
#ifdef SYSLOGD_MARK
	.markInterval = 20 * 60,
#endif
	.logLevel = 8,
#if ENABLE_FEATURE_ROTATE_LOGFILE
	.logFileSize = 200 * 1024,
	.logFileRotate = 1,
#endif
#if ENABLE_FEATURE_REMOTE_LOG
	.remoteFD = -1,
#endif
#if ENABLE_FEATURE_IPC_SYSLOG
	.shmid = -1,
	.s_semid = -1,
	.shm_size = ((CONFIG_FEATURE_IPC_SYSLOG_BUFFER_SIZE)*1024), // default shm size
	.SMwup = { {1, -1, IPC_NOWAIT} },
	.SMwdn = { {0, 0}, {1, 0}, {1, +1} },
#endif
};

#define G (*ptr_to_globals)
#define INIT_G() do { \
	SET_PTR_TO_GLOBALS(memcpy(xzalloc(sizeof(G)), &init_data, sizeof(init_data))); \
} while (0)


/* Options */
enum {
	OPTBIT_mark = 0, // -m
	OPTBIT_nofork, // -n
	OPTBIT_outfile, // -O
	OPTBIT_loglevel, // -l
	OPTBIT_small, // -S
	USE_FEATURE_ROTATE_LOGFILE(OPTBIT_filesize   ,)	// -s
	USE_FEATURE_ROTATE_LOGFILE(OPTBIT_rotatecnt  ,)	// -b
	USE_FEATURE_REMOTE_LOG(    OPTBIT_remote     ,)	// -R
	USE_FEATURE_REMOTE_LOG(    OPTBIT_locallog   ,)	// -L
	USE_FEATURE_IPC_SYSLOG(    OPTBIT_circularlog,)	// -C
	USE_FEATURE_SYSLOGD_DUP(   OPTBIT_dup        ,)	// -D

	OPT_mark        = 1 << OPTBIT_mark    ,
	OPT_nofork      = 1 << OPTBIT_nofork  ,
	OPT_outfile     = 1 << OPTBIT_outfile ,
	OPT_loglevel    = 1 << OPTBIT_loglevel,
	OPT_small       = 1 << OPTBIT_small   ,
	OPT_filesize    = USE_FEATURE_ROTATE_LOGFILE((1 << OPTBIT_filesize   )) + 0,
	OPT_rotatecnt   = USE_FEATURE_ROTATE_LOGFILE((1 << OPTBIT_rotatecnt  )) + 0,
	OPT_remotelog   = USE_FEATURE_REMOTE_LOG(    (1 << OPTBIT_remote     )) + 0,
	OPT_locallog    = USE_FEATURE_REMOTE_LOG(    (1 << OPTBIT_locallog   )) + 0,
	OPT_circularlog = USE_FEATURE_IPC_SYSLOG(    (1 << OPTBIT_circularlog)) + 0,
	OPT_dup         = USE_FEATURE_SYSLOGD_DUP(   (1 << OPTBIT_dup        )) + 0,
};
#define OPTION_STR "m:nO:l:S" \
	USE_FEATURE_ROTATE_LOGFILE("s:" ) \
	USE_FEATURE_ROTATE_LOGFILE("b:" ) \
	USE_FEATURE_REMOTE_LOG(    "R:" ) \
	USE_FEATURE_REMOTE_LOG(    "L"  ) \
	USE_FEATURE_IPC_SYSLOG(    "C::") \
	USE_FEATURE_SYSLOGD_DUP(   "D"  )
#define OPTION_DECL *opt_m, *opt_l \
	USE_FEATURE_ROTATE_LOGFILE(,*opt_s) \
	USE_FEATURE_ROTATE_LOGFILE(,*opt_b) \
	USE_FEATURE_IPC_SYSLOG(    ,*opt_C = NULL)
#define OPTION_PARAM &opt_m, &G.logFilePath, &opt_l \
	USE_FEATURE_ROTATE_LOGFILE(,&opt_s) \
	USE_FEATURE_ROTATE_LOGFILE(,&opt_b) \
	USE_FEATURE_REMOTE_LOG(    ,&G.remoteAddrStr) \
	USE_FEATURE_IPC_SYSLOG(    ,&opt_C)


/* circular buffer variables/structures */
#if ENABLE_FEATURE_IPC_SYSLOG

#if CONFIG_FEATURE_IPC_SYSLOG_BUFFER_SIZE < 4
#error Sorry, you must set the syslogd buffer size to at least 4KB.
#error Please check CONFIG_FEATURE_IPC_SYSLOG_BUFFER_SIZE
#endif

/* our shared key (syslogd.c and logread.c must be in sync) */
enum { KEY_ID = 0x414e4547 }; /* "GENA" */

static void ipcsyslog_cleanup(void)
{
	if (G.shmid != -1) {
		shmdt(G.shbuf);
	}
	if (G.shmid != -1) {
		shmctl(G.shmid, IPC_RMID, NULL);
	}
	if (G.s_semid != -1) {
		semctl(G.s_semid, 0, IPC_RMID, 0);
	}
}

static void ipcsyslog_init(void)
{
	if (DEBUG)
		printf("shmget(%x, %d,...)\n", (int)KEY_ID, G.shm_size);

	G.shmid = shmget(KEY_ID, G.shm_size, IPC_CREAT | 0644);
	if (G.shmid == -1) {
		bb_perror_msg_and_die("shmget");
	}

	G.shbuf = shmat(G.shmid, NULL, 0);
	if (G.shbuf == (void*) -1L) { /* shmat has bizarre error return */
		bb_perror_msg_and_die("shmat");
	}

	memset(G.shbuf, 0, G.shm_size);
	G.shbuf->size = G.shm_size - offsetof(struct shbuf_ds, data) - 1;
	/*G.shbuf->tail = 0;*/

	// we'll trust the OS to set initial semval to 0 (let's hope)
	G.s_semid = semget(KEY_ID, 2, IPC_CREAT | IPC_EXCL | 1023);
	if (G.s_semid == -1) {
		if (errno == EEXIST) {
			G.s_semid = semget(KEY_ID, 2, 0);
			if (G.s_semid != -1)
				return;
		}
		bb_perror_msg_and_die("semget");
	}
}

/* Write message to shared mem buffer */
static void log_to_shmem(const char *msg, int len)
{
	int old_tail, new_tail;

	if (semop(G.s_semid, G.SMwdn, 3) == -1) {
		bb_perror_msg_and_die("SMwdn");
	}

	/* Circular Buffer Algorithm:
	 * --------------------------
	 * tail == position where to store next syslog message.
	 * tail's max value is (shbuf->size - 1)
	 * Last byte of buffer is never used and remains NUL.
	 */
	len++; /* length with NUL included */
 again:
	old_tail = G.shbuf->tail;
	new_tail = old_tail + len;
	if (new_tail < G.shbuf->size) {
		/* store message, set new tail */
		memcpy(G.shbuf->data + old_tail, msg, len);
		G.shbuf->tail = new_tail;
	} else {
		/* k == available buffer space ahead of old tail */
		int k = G.shbuf->size - old_tail;
		/* copy what fits to the end of buffer, and repeat */
		memcpy(G.shbuf->data + old_tail, msg, k);
		msg += k;
		len -= k;
		G.shbuf->tail = 0;
		goto again;
	}
	if (semop(G.s_semid, G.SMwup, 1) == -1) {
		bb_perror_msg_and_die("SMwup");
	}
	if (DEBUG)
		printf("tail:%d\n", G.shbuf->tail);
}
#else
void ipcsyslog_cleanup(void);
void ipcsyslog_init(void);
void log_to_shmem(const char *msg);
#endif /* FEATURE_IPC_SYSLOG */


/* Print a message to the log file. */
static void log_locally(time_t now, char *msg)
{
	struct flock fl;
	int len = strlen(msg);

#if ENABLE_FEATURE_IPC_SYSLOG
	if ((option_mask32 & OPT_circularlog) && G.shbuf) {
		log_to_shmem(msg, len);
		return;
	}
#endif
	if (G.logFD >= 0) {
		if (!now)
			now = time(NULL);
		if (G.last_log_time != now) {
			G.last_log_time = now; /* reopen log file every second */
			close(G.logFD);
			goto reopen;
		}
	} else {
 reopen:
		G.logFD = device_open(G.logFilePath, O_WRONLY | O_CREAT
					| O_NOCTTY | O_APPEND | O_NONBLOCK);
		if (G.logFD < 0) {
			/* cannot open logfile? - print to /dev/console then */
			int fd = device_open(DEV_CONSOLE, O_WRONLY | O_NOCTTY | O_NONBLOCK);
			if (fd < 0)
				fd = 2; /* then stderr, dammit */
			full_write(fd, msg, len);
			if (fd != 2)
				close(fd);
			return;
		}
#if ENABLE_FEATURE_ROTATE_LOGFILE
		{
			struct stat statf;
			G.isRegular = (fstat(G.logFD, &statf) == 0 && S_ISREG(statf.st_mode));
			/* bug (mostly harmless): can wrap around if file > 4gb */
			G.curFileSize = statf.st_size;
		}
#endif
	}

	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;
	fl.l_type = F_WRLCK;
	fcntl(G.logFD, F_SETLKW, &fl);

#if ENABLE_FEATURE_ROTATE_LOGFILE
	if (G.logFileSize && G.isRegular && G.curFileSize > G.logFileSize) {
		if (G.logFileRotate) { /* always 0..99 */
			int i = strlen(G.logFilePath) + 3 + 1;
			char oldFile[i];
			char newFile[i];
			i = G.logFileRotate - 1;
			/* rename: f.8 -> f.9; f.7 -> f.8; ... */
			while (1) {
				sprintf(newFile, "%s.%d", G.logFilePath, i);
				if (i == 0) break;
				sprintf(oldFile, "%s.%d", G.logFilePath, --i);
				/* ignore errors - file might be missing */
				rename(oldFile, newFile);
			}
			/* newFile == "f.0" now */
			rename(G.logFilePath, newFile);
			fl.l_type = F_UNLCK;
			fcntl(G.logFD, F_SETLKW, &fl);
			close(G.logFD);
			goto reopen;
		}
		ftruncate(G.logFD, 0);
	}
	G.curFileSize +=
#endif
	                full_write(G.logFD, msg, len);
	fl.l_type = F_UNLCK;
	fcntl(G.logFD, F_SETLKW, &fl);
}

static void parse_fac_prio_20(int pri, char *res20)
{
	const CODE *c_pri, *c_fac;

	if (pri != 0) {
		c_fac = facilitynames;
		while (c_fac->c_name) {
			if (c_fac->c_val != (LOG_FAC(pri) << 3)) {
				c_fac++;
				continue;
			}
			/* facility is found, look for prio */
			c_pri = prioritynames;
			while (c_pri->c_name) {
				if (c_pri->c_val != LOG_PRI(pri)) {
					c_pri++;
					continue;
				}
				snprintf(res20, 20, "%s.%s",
						c_fac->c_name, c_pri->c_name);
				return;
			}
			
			snprintf(res20, 20, "%s",
					c_fac->c_name);
			return;
		}
		snprintf(res20, 20, "<%d>", pri);
	}
}

/* len parameter is used only for "is there a timestamp?" check.
 * NB: some callers cheat and supply len==0 when they know
 * that there is no timestamp, short-circuiting the test. */
static void timestamp_and_log(int pri, char *msg, int len)
{
	char *timestamp;
	time_t now;
    struct tm *tm_now;
    char    datetime[200] = {0};
	
	if (len < 16 || msg[3] != ' ' || msg[6] != ' '
	 || msg[9] != ':' || msg[12] != ':' || msg[15] != ' '
	) {
		time(&now);
		tm_now = localtime(&now);

		strftime(datetime, 200, "%Y-%m-%d %H:%M:%S", tm_now);
		timestamp = (char *)&(datetime[0]);

	} else {
		now = 0;
		timestamp = msg;
		msg += 16;
		timestamp[15] = '\0';
		
		time(&now);
		tm_now = localtime(&now);

		strftime(datetime, 200, "%Y-%m-%d %H:%M:%S", tm_now);
		timestamp = (char *)&(datetime[0]);
	}

	if (option_mask32 & OPT_small)
		sprintf(G.printbuf, "%s %s\n", timestamp, msg);
	else {
		char res[20];
		parse_fac_prio_20(pri, res);
		sprintf(G.printbuf, "%s %s\n", timestamp, msg);
	}

	/* Log message locally (to file or shared mem) */
	log_locally(now, G.printbuf);
}

static void timestamp_and_log_internal(const char *msg)
{
	if (ENABLE_FEATURE_REMOTE_LOG && !(option_mask32 & OPT_locallog))
		return;
	timestamp_and_log(LOG_SYSLOG | LOG_INFO, (char*)msg, 0);
}

/* tmpbuf[len] is a NUL byte (set by caller), but there can be other,
 * embedded NULs. Split messages on each of these NULs, parse prio,
 * escape control chars and log each locally. */
static void split_escape_and_log(char *tmpbuf, int len)
{
	char *p = tmpbuf;

	tmpbuf += len;
	while (p < tmpbuf) {
		char c;
		char *q = G.parsebuf;
		int pri = (LOG_USER | LOG_NOTICE);

		if (*p == '<') {
			/* Parse the magic priority number */
			pri = bb_strtou(p + 1, &p, 10);
			if (*p == '>')
				p++;
			if (pri & ~(LOG_FACMASK | LOG_PRIMASK))
				pri = (LOG_USER | LOG_NOTICE);
		}

		while ((c = *p++)) {
			if (c == '\n')
				c = ' ';
			if (!(c & ~0x1f) && c != '\t') {
				*q++ = '^';
				c += '@'; /* ^@, ^A, ^B... */
			}
			*q++ = c;
		}
		*q = '\0';

		/* Now log it */
		if (LOG_PRI(pri) < G.logLevel)
			timestamp_and_log(pri, G.parsebuf, q - G.parsebuf);
	}
}

static void quit_signal(int sig)
{
	timestamp_and_log_internal("syslogd exiting");
	puts("syslogd exiting");
	if (ENABLE_FEATURE_IPC_SYSLOG)
		ipcsyslog_cleanup();
	kill_myself_with_sig(sig);
}

#ifdef SYSLOGD_MARK
static void do_mark(int sig)
{
	if (G.markInterval) {
		timestamp_and_log_internal("-- MARK --");
		alarm(G.markInterval);
	}
}
#endif

/* Don't inline: prevent struct sockaddr_un to take up space on stack
 * permanently */
static NOINLINE int create_socket(void)
{
	struct sockaddr_un sunx;
	int sock_fd;
	char *dev_log_name;

	memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;

	/* Unlink old /dev/log or object it points to. */
	/* (if it exists, bind will fail) */
	strcpy(sunx.sun_path, "/var/syslogd");
	dev_log_name = xmalloc_follow_symlinks("/var/syslogd");
	if (dev_log_name) {
		safe_strncpy(sunx.sun_path, dev_log_name, sizeof(sunx.sun_path));
		free(dev_log_name);
	}
	unlink(sunx.sun_path);

	sock_fd = xsocket(AF_UNIX, SOCK_DGRAM, 0);
	xbind(sock_fd, (struct sockaddr *) &sunx, sizeof(sunx));
	chmod("/var/syslogd", 0666);

	return sock_fd;
}

#if ENABLE_FEATURE_REMOTE_LOG
static int try_to_resolve_remote(void)
{
	if (!G.remoteAddr) {
		unsigned now = monotonic_sec();

		/* Don't resolve name too often - DNS timeouts can be big */
		if ((now - G.last_dns_resolve) < DNS_WAIT_SEC)
			return -1;
		G.last_dns_resolve = now;
		G.remoteAddr = host2sockaddr(G.remoteAddrStr, 514);
		if (!G.remoteAddr)
			return -1;
	}
	return socket(G.remoteAddr->u.sa.sa_family, SOCK_DGRAM, 0);
}
#endif

static void do_syslogd(void) NORETURN;
static void do_syslogd(void)
{
	int sock_fd;
#if ENABLE_FEATURE_SYSLOGD_DUP
	int last_sz = -1;
	char *last_buf;
	char *recvbuf = G.recvbuf;
#else
#define recvbuf (G.recvbuf)
#endif

	/* Set up signal handlers */
	bb_signals(0
		+ (1 << SIGINT)
		+ (1 << SIGTERM)
		+ (1 << SIGQUIT)
		, quit_signal);
	signal(SIGHUP, SIG_IGN);
	/* signal(SIGCHLD, SIG_IGN); - why? */
#ifdef SYSLOGD_MARK
	signal(SIGALRM, do_mark);
	alarm(G.markInterval);
#endif
	sock_fd = create_socket();

	if (ENABLE_FEATURE_IPC_SYSLOG && (option_mask32 & OPT_circularlog)) {
		ipcsyslog_init();
	}

	timestamp_and_log_internal("[SYSLOG]: start BusyBox v" BB_VER);

	for (;;) {
		ssize_t sz;

#if ENABLE_FEATURE_SYSLOGD_DUP
		last_buf = recvbuf;
		if (recvbuf == G.recvbuf)
			recvbuf = G.recvbuf + MAX_READ;
		else
			recvbuf = G.recvbuf;
#endif
 read_again:
		sz = safe_read(sock_fd, recvbuf, MAX_READ - 1);
		if (sz < 0)
			bb_perror_msg_and_die("read from /dev/log");

		/* Drop trailing '\n' and NULs (typically there is one NUL) */
		while (1) {
			if (sz == 0)
				goto read_again;
			/* man 3 syslog says: "A trailing newline is added when needed".
			 * However, neither glibc nor uclibc do this:
			 * syslog(prio, "test")   sends "test\0" to /dev/log,
			 * syslog(prio, "test\n") sends "test\n\0".
			 * IOW: newline is passed verbatim!
			 * I take it to mean that it's syslogd's job
			 * to make those look identical in the log files. */
			if (recvbuf[sz-1] != '\0' && recvbuf[sz-1] != '\n')
				break;
			sz--;
		}
#if ENABLE_FEATURE_SYSLOGD_DUP
		if ((option_mask32 & OPT_dup) && (sz == last_sz))
			if (memcmp(last_buf, recvbuf, sz) == 0)
				continue;
		last_sz = sz;
#endif
#if ENABLE_FEATURE_REMOTE_LOG
		/* We are not modifying log messages in any way before send */
		/* Remote site cannot trust _us_ anyway and need to do validation again */
		if (G.remoteAddrStr) {
			if (-1 == G.remoteFD) {
				G.remoteFD = try_to_resolve_remote();
				if (-1 == G.remoteFD)
					goto no_luck;
			}
			/* Stock syslogd sends it '\n'-terminated
			 * over network, mimic that */
			recvbuf[sz] = '\n';
			/* send message to remote logger, ignore possible error */
			/* TODO: on some errors, close and set G.remoteFD to -1
			 * so that DNS resolution and connect is retried? */
			sendto(G.remoteFD, recvbuf, sz+1, MSG_DONTWAIT,
				    &G.remoteAddr->u.sa, G.remoteAddr->len);
 no_luck: ;
		}
#endif
		if (!ENABLE_FEATURE_REMOTE_LOG || (option_mask32 & OPT_locallog)) {
			recvbuf[sz] = '\0'; /* ensure it *is* NUL terminated */
			split_escape_and_log(recvbuf, sz);
		}
	} /* for (;;) */
#undef recvbuf
}

int syslogd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int syslogd_main(int argc UNUSED_PARAM, char **argv)
{
	char OPTION_DECL;

	INIT_G();
#if ENABLE_FEATURE_REMOTE_LOG
	G.last_dns_resolve = monotonic_sec() - DNS_WAIT_SEC - 1;
#endif

	/* do normal option parsing */
	opt_complementary = "=0"; /* no non-option params */
	getopt32(argv, OPTION_STR, OPTION_PARAM);
#ifdef SYSLOGD_MARK
	if (option_mask32 & OPT_mark) // -m
		G.markInterval = xatou_range(opt_m, 0, INT_MAX/60) * 60;
#endif
	//if (option_mask32 & OPT_nofork) // -n
	//if (option_mask32 & OPT_outfile) // -O
	if (option_mask32 & OPT_loglevel) // -l
		G.logLevel = xatou_range(opt_l, 1, 8);
	//if (option_mask32 & OPT_small) // -S
#if ENABLE_FEATURE_ROTATE_LOGFILE
	if (option_mask32 & OPT_filesize) // -s
		G.logFileSize = xatou_range(opt_s, 0, INT_MAX/1024) * 1024;
	if (option_mask32 & OPT_rotatecnt) // -b
		G.logFileRotate = xatou_range(opt_b, 0, 99);
#endif
#if ENABLE_FEATURE_IPC_SYSLOG
	if (opt_C) // -Cn
		G.shm_size = xatoul_range(opt_C, 4, INT_MAX/1024) * 1024;
#endif

	/* If they have not specified remote logging, then log locally */
	if (ENABLE_FEATURE_REMOTE_LOG && !(option_mask32 & OPT_remotelog))
		option_mask32 |= OPT_locallog;

	/* Store away localhost's name before the fork */
	G.hostname = safe_gethostname();
	*strchrnul(G.hostname, '.') = '\0';

	if (!(option_mask32 & OPT_nofork)) {
		bb_daemonize_or_rexec(DAEMON_CHDIR_ROOT, argv);
	}
	umask(0);
	write_pidfile("/var/run/syslogd.pid");
	do_syslogd();
	/* return EXIT_SUCCESS; */
}

/* Clean up. Needed because we are included from syslogd_and_logger.c */
#undef G
#undef GLOBALS
#undef INIT_G
#undef OPTION_STR
#undef OPTION_DECL
#undef OPTION_PARAM
