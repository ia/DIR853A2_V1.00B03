#ifndef MY_TOOL_H_
#define MY_TOOL_H_
#include <errno.h>

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02X:%02X:%02X:%02X:%02X:%02X"
#define IP2STR(v) ((v>>24)&0xff), ((v>>16)&0xff), ((v>>8)&0xff), (v&0xff)
#define IPSTR "%d.%d.%d.%d"

#define COLOR_RED     "\033[22;31m"
#define COLOR_GREEN   "\033[22;32m"
#define COLOR_BLUE    "\033[22;34m"
#define COLOR_MAGENTA "\033[22;35m"
#define COLOR_WHITE   "\033[01;37m"
#define COLOR_YELLOW  "\033[01;33m"
#define COLOR_GYAN    "\033[01;36m"

#define COLOR_LIGHTRED   "\033[01;31m"
#define COLOR_LIGHTGREEN "\033[01;32m"
#define COLOR_LIGHTBLUE  "\033[01;34m"
#define COLOR_LIGHTWHITE "\033[m"

void set_dbg_level(char level);
void dlinkshowmsg(char level, const char* text_color, const char* warrmsg, const char* format, ...);
#define MSG_DEBUG(...)  dlinkshowmsg(4, COLOR_LIGHTWHITE, "DEBUG", __VA_ARGS__)
#define MSG_NOTICE(...) dlinkshowmsg(3, COLOR_YELLOW, "NOTICE", __VA_ARGS__)
#define MSG_WARN(...)   dlinkshowmsg(2, COLOR_RED, "WARN", __VA_ARGS__)
#define MSG_ERROR(...)  dlinkshowmsg(1, COLOR_LIGHTRED, "ERROR", __VA_ARGS__)
#define MSG_TRACE(...)  dlinkshowmsg(4, COLOR_GYAN, "TRACE", __VA_ARGS__)

#define TRACECODE MSG_TRACE("Function : %s, Line : %d\n", __FUNCTION__, __LINE__);

unsigned long sys_currtime(void);
unsigned long sys_difftime(unsigned long from,unsigned long to);
void sys_sleep(int msec);

#define LOCAL_COUNTER_ACCURACY	1000	//ms
void local_counter_init(void);
void local_counter_fin(void);
unsigned long get_local_counter_in_sec(void);
unsigned long get_local_counter(void);
void set_local_counter(unsigned long count);

void *mymalloc(size_t size);
void myfree(void *p);
void *myrealloc(void *p, size_t size);
void *mycalloc(size_t num, size_t size);
char *mystrdup(const char *str);

void mac2char(unsigned char *arr, char *str, const char *del);
#endif // MY_TOOL_H_
