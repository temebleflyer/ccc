#ifndef __LOGOUT_H__
#define __LOGOUT_H__
#include <iostream>
#include <list>
#include <string>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>
using namespace std;

struct LogInfo
{
	int level;
	char szDate[32];
	string logStr;
};

class LogOut
{
public:
	
	LogOut();
	~LogOut();
	static LogOut* instance();
	
	int run();
	int init(const char* path, const char* head);
	int WriteFile(int level, const char* szDate, const char* content);
	int CheckFile();
	int push(LogInfo &log);
	int pop(list<LogInfo> &logs);
	void setModName(const char * Name)		
	{			  
	    strcpy(m_ModName, Name);		
	}
	const char* getModName()
	{
		return m_ModName;
	}

private:
	static LogOut * m_pinst;
	FILE* m_fp;
	
	unsigned int m_seq;
	long m_pid;
	
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;
	
	char m_path[512]; //路径
	char m_filePath[512+120];	//文件绝对路径
	char m_date[32];	//日期
	char m_head[80];	//文件名前缀
	char m_ModName[64];	//模块名
	list<LogInfo> m_logs;	//日志内容
};


#endif 

