
#ifndef _MYDLINK_MDB_H
#define _MYDLINK_MDB_H

#include <unistd.h>

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

typedef struct mdb_list {
	char *name;
	int (*get)(const char *name, char *vbuf, int size);
	int (*set)(const char *name, const char *value);
	int (*apply)(int argc, char **argv);
}MDB_LIST;


enum {
	MDB_GET = 0,
	MDB_SET = 1,
	MDB_APPLY = 2
};

/**
* Get attribute value
*
* param attr: attribute name to get
* param val_buf: buffer to put the result string (with '\0').
* param buf_sz: size of given buffer
* return: 0 for no error, else nonzero.
*/
extern int mdb_get (const char *attr, char *val_buf, size_t buf_sz);
/**
* Set attribute value
*
* param attr: attribute name to set
* param attr_val: value string to set (with '\0').
* return: 0 for no error, else nonzero.
*/
extern int mdb_set (const char *attr, const char *attr_val);
/**
* Apply any pending change
*
* return: 0 for no error, else nonzero.
*/
extern int mdb_apply ();

/**
* Definition of exit/return code
*/
#define ERR_NO_ERROR 0 // success
#define ERR_GENERIC 1 // unknown error
#define ERR_BAD_CMD 10 // command isn¡¯t get/set/apply
#define ERR_UNKNOWN_ATTR 11 // unknown attribute is given
#define ERR_BAD_ATTR_FORMAT 12 // invalid attribute value
#define ERR_BUF_TOO_SHORT 13 // given buffer size is not enough
#define ERR_SYSTEM 14 // systemwise error

#define MDB_SUCC   ERR_NO_ERROR
#define MDB_FAIL   ERR_GENERIC

#endif
