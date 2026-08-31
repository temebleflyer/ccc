#ifndef _PRDLOG_H_
#define _PRDLOG_H_

#include <string>
#include <stdarg.h>
#include "LogOut.h"

#define LOG_LEVEL_FATAL 5
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_TRACE 1
#define LOG_LEVEL_DEBUG 0

#define LOGOUT(X,Y,...) PrdLog::Log(X, 0, __FUNCTION__, __FILE__, __LINE__, Y, ##__VA_ARGS__)
#define LOGINIT(L,H,P) PrdLog::init(L,H,P)

class PrdLog
{
public:
	static int init(int *level, const char* head, const char* path);
	static void Log(int level, int ErrorCode, const char* fun, const char* file, int line, const char *format, ... );
	static void WriteLog(int ErrorCode, char *szBuf,int iLevel, const char* fun, const char* file, int line);
	static void CheckSetLevel(const int level, char* ret);
public:
private:
	PrdLog()	  
	{
		
	}
private:
	//static PrdLog * prdlog;
	
};
#endif
