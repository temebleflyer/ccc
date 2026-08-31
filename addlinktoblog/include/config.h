#include <map>
#include <string>
//#include "myThread.h"
#include <vector>
#include <string.h>

struct dbData 
{
	char host[32];
	char username[64];
	char passwd[32];
	char dbname[64];
	char port[16];
};
/*
struct sqlData
{
	map<string, string> sqlName;
	map<string, string> sqlBind;
};
*/
struct sqlconfig
{
	char name[512];
	char bind[256];
	char sql[4096];
};


struct configData
{
	dbData db;
	sqlconfig sqCon;
	//sqlData sql;
	char cfgPath[256];
	char cfgMode[16];
};

class config //: public myThread
{
public:
	config();
	~config();
	int Init(const char* path);
	static config* instance();
	config* GetConfig();
	int LoadXmlCommonPart(configData * objConfig);
	configData m_config;
	//int routine();
private:
	static config*	m_instance;
	
	char m_configPath[256];
	
};

