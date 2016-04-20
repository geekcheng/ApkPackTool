#include "libos.h"
#include <time.h>

char *we_cmmn_strndup(const char *s, int len)
{
    char *s_new;
    
    if (s != NULL) {
        s_new = (char*)malloc(len + 1);
        if (s_new != NULL) {
            memcpy(s_new, s, len);
            s_new[len] = '\0';
        }
    }
    else {
        s_new = (char*)malloc(1);
        if (s_new != NULL) {
            s_new[0] = '\0';
        }
    }
    
    return s_new;
}

char *
we_cmmn_strnchr(const char *s, char c, int len)
{
    int i;
    
    if(s == NULL)
        return NULL;
    
    for (i = 0; (i < len) && (s[i] != '\0'); i++) {
        if(s[i] == c)
            return (char *)(s + i);
    }
    
    return NULL;
}

int file2buff(char *pszFileName, char **ppszBuff, int *piLen)
{
    FILE *pFile = NULL;    
    char *pcFileBuff = NULL;
    int iInLen = 0;
    
    pFile = fopen(pszFileName, "rb");    
    if(0 == pFile){
        return -1;
    }
    
    fseek(pFile, 0, SEEK_END);    
    iInLen = ftell(pFile);
    pcFileBuff = (char*)malloc(iInLen+1);
    memset(pcFileBuff, 0x00, iInLen+1);
    fseek(pFile, 0, SEEK_SET);    
    fread(pcFileBuff,iInLen, 1, pFile);    
    fclose(pFile);
    
    *ppszBuff = pcFileBuff;
    *piLen = iInLen;
    
    return 0;
}

int buff2file(char *buffer, int len, char *pszFileName)
{
	FILE *pOUT = NULL;	
	remove(pszFileName);
    pOUT = fopen(pszFileName, "ab+");
    fwrite(buffer, len, 1, pOUT);
    fclose(pOUT);
	return 0;
}

char* we_str_merge(char *s, char* t)
{
	static char g_buffer[80]={0};
	strcpy(g_buffer,s);
	strcat(g_buffer,t);
	return g_buffer;
}

void we_getvalue(char* value, int len, char *line, char *key)
{
	char *pST = NULL;
	char *pED = NULL;

	pST = strstr(line, key);
	if(pST){
		pST+=strlen(key);
		pED = strstr(pST,"&");
		if(!pED){
			pED=pST+strlen(pST);
		}
		if(pED-pST>0){
			memcpy(value,pST,pED-pST);
			return;
		}
	}
	
	memset(value,0x00,len);
}

#define cp_upchar(a)    (((a)>='a' && (a)<='z')?((a)-0x20):(a))
int Cmmn_Strincmp(char *pszStr1, char *pszStr2,int iLen)
{
    int iRetval = 0;
    int i=0;
    do {
        if ((NULL == pszStr1 && pszStr2)||(pszStr1 && NULL == pszStr2)) {
            iRetval=-1;
            break;
        }
        if (NULL == pszStr1 && NULL == pszStr2) {
            break;
        }
        if ((!strlen(pszStr1)&&strlen(pszStr2))||(strlen(pszStr1)&&!strlen(pszStr2) )) {
            iRetval=-1;
            break;
        }
        if (!strlen(pszStr1)&&!strlen(pszStr2)) {
            break;
        }
        while(*pszStr1)
        {
            if (i == iLen) {
                break;
            }
            if (0 == *pszStr2) {
                iRetval =1;
                break;
            }
            if (cp_upchar(*pszStr1) - cp_upchar(*pszStr2) >0) {
                iRetval =1;
                break;
            }
            else if (cp_upchar(*pszStr1) - cp_upchar(*pszStr2) <0) {
                iRetval = -1;
                break;
            }
            pszStr1++;
            pszStr2++;
            i++;
        }
        
    } while(0);
    if (i!=iLen) {
        iRetval = -1;
    }
    return iRetval;
}

char* Ip2Str(unsigned int ip, char *strip)
{
    unsigned int temp = 0;
    unsigned int IP_first = 0;
    unsigned int IP_second = 0;
    unsigned int IP_thrid = 0;
    unsigned int IP_fourth = 0;
    
    temp = ip << 8 * 3; 
    IP_first = temp >> 8 * 3;    
    temp = ip << 8 * 2; 
    IP_second= temp >> 8 * 3;    
    temp = ip << 8 * 1; 
    IP_thrid = temp >> 8 * 3;    
    IP_fourth=ip>> 8 * 3;
    
    sprintf(strip,"%d.%d.%d.%d",IP_first,IP_second,IP_thrid,IP_fourth);
    
    return strip;
}

int _natoi(char *str, int n)
{
	char buffer[32] = {0};
	memcpy(buffer,str,__min(n,31));
	return atoi(buffer);
}

float _natof(char *str, int n)
{
	char buffer[32] = {0};
	memcpy(buffer,str,__min(n,31));
	return (float)atof(buffer);
}

typedef union {
    unsigned int uDate;
    struct {
        unsigned char y;
        unsigned char m;
        unsigned char d;
        unsigned char pad;
    }stDate;
}UnYMDH;

unsigned int cmmn_date()
{
    time_t _time = time(NULL);
    struct tm *ptm = localtime(&_time);
    UnYMDH u_ymdh = {0};
    
    u_ymdh.stDate.y = ptm->tm_year;
    u_ymdh.stDate.m = ptm->tm_mon;
    u_ymdh.stDate.d = ptm->tm_mday;
    	
    return u_ymdh.uDate;
}

int cmmn_diff_days(unsigned int currDate, unsigned int baseDate)
{
    struct tm BASE_DATE = {0};
    struct tm CURR_DATE = {0};
    UnYMDH stCurrDate = {0};
    UnYMDH stBaseDate = {0};
    
    stCurrDate.uDate = currDate;
    stBaseDate.uDate = baseDate;
    
    CURR_DATE.tm_year = stCurrDate.stDate.y;
    CURR_DATE.tm_mon  = stCurrDate.stDate.m;
    CURR_DATE.tm_mday = stCurrDate.stDate.d;
    
    BASE_DATE.tm_year = stBaseDate.stDate.y;
    BASE_DATE.tm_mon  = stBaseDate.stDate.m;
    BASE_DATE.tm_mday = stBaseDate.stDate.d;
    
	return (int)difftime(mktime(&CURR_DATE),mktime(&BASE_DATE))/24/3600;
}

char* timeToStrDateTime()
{
    return "2015-02-19 14:51";
}

char* getDeviceID()
{
	return "1387634";
}
