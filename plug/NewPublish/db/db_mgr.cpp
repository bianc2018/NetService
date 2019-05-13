#include "db_mgr.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "log/log.hpp"

int DbMgr::db_init(const std::string & db_uri, int busy_time_out, int synchronous)
{
	//table_name_ = table_name;
	if (SQLITE_OK != connect(db_uri, busy_time_out))
	{
		LOG(LERROR, "数据库打开失败,url=", db_uri);
		return -1;
	}
	set_synchronous(synchronous);

	//初始化
	//prepare("get_data", "");
	return 0;
}

Json DbMgr::get_all(const std::string & table_name)
{
	db::DbStmt stmt = new_stmt("SELECT * FROM "+table_name+";");
	Json json;
	stmt.bind_get_rows(json);
	return json;
}

Json DbMgr::get_data(const std::string & table_name, const std::string & key, const std::string & value)
{
	db::DbStmt stmt = new_stmt("SELECT * FROM "+table_name+" WHERE "+key+"=?;");
	Json json;
	stmt.bind_get_row(json, value);
	return json;
}

int DbMgr::get_datas(Json & datas, const std::string & table_name, const std::string & _where, const std::string & order, int beg, int len, size_t * p_all)
{
	std::string str = _where;
	if (_where == "")
		str = "1=1";

	db::DbStmt stmt =new_stmt("SELECT * FROM " + table_name + " WHERE " + str + "  ORDER BY " + order + " limit ?,?;");
	if (p_all != nullptr)
	{
		db::DbStmt stmt_count = new_stmt("SELECT * FROM " + table_name + " WHERE   " + str + ";");
		Json d;
		*p_all = stmt_count.bind_get_rows(d);
	}

	return stmt.bind_get_rows(datas, beg, len);
}

Json DbMgr::get_likes(const std::string & table_name, const std::string & key, const std::string & keyword, const std::string & orderby, const std::string& order, int offset=0, int max_len=-1)
{
	db::DbStmt stmt = new_stmt("SELECT * FROM "+table_name+" WHERE "+key+" like ? ORDER BY "+orderby+" "+order+" limit ?,?;");
	Json json;
	//"DESC" "ASC"
	stmt.bind_get_rows(json, "%" + keyword + "%",offset, max_len);
	return json;
}

int DbMgr::reset_status(const std::string & table_name,const std::string & key, const std::string & value, int status)
{
	//UPDATE table_name
	//SET column1 = value1, column2 = value2, ...
	//	WHERE some_column = some_value;
	db::DbStmt stmt = new_stmt("UPDATE "+table_name+" SET status=? WHERE "+key+"=?;");
	//"DESC" "ASC"
	return stmt.bind_exec(status, value);
	
}

int DbMgr::get_status(const std::string & table_name, const std::string & key, const std::string & value)
{
	db::DbStmt stmt = new_stmt("SELECT status FROM "+table_name+" WHERE "+key+"=?;");
	Json json;
	stmt.bind_get_row(json, value);
	return json.get_int_value("status");
}

int DbMgr::delete_data(const std::string & table_name, const std::string & key, const std::string & value)
{
	db::DbStmt stmt = new_stmt("DELETE FROM "+table_name+"  WHERE "+key+"=?;");
	return stmt.bind_exec(value);
}

int DbMgr::delete_data(const std::string & table_name, const std::string & key, int value)
{
	db::DbStmt stmt = new_stmt("DELETE FROM "+table_name+"  WHERE "+key+"=?;");
	return stmt.bind_exec(value);
}


std::string DbMgr::get_unique_code()
{
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); // 这里是两个() ，因为这里是调用的 () 的运算符重载
	return boost::uuids::to_string(a_uuid);
}

int DbMgr::close()
{
	
}

bool DbMgr::has_table(const std::string & table_name)
{
	db::DbStmt stmt = new_stmt("SELECT name FROM sqlite_master WHERE type='table' AND name=?;");
	Json json;
	if (0 == stmt.bind_get_row(json, table_name))
		return true;
	return false;
}
