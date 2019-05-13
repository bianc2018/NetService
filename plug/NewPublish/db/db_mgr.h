#ifndef DB_DBMGR_H
#define DB_DBMGR_H

#include <map>

#include "db_base.h"

class DbMgr :public db::DbBase
{
public:
		
	int db_init(const std::string &db_uri,  int busy_time_out = 1000, int synchronous=1/*0 full¡¢ 1 normal¡¢2 off Ð´Í¬²½*/);
	
	Json get_all(const std::string &table_name);

	Json get_data(const std::string &table_name, const std::string & key, const std::string & value);

	//int get_datas(Json&news, const std::string&_where, const std::string&order="",int beg=0,int len=-1);
	int get_datas(Json&datas, const std::string & table_name, const std::string&_where, const std::string&order = "", int beg = 0, int len = -1,size_t *p_all=nullptr);

	Json get_likes(const std::string &table_name, const std::string&key, const std::string&keyword, const std::string &orderby, const std::string& order,int offset,int max_len);

	//
	int reset_status(const std::string &table_name, const std::string & key, const std::string & value, int status);

	int get_status(const std::string &table_name, const std::string & key, const std::string & value);

	int delete_data(const std::string &table_name, const std::string & key, const std::string & value);
	int delete_data(const std::string &table_name, const std::string & key, int value);
	std::string get_unique_code();

	int close();

	bool has_table(const std::string &table_name);
private:
};
#endif