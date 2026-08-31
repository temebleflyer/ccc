#include<iostream>
#include<cstdlib>
#include<stdlib.h>
#include "prdLog.h"

#include "mysqldb.h"
using namespace std;

mysqldb::mysqldb()
{
	connection = mysql_init(NULL); 
	if(connection == NULL)
	{
		cout << "Error:" << mysql_error(connection);
		LOGOUT(0, "Error:[%s]", mysql_error(connection));
	}
	result = NULL;
	row = NULL;
}

mysqldb::~mysqldb()
{
	if(connection != NULL)  // 关闭数据库连接
	{
		mysql_close(connection);
	}
}

int mysqldb::initDB(dbData &db)
{
	// 函数mysql_real_connect建立一个数据库连接
	// 成功返回MYSQL*连接句柄，失败返回NULL
	
	connection = mysql_real_connect(connection, db.host,
			db.username, db.passwd, db.dbname, atoi(db.port), NULL, 0);
	if(connection == NULL)
	{
		cout << "Error:" << mysql_error(connection);
		LOGOUT(0, "Error:[%s]", mysql_error(connection));
	}
	return 0;
}

int mysqldb::execSQL(string sql, vector<string>&vec)
{
	// mysql_query()执行成功返回0，失败返回非0值。
	if(mysql_query(connection, sql.c_str()))
	{
		cout << "Query Error:" << mysql_error(connection);
		LOGOUT(0, "Query Error:[%s]", mysql_error(connection));
	}
	else
	{
		result = mysql_use_result(connection); // 获取结果集
		// mysql_field_count()返回connection查询的列数
		for(int i=0; i < mysql_field_count(connection); ++i)
		{
			// 获取下一行
			row = mysql_fetch_row(result);
			if(row <= 0)
			{
				break;
			}
			// mysql_num_fields()返回结果集中的字段数
			for(int j=0; j < mysql_num_fields(result); ++j)
			{
				//cout << row[j] << " ";
				vec.push_back(row[j]);

			}
			cout << endl;
		}

		// 释放结果集的内存
		mysql_free_result(result);
	}
	return 0;
}

