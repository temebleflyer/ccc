#include<iostream>
#include<string>
#include<mysql.h>
#include "config.h"
#include <vector>
using namespace std;

class mysqldb
{
public:
	mysqldb();
	~mysqldb();
	int initDB(dbData& db);
	int execSQL(string sql, vector<string>&);
	int initSQL(string path);
	mysqldb *m_inst;
private:
	MYSQL *connection;
	MYSQL_RES *result;
	MYSQL_ROW row;
		
};

