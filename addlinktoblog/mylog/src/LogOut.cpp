#include "LogOut.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <errno.h>
#include <sys/time.h>

LogOut* LogOut::m_pinst = NULL;

static void* process(void * arge)
{
	LogOut* log = (LogOut*)arge;
	log->run();
	return NULL;
}

LogOut::LogOut()
{
	memset(m_path, 0x0, sizeof(m_path));
    memset(m_head, 0x0, sizeof(m_head));
	memset(m_filePath, 0x0, sizeof(m_filePath));
	memset(m_date, 0x0, sizeof(m_date));
	m_seq = 0;
	m_pid = getpid();
	m_fp = NULL;

    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

LogOut::~LogOut()
{
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
}

LogOut* LogOut::instance()
{
    if(!m_pinst){
		m_pinst = new LogOut();
	}
	return m_pinst;
}

int LogOut::init(const char* path, const char* head)
{
	int ret = 0;
	ret = access(path, W_OK);
	if(ret < 0){
		cout << "path error" <<endl;
		return -1;
	}
	if(strlen(path) >= sizeof(m_path)){
		cout << "path too long" <<endl;
		return -1;
	}
	strncpy(m_path, path, strlen(path));
	strncpy(m_head, head, strlen(head));
	pthread_t p;
    ret = pthread_create(&p, NULL, process, (void*)this);
    if(ret < 0){
		cout << "create logout pthread failed" <<endl;
		return -1;
	}
	return 0;
}

int LogOut::run()
{
	std::list<LogInfo> logs;
	std::list<LogInfo>::iterator iter;
	while(1){
		if(pop(logs) == -1){
			CheckFile();
		}else{
			for(iter = logs.begin(); iter != logs.end(); iter++){
				WriteFile(iter->level, iter->szDate, iter->logStr.c_str());
			}
		}
	}
	return 0;
}

int LogOut::CheckFile()
{
	if(!m_fp){
		return 0;
	}
	fflush(m_fp);
	return 0;
}

// 写文件
int LogOut::WriteFile(int level, const char* szDate, const char* content)
{
	unsigned long fsize = 0;
	if(m_fp){		
		if(strncmp(m_date, szDate, 8)){		//按天建立文件
			fclose(m_fp);
            m_fp = NULL;
            m_seq = 0;
		}else if( access(m_filePath, F_OK) < 0){	//文件消失了，需要新建
			fclose(m_fp);
            m_fp = NULL;
		}else{		//单文件过大，建立新文件
			fsize = ftell(m_fp);
			if(fsize >= 1024*1024*20)
			{
				fclose(m_fp);
				m_fp = NULL;
			}
		}
	}
	if(!m_fp){
		strncpy(m_date, szDate, sizeof(m_date));
		m_date[32] = 0;
		m_seq++;
		sprintf(m_filePath, "%s/%s_%d_%s_%s_%06u", m_path, m_head, m_pid, "all",m_date,m_seq); //组装文件名
		m_fp = fopen(m_filePath, "a+");	//建立新文件
		if(!m_fp)
		{
			cout << "open file fail" <<endl;
			return -1;
		}
	}
	int length = strlen(content);
	fwrite(content, length, 1, m_fp);
	return 0;
}

//传出内容
int LogOut::pop(list<LogInfo> &logs)
{
	logs.clear();
	pthread_mutex_lock(&m_mutex);
	if(m_logs.empty()){
		struct timespec tm;
		tm.tv_sec = time(NULL) + 5;
		tm.tv_nsec = 0;
		int ret = pthread_cond_timedwait(&m_cond, &m_mutex, &tm);	//等5秒
		if(m_logs.empty() || ret < 0){
			pthread_mutex_unlock(&m_mutex);
			return -1;
		}
	}
	m_logs.swap(logs); //传出所有内容
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

//插入内容
int LogOut::push(LogInfo &log)
{
	pthread_mutex_lock(&m_mutex);
	m_logs.push_back(log);
	pthread_mutex_unlock(&m_mutex);
	pthread_cond_signal(&m_cond);
	return 0;
}



