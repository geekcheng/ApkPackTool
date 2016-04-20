#include "libos.h"
#include "direct.h"
#include "stdarg.h"
#include "time.h"


char* TimeToStrDateTime(char *strz)
{
    time_t t = time(NULL);
    struct tm *ptm = localtime(&t);
    
    sprintf(strz,"%04d-%02d-%02d %02d:%02d:%02d",ptm->tm_year+1900, ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
    
    return strz;
}

#define LOGFILENAME  "./tool/log.txt"
void mYdump(char* pszFormat, ...)
{
	static char nPrintableStr[3*1024] = {0};
	va_list MyList;
    
    memset(nPrintableStr, 0x00, 3*1024);  
    
    va_start(MyList, pszFormat);
    _vsnprintf(nPrintableStr, 3*1024-1, pszFormat, MyList);
    va_end(MyList);
	
	FILE* hFile = NULL;
    char timeSTR[32] = {0};
    
    hFile = fopen(LOGFILENAME, "ab+");
    if (NULL != hFile) {
        fseek(hFile, 0, SEEK_END);
        fprintf(hFile,"%s %s" ,TimeToStrDateTime(timeSTR), nPrintableStr);
        fclose(hFile);
    }
}


void mYlog(char* pszFormat, ...)
{ 
	static char nPrintableStr[3*1024] = {0};
    va_list MyList;
    
    memset(nPrintableStr, 0x00, 3*1024);  
    
    va_start(MyList, pszFormat);
    _vsnprintf(nPrintableStr, 3*1024-1, pszFormat, MyList);
    va_end(MyList);
	
	fprintf(stdout,nPrintableStr);
	fflush(stdout);

	mYdump(nPrintableStr);
}


#define LOGI  mYdump
#define LOGE  mYlog


static int convertSmali(char *buffer, int len, char *CHID)
{
	char *pszST = strstr(buffer, "method public initPayAgent()V");
	if(!pszST){
		LOGI("cant find initPayAgent");
		return 0;
	}
	pszST = strstr(pszST, "Lcom/door/frame/DnPayServer;->getInstance()Lcom/door/frame/DnPayServer");
	if(!pszST){
		LOGI("Error(2)");
		return 0;
	}

	pszST = strstr(pszST, "const-string v3, \"");
	if(!pszST){
		LOGI("Error(3)");
		return 0;
	}
	
	pszST += strlen("const-string v3, \"");
	char *pszED = strstr(pszST, "\"");

	if((unsigned int)(pszED-pszST)!=strlen(CHID)){
		LOGI("Error(4)");
		return -1;
	}

	memcpy(pszST,CHID,strlen(CHID));

	return 0;
}


static int hackSmali(char *smali, char *CID)
{
	LOGI("Info: SMALI FILE path NAME = %s\r\n", smali);
	LOGI("Covert CID to %s\r\n", CID);

	char *buffer = NULL;
	int len = 0;
	if(0!=file2buff(smali,&buffer,&len)){
		return 0;
	}
	
	if(0!=convertSmali(buffer,len,CID)){
		LOGI("-ERROR2\r\n");
		return -1;
	}
	
	buff2file(buffer,len,smali);
	free(buffer);
	
	LOGI("Hack Smali (CID=%s)SUCCESS\r\n", CID);
	return 0;
}


static int convertAndroidManifest(char *buffer, int len, char *CHANNEL, char *CID)
{
	// 1. DOOR-CHANNEL
	char *pszST = strstr(buffer, "meta-data android:name=\"DOOR_CHANNEL\"");
	char *pszED = NULL;
	if(pszST){
		pszST += strlen("meta-data android:name=\"DOOR_CHANNEL\""); // SKIP
		pszST = strstr(pszST,"android:value=\"");
		pszST += strlen("android:value=\"");
		if(*pszST=='C'){
			pszST++; // SKIP olo C-BEGINs channel style
		}
		pszED = strstr(pszST,"\"");
		if((unsigned int)(pszED-pszST)==strlen(CHANNEL)){
			memcpy(pszST,CHANNEL,strlen(CHANNEL));
		}else{
			LOGE("ERROR1007");
			exit(-1);
		}
	}

	//2. UMENG_CHANNEL
	pszST = strstr(buffer, "meta-data android:name=\"UMENG_CHANNEL\"");
	if(pszST){
		pszST += strlen("meta-data android:name=\"UMENG_CHANNEL\""); // SKIP
		pszST = strstr(pszST,"android:value=\"");
		pszST += strlen("android:value=\"");
		if(*pszST=='C'){
			pszST++; // SKIP olo C-BEGINs channel style
		}
		pszED = strstr(pszST,"\"");
		if((unsigned int)(pszED-pszST)==strlen(CHANNEL)){
			memcpy(pszST,CHANNEL,strlen(CHANNEL));
		}else{
			LOGE("ERROR1006");
			exit(-1);
		}
	}

	//2-0. QP_CHANNEL_ID
	pszST = strstr(buffer, "meta-data android:name=\"QP_CHANNEL_ID\"");
	if(pszST){
		pszST += strlen("meta-data android:name=\"QP_CHANNEL_ID\""); // SKIP
		pszST = strstr(pszST,"android:value=\"");
		pszST += strlen("android:value=\"");
		if(*pszST=='C'){
			pszST++; // SKIP olo C-BEGINs channel style
		}
		pszED = strstr(pszST,"\"");
		if((unsigned int)(pszED-pszST)==strlen(CHANNEL)){
			memcpy(pszST,CHANNEL,strlen(CHANNEL));
		}else{
			LOGE("ERROR1008-0");
			exit(-1);
		}
	}

	//3. LETU CID
	pszST = strstr(buffer, "meta-data android:name=\"lltt_cpchannelid\"");
	if(pszST){
		pszST += strlen("meta-data android:name=\"lltt_cpchannelid\""); // SKIP
		pszST = strstr(pszST,"android:value=\"");
		pszST += strlen("android:value=\"");
		pszED = strstr(pszST,"\"");
		if((unsigned int)(pszED-pszST)==strlen(CID)){
			memcpy(pszST,CID,strlen(CID));
		}else{
			LOGE("ERROR1005");
			exit(-1);
		}		
	}

	//3. OLD CID
	pszST = strstr(buffer, "meta-data android:name=\"CHID\"");
	if(pszST){
		pszST += strlen("meta-data android:name=\"CHID\""); // SKIP
		pszST = strstr(pszST,"android:value=\"");
		pszST += strlen("android:value=\"");
		pszED = strstr(pszST,"\"");
		if((unsigned int)(pszED-pszST)==strlen(CID)){
			memcpy(pszST,CID,strlen(CID));
		}else{
			LOGE("ERROR1004");
			exit(-1);
		}
	}

	return 0;
}


static int hackAndroidManifestXml(char *AndroidManifestXml, char *CHANNEL, char *CID)
{
	LOGI("Info: AndroidManifestXml FILE path NAME = %s\r\n", AndroidManifestXml);
	LOGI("Covert CHANNEL to %s, CID to %s\r\n", CHANNEL, CID);
	
	char *buffer = NULL;
	int len = 0;
	if(0!=file2buff(AndroidManifestXml,&buffer,&len)){
		LOGI("-ERROR1\r\n");
		return -1;
	}
	
	if(0!=convertAndroidManifest(buffer,len,CHANNEL,CID)){
		LOGI("-ERROR2\r\n");
		return -1;
	}
	
	buff2file(buffer,len,AndroidManifestXml);
	free(buffer);
	
	LOGI("Hack AndroidManifestXml (CHANNEL=%s,CID=%s)SUCCESS\r\n", CHANNEL,CID);
	return 0;
}


#define Cmmn_isSpace(a)   ((a)==0x20 || ((a)==0x09))
#define SkipSpace(a) while((Cmmn_isSpace(*a)||0x0a ==*a || 0x0d == *a) && *a ){a++;}

static int    g_iCount = 0;
static char*  g_pszCHANNEL[2048]={0};
static char*  g_pszCID[2048]={0};

static void _appendLine(char *pszLine, int len)
{
	char *CHANNEL = NULL;
	char *CID = NULL;

    char *temp = NULL;
    char *strz = NULL;
    /* hack end char to '\0' */
    *(pszLine+len-1) = '\0';
    
    /* CHANNEL */
    strz = strstr(pszLine, "[");
	strz++;
	SkipSpace(strz);
    temp = strstr(strz, " ");
	if(!temp){
		LOGE("Failure(2005)\r\n");
		exit(-1);
	}
	CHANNEL = we_cmmn_strndup(strz,temp-strz);
    	
	strz = temp;
	SkipSpace(strz);
    /* CID */
    temp = strstr(strz, "]"); 
	temp = temp?temp:pszLine+len-1;
    CID = we_cmmn_strndup(strz,temp-strz);
	// space 2 \0
	for(int i=strlen(CID);i>=0;i--){
		if(Cmmn_isSpace(CID[i])){
			CID[i]='\0';
		}
	}
    
    /* recover end char to old */
    *(pszLine+len-1) = ']';
	
	// SAVE
	g_pszCHANNEL[g_iCount] = CHANNEL;
	g_pszCID[g_iCount] = CID;
	g_iCount ++;
}

static char *smali = "./temp/smali/org/cocos2dx/cpp/AppActivity.smali";
static char *AndroidManifestXml = "./temp/AndroidManifest.xml";
static char cmdLine[1024]={0};

static char* getOutputApkFileName(char *pszSourceApkFileName, char *CHANNEL, char *CID)
{
	static char pszOutputFileName[256]={0};

	char *pszST = strrchr(pszSourceApkFileName,'\\');
	if(!pszST){
		pszST = strrchr(pszSourceApkFileName, '/');		
	}
	
	if(!pszST){
		pszST = pszSourceApkFileName;
	}else{
		pszST++; // SKIP DIR CHARACTOR
	}	

	char *pszED = strstr(pszST,".apk");
	if(!pszED){
		pszED=strstr(pszST,".APK");
	}
	if(!pszED){
		return NULL;
	}
	char pszTEMP[128]={0};
	memcpy(pszTEMP,pszST,pszED-pszST);
	sprintf(pszOutputFileName,"output\\%s_%s_%s.apk",pszTEMP,CHANNEL,CID);

	return pszOutputFileName;
}

int main(int argc, char **argv)
{
	remove(LOGFILENAME);  // Remove LOG FILENAME
	if(argc<6){
		LOGE("BAD PARM NUM\r\n");
		return -1;
	}

	char *pszApkFileName = argv[1];
	char *pszKeyStore = argv[2];
	char *pszPassword = argv[3];
	char *pszAlias = argv[4];

	char *buffer = NULL;
	int len = 0;
	if(0!=file2buff(argv[5],&buffer,&len)){
		LOGE("CFG ERROR1\r\n");
		return -1;
	}
	
	LOGI("当前操作--->APK=%s,\r\n\tkeystore=%s\r\n\r\n",pszApkFileName,pszKeyStore);
	
	/* parse plst buff, and create frames */
    char *pST = strchr(buffer, '[');
    while(NULL != pST && *pST == '['){
        char *pED = strchr(pST, ']');
        if(!pED){
			LOGE("bad config buffer");
			exit(-1);
        }
        _appendLine(pST, pED+1-pST);
        pST = strchr(pED, '[');
    }

	free(buffer);

	LOGE("解包中.......");
	sprintf(cmdLine,"apktool.bat d -f  %s --output temp >nul",pszApkFileName);
	int iRET = system(cmdLine);
	if(0!=iRET){
		LOGE("Failure");
		exit(-1);
	}
	LOGE("OK!\r\n\r\n");

	mkdir("./output");
	for(int i=0;i<g_iCount;i++){
		char *CHANNEL = g_pszCHANNEL[i];
		char *CID = g_pszCID[i];

		LOGE("正在打包(%d)--->[CHANNEL=%s,CID=%s]\r\n", i,CHANNEL, CID);
		LOGE("0...%d%%", 20);		
		// 2. smali修改
		iRET = hackSmali(smali, CID);
		if(0!=iRET){
			LOGE("hackSmali Failure0");
			exit(-1);
			break;
		}
		LOGE("...%d%%", 30);	
		// 3. ManifestXml修改
		iRET = hackAndroidManifestXml(AndroidManifestXml, CHANNEL, CID);
		if(0!=iRET){
			LOGE("hackSmali Failure1");
			exit(-1);
			break;
		}
		LOGE("...%d%%", 60);
		// 4. 打包
		sprintf(cmdLine,"apktool b temp -o temp.apk >nul");
		iRET=system(cmdLine);
		if(0!=iRET){
			LOGE("Failure2");
			exit(-1);
			break;
		}
		LOGE("...%d%%", 80);
		
		// 5. 签名
		sprintf(cmdLine,"jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore %s -storepass %s temp.apk %s >nul",pszKeyStore,pszPassword,pszAlias);
		iRET = system(cmdLine);
		if(0!=iRET){
			LOGE("Failure3");
			exit(-1);
			break;
		}
		LOGE("...%d%%", 90);
		
		// 6. 清理原来的输出的名字,如果有的话
		char *pszOutputFileName = getOutputApkFileName(pszApkFileName,CHANNEL,CID);
		sprintf(cmdLine,"if exist %s del /q/f %s >nul", pszOutputFileName,pszOutputFileName);
		system(cmdLine);
		
		// 7. 对齐
		sprintf(cmdLine,"zipalign -v 4 temp.apk %s >nul", pszOutputFileName);
		iRET = system(cmdLine);
		if(0!=iRET){
			LOGE("Failure4");
			exit(-1);
			break;
		}
		LOGE("...%d%%", 99);

		// 8. 清理
		sprintf(cmdLine,"call del /q/f temp.apk >nul");
		system(cmdLine);
		LOGE("...%d%%....OK!\r\n\r\n", 100);
	}
	
	LOGE("清理临时文件.....");
	system("rd /s/q temp >nul");
	LOGE("OK!\r\n\r\n");

	LOGE("本次操作完成......^v^\r\n\r\n");

	return 0;
}
