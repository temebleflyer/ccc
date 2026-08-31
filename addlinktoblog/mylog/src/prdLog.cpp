#include <iostream>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdarg.h>
#include "LogOut.h"
#include "prdLog.h"
#include <unistd.h>

using namespace std;
#define LOGBUFSIZE 40*1024

static int * logLevel = NULL;
static LogOut* pLogOut = 0;
//static PrdLog* prdlog = 0;

void getDate(char* ret, int size)
{
	if(ret == NULL || size < 1){
		cout << "error ret buf to get date" <<endl;
		return ;
	}
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	struct tm ctime = {0};
	localtime_r(&(tv.tv_sec),&ctime);
	sprintf(ret, "%04d%02d%02d%02d_%02d_%02d_%d", 
		1900+ctime.tm_year, 
		ctime.tm_mon+1,
		ctime.tm_mday,
		ctime.tm_hour, 
		ctime.tm_min, 
		ctime.tm_sec,
		tv.tv_usec
	);
}

void PrdLog::Log(int level, int ErrorCode, const char* fun, const char* file, int line, const char * format, ...)
{
	if(level <0 || level > LOG_LEVEL_FATAL){
		cout << "error log level set" <<endl;
		return;
	}
	char *buf = new char[LOGBUFSIZE];
	memset(buf, 0, sizeof(buf));
	va_list argptr;
	va_start(argptr, format);
    
    vsnprintf(buf,LOGBUFSIZE - 1, format, argptr);
    va_end (argptr); 
    strcat(buf+strlen(buf),"\n");
	WriteLog(ErrorCode, buf, level, fun, file, line);
	//delete []buf;
}

void PrdLog::CheckSetLevel(const int level, char* ret)
{	
	switch(level){
		case LOG_LEVEL_FATAL: strcpy(ret, "FATAL"); break;
		case LOG_LEVEL_ERROR: strcpy(ret, "ERROR"); break;		
		case LOG_LEVEL_WARN: strcpy(ret, "WARN"); break;
		case LOG_LEVEL_INFO: strcpy(ret, "INFO"); break;
		case LOG_LEVEL_TRACE: strcpy(ret, "TRACE"); break;
		case LOG_LEVEL_DEBUG: strcpy(ret, "DEBUG"); break;
	}
}

void PrdLog::WriteLog(int ErrorCode, char *szBuf,int iLevel, const char* fun, const char* file, int line)
{
	char nowDate[64] = {0};
	getDate(nowDate, sizeof(nowDate));
	unsigned long lnThreadId = pthread_self();
	long pid = getpid();
	
	char buf[LOGBUFSIZE];
	memset(buf, 0, LOGBUFSIZE);
	char level[8] = {0};
	CheckSetLevel(iLevel, level);

	char funName[256] = {0};
	const char * pTemp = strchr(fun,'(');
	if(pTemp != NULL){
	    strncpy(funName,fun,pTemp - fun);
	}else{
	    strcpy(funName,fun);
	}
	char fileName[256] = {0};
	const char * fixflag= strrchr(file,'/');
	if(fixflag !=NULL){
		sprintf(fileName,"%s:%d",fixflag+1,line);
	}else{
	   sprintf(fileName,"%s:%d",file,line);
	}
	const char *modName = pLogOut->getModName();
	
	cout << szBuf <<endl;
	
	snprintf(buf, LOGBUFSIZE, "%s|%s|%s|%d|%s%ld|%s%ld|%s|%s|%s", nowDate, \
	modName, \
	level, \
	ErrorCode, \
	"pid:", pid, \
	"tid:", lnThreadId, \
	fileName, \
	funName, \
	szBuf
	);
	if(pLogOut){
		LogInfo info;
		info.level = iLevel;
		strncpy(info.szDate, nowDate, sizeof(info.szDate) - 1);
		info.logStr = buf;
		pLogOut->push(info);
	}
	//delete []buf;
}

static int defaultLogLevel()
{
	char level[16] = {0};
	if(logLevel != NULL){
		return *logLevel;
	}else{
		return LOG_LEVEL_DEBUG; 
	}
	return 0;
}
int PrdLog::init(int *level, const char* head, const char* path)
{
	if(level != NULL){
		logLevel = level;
	}
	
	int ret = LogOut::instance()->init(path, head);
	if(ret < 0){
		pLogOut = NULL;
		return -1;
	}
	
	pLogOut = LogOut::instance();
	return 0;
}

