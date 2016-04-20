#ifndef LIBOS_H
#define LIBOS_H

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"

#ifndef NULL
#define NULL  ((void*)0)
#endif

#ifndef __min
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef TRUE
#define TRUE  (1)
#define FALSE (0)
#endif

#define LwChar(a)    (((a)>='A' && (a)<='Z')?((a)+0x20):(a))

#define cp_zfree(p) if(p){free(p);p=NULL;}

#ifndef WIN32
#define stricmp strcasecmp
#endif

int Cmmn_Strincmp(char *pszStr1, char *pszStr2,int iLen);

char* we_cmmn_strndup(const char *s, int len);

int file2buff(char *pszFileName, char **ppszBuff, int *piLen);

int buff2file(char *buffer, int len, char *pszFileName);

char* we_cmmn_strnchr(const char *s, char c, int len);

char* we_str_merge(char *s, char* t);

void we_getvalue(char* value, int len, char *line, char *key);

char* Ip2Str(unsigned int ip, char *strip);

int _natoi(char *str, int n);

float _natof(char *str, int n);

unsigned int cmmn_date();
int cmmn_diff_days(unsigned int currDate, unsigned int baseDate);

char* timeToStrDateTime();

char* getDeviceID();

#endif
