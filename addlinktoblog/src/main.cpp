#include <iostream>
#include <string.h>
#include <string>
#include "classtest.h"
#include <unistd.h>
#include <vector>
#include "tinyxml.h"
#include "prdLog.h"


using namespace std;
/*
vector<demo*> vec;
vector<demo*>::iterator iter;

int init()
{

	for(int i = 0;i<10;i++){
		demo *classtest = new demo;
		vec.push_back(classtest);
	}
	for(iter = vec.begin();iter!=vec.end();iter++)
	{
		(*iter)->start();
	}
}
*/

int main(int argc, char *argv[])
{
	

	config* conf = new config();
	char path[256] = {0};
	sprintf(path, "%s", "/home/cjn/threadtest/cfg/test.xml");
	conf->Init(path);
	mysqldb* db = new mysqldb();
	//dbData data;
	//memset(&data, 0, sizeof(data));
	//data.dbname = conf->m_config.db.dbname; //"sql106_55_173_8";
	//data.host = conf->m_config.db.host; //"127.0.0.1";
	//data.passwd = conf->m_config.db.passwd; //"1411714511";
	//data.username = conf->m_config.db.username; //"sql106_55_173_8";
	//data.port = conf->m_config.db.port; //"3306";
	LogOut::instance()->setModName("emlogtest");
	LOGINIT(0, "emlogtest", "/home/cjn/threadtest/log");
	//LogOut::instance()->init("/home/cjn/threadtest/log", "emlogtest");
	//LOGINITS(&level, head, path);
	db->initDB(conf->m_config.db);
	demo *classtest = new demo;
	int ret = classtest->init(conf, db);
	classtest->start();
	while(1)
	{
		sleep(60);
	}
	//db->execSQL("select * from yaopin;");
	return 0;
}

