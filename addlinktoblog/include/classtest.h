#include <iostream>
#include "myThread.h"
#include "mysqldb.h"
class demo : public TThread
{
public:
	demo();
	~demo();
	int init(config*, mysqldb*);
	void threadRun();
	static demo* instance();
	void routine();
	int lock();
	int unlock();
	void writefile();
	void stringset(string &, vector<string> &);
	void sqlset(string &, string &);
	int getfilerecord(string &,vector<string>&);
	static demo* m_instance;
	pthread_mutex_t m_mutex;
public:
	config* m_pConf;
	mysqldb* m_pDb;
	
};
