#include "classtest.h"
#include <iostream>
#include <fstream>
//#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "prdLog.h"
using namespace std;
demo* demo::m_instance = NULL;
demo::demo()
{
	pthread_mutex_init(&m_mutex, NULL);
	config* conf = NULL;
	mysqldb* db = NULL;
}

demo::~demo()
{
	pthread_mutex_destroy(&m_mutex);
}

int demo::lock()
{
	return pthread_mutex_lock(&m_mutex);
}

int demo::unlock()
{
	return pthread_mutex_unlock(&m_mutex);
}

demo* demo::instance()
{
	if (NULL == m_instance)
	{
		m_instance = new demo;
	}

	return m_instance;
}

int demo::init(config *conf, mysqldb *db)
{
	m_pConf = conf;
	m_pDb = db;
	return 0;
}

void demo::writefile()
{
	string log;
	int ret = lock();
	if(ret)
	{
		cout << "file lock" << endl;
		unlock();
		return;
	}
	FILE *fp=NULL;
	if((fp=fopen("/home/lighthouse/classtest/out.txt","w+"))==NULL)
	{
		cout << "error opening destination file." << endl;
		return ;

	}
	log = "classtest:" ;
	log += to_string(self());
	log += "\n";
	fwrite(log.c_str(), sizeof(log.c_str()), 3, fp);
	cout << log.c_str()<<endl;
	//destFile << log << getpid();
	//cout << "classtest threadRun pid=" << self() << endl;
	fflush(fp);
	fclose(fp);
	unlock();
	sleep(1);
	
}

void demo::sqlset(string &temp, string &sql)
{
//update emlog_blog set content=temp where gid=2;
	string sqlPre = "update emlog_blog set content=";
	string sqlSuff = "where gid=2";
	sql += sqlPre;
	sql += temp;
	sql += sqlSuff;
	LOGOUT(0, "sqlfix:[%s]", sql.c_str());
}

void demo::stringset(string &temp, vector<string> &vec)
{
	string httpPre = "http://192.168.1.100/video/";
	string httpSuff = "\\\"http://192.168.1.100/video/\\\")";
	vector<string>::iterator iter;
	for(iter = vec.begin();iter != vec.end(); iter++)
	{
		temp += "\"\\n[";
		temp += *iter;
		temp += "](";
		temp += httpPre;
		temp += *iter;
		temp += " ";
		temp += httpSuff;
		temp += "\"\n";
	}
	LOGOUT(0, "stringset:[%s]", temp.c_str());
}

int demo::getfilerecord(string &str, vector<string> &vec)
{
	LOGOUT(0, "last record:[%s]", str.c_str());
	char buff[1024] = {0};
	ifstream infile;
	infile.open("./temp",ios::in);
	string tempStr = "";
	while(!infile.eof())            // 若未到文件结束一直循环 
	{
		infile.getline(buff, sizeof(buff),'\n');//读取一行，以换行符结束，存入 a[] 中
		vec.push_back(buff);
	}
	
	for(auto p:vec)
	{
		tempStr += p;
	}
	LOGOUT(0, "now record:[%s]", tempStr.c_str());
	if(!strncmp(tempStr.c_str(), str.c_str(), tempStr.length())) //比较上次查询结果，相同则返回-1
	{
		return -1;
	}
	str = tempStr;//保存上次查询结果
	return 0;
}


//select content from emlog_blog where gid=2;
void demo::routine()
{	

	string str = "";
	
	while(1)
	{
		LOGOUT(0, "begin to deal data");
		sleep(30);
		system("ln -s /home/cjn/share_emlog/* /www/wwwroot/emlog/video/");
	
		vector<string> vec;
		vector<string> result;
		vector<string>::iterator iter;
		
		system("rm -f temp");
		
		string cmd = "ls /www/wwwroot/emlog/video/ >> temp";
		system(cmd.c_str());
		if(getfilerecord(str, vec) == -1) //和上次结果相同，文件没有变化，不需要进行下一步
		{
			LOGOUT(0, "this record is same of last record continue");
			continue;
		}
		else
		{
			system("rm -rf /www/wwwroot/emlog/video/*");
		}
		
		string temp;
	/*	
		string temp;
		m_pDb->execSQL("]select content from emlog_blog where gid=2;", result);
		for(iter = result.begin();iter!=result.end();iter++)
		{
			if(iter->length() > 0)
			{
				temp = *iter;
				break;
			}
		}
	*/	
		stringset(temp, vec);
		string sql;
		sqlset(temp, sql);
		//sql set
		m_pDb->execSQL(sql, result);
		
	}
}

