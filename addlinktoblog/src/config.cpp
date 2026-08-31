#include "config.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tinyxml.h"

using namespace std;

config::config()
{
	memset(&m_config, 0, sizeof(m_config));
	memset(&m_configPath, 0, sizeof(m_configPath));
}
config::~config()
{
}

int config::Init(const char* path)
{
	strcpy(m_configPath, path);

	configData *objConfig = &m_config;
	int ret = LoadXmlCommonPart(objConfig);
	if(-1 == ret)return ret;

	return 0;
}

int config::LoadXmlCommonPart(configData * objConfig)
{
	TiXmlDocument config(m_configPath);
	if(!config.LoadFile())
	{
		cout<<"read config file error:"<<m_configPath<<endl;
		return -1;
	}
	TiXmlHandle pHandle(&config);

	TiXmlElement * element1 = NULL;
	TiXmlElement * element2 = NULL;
	TiXmlElement * element3 = NULL;

	element1 = pHandle.FirstChildElement("config").Element();
	if(element1 == NULL)
	{
		cout<<"read config error"<<endl;
		return -1;
	}
	
	element2 =  element1->FirstChildElement("dbclient");
	if(element2 == NULL)
	{
		cout<<"read dbclient error"<<endl;
		return -1;
	}
	strcpy(objConfig->db.dbname, element2->Attribute("name"));
	strcpy(objConfig->db.host, element2->Attribute("host"));
	strcpy(objConfig->db.passwd, element2->Attribute("passwd"));
	strcpy(objConfig->db.username, element2->Attribute("user"));
	strcpy(objConfig->db.port, element2->Attribute("port"));
	
	element2 =  element1->FirstChildElement("filename");
	if(element2 == NULL)
	{
		cout<<"read filename error"<<endl;
		return -1;
	}
	strcpy(objConfig->cfgPath, element2->Attribute("path"));
	strcpy(objConfig->cfgMode, element2->Attribute("mode"));
/*	
	element1 = pHandle.FirstChildElement("app").Element();
	if(element1 == NULL)
	{
		cout<<"read config error"<<endl;
		return -1;
	}
	
	element2 =  element1->FirstChildElement("data");
	while(element2 != NULL)
	{
		const char* name = element2->Attribute("name");
		const char* bind = element2->Attribute("bind");
		const char* sql = element2->Attribute("sql");
		if(!name || !sql)
		{
			//do not thing
		}
		else
		{
			//objConfig->sql.sqlBind.insert(std::pair<string, string>(name, bind));
			//objConfig->sql.sqlName.insert(std::pair<string, string>(name, sql));
		}
		element2 = element2->NextSiblingElement("data");
	}
	
	if(objConfig->sql.sqlBind.empty() || objConfig->sql.sqlName.empty())
	{
		cout << "load sql config empty" <<endl;
	}
*/	
	return 0;
}
/*
int config::routine()
{
	int ret = 0;
	struct stat oldStat;
	struct stat newStat;
	stat(m_configPath, &oldStat);
	stat(m_configPath, &newStat);
	while(1)
	{
		sleep(5);
		//配置文件更新检查
		ret = stat(m_configPath, &newStat);
		if(ret != 0)
		{
			cout<<"error: no finded config file["<<m_configPath<<"]"<<endl;
			continue;
		}

		if(newStat.st_mtime > oldStat.st_mtime)
		{
			oldStat.st_mtime = newStat.st_mtime;
			ret = Init(m_configPath);
			if(ret != 0)
			{
				cout<<"error: config file load error["<<m_configPath<<"]"<<endl;
			}
			else
			{			
				config * pConfig = NULL;
				//加载成功，切换标识, m_activeConfig
				if(ACTIVE_CONFIG_MASTER == m_activeConfig)
				{
					m_activeConfig = ACTIVE_CONFIG_SLAVE;
					pConfig = &m_config_slave;
				}
				else
				{
					m_activeConfig = ACTIVE_CONFIG_MASTER;
					pConfig = &m_config_master;
				}
				DCLOG_SETLEVEL(DCLOG_CLASS_SYS,  pConfig->logLevel);
				DCLOG_SETLEVEL(DCLOG_CLASS_BIZ,  pConfig->logLevel);
				DCLOG_SETLEVEL(DCLOG_CLASS_PERF,  pConfig->logPerfFlag);
			}
		}
	}
}
*/
