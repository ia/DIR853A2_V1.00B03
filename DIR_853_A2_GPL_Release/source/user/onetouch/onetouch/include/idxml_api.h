/****************************************************************************/
/*                                                                          */
/* IDXML_API.H                                                              */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*    This file defines config APIs dxmlGet[Int, Str], dxmlSet[Int, Str], dxmlGetRow and dxmlDel             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#ifndef __IDXML_API_H__
#define __IDXML_API_H__

#include "idxml.h"
#include "libmisc.h"

char *dxmlGetStr(char *buf, const char *format, ...);
char *dxmlGetEncryStr(char *buf, const char *format, ...);
int dxmlSetStr(char *buf, const char *format, ...);
int dxmlSetFile(char *filename);
signed int dxmlGetInt(const char *format, ...);
int dxmlSetInt(int set_value, const char *format, ...);
int dxmlGetRow(unsigned short *rowcount, const char *format, ...);
int dxmlGetNoe(const char * format, ...);
int dxmlDel(const char *format, ...);

#endif /* __IDXML_API_H__ */

