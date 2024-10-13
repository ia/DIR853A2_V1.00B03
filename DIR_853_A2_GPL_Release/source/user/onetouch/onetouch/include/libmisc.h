#ifndef __LIB_MISC_H__
#define __LIB_MISC_H__

#define SYSCMD_OK 0
#define SYSCMD_ERR -1

extern int lib_RunShell(char * cmd, char * buf, size_t size);
extern int lib_System(const char *format, ...);
#endif /* __LIB_MISC_H__ */
