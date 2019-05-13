#include "db_base.h"

#include "log/log.hpp"

db::DbStmt::DbStmt():ptr_(nullptr)
{

}

db::DbStmt::DbStmt(const DbStmt & s)
{
	ptr_ = s.ptr_;
	db_ptr_ = s.db_ptr_;
}

db::DbStmt::DbStmt(sqlite3 * db_ptr, const std::string & sql):ptr_(nullptr), db_ptr_(nullptr)
{
	const char *ptail;
	auto ret = sqlite3_prepare_v2(db_ptr, sql.c_str(), sql.size(), &ptr_, &ptail);
	
	if(ptr_==nullptr || ret!=SQLITE_OK)
		LOG(LFATAL, "prepare ERROR tail = ", ptail, ",ret=", ret,",msg=", sqlite3_errmsg(db_ptr), ",sql=", sql);
	else
		LOG(LDEBUG, "prepare ok tail = ", ptail,",ret=", ret,",sql=",sql);
}

db::DbStmt::~DbStmt()
{
	
}

// 0 取到数据 1完结 <0 出错
int db::DbStmt::get_row(Json & json)
{
	auto ok = sqlite3_step(ptr_);
	if (ok == SQLITE_DONE)
	{
		LOG(LDEBUG, "sql done");
		return 1;
	}
	else if (ok == SQLITE_ROW) //获取到一行的结果
	{
		int cNum = 0;//结果集中有多少列
		cNum = sqlite3_column_count(ptr_);
		//获取数据
		for (int i = 0; i < cNum; i++)
		{
			//返回结果集中第i列的名字
			const char *p = sqlite3_column_name(ptr_, i);
			//获取结果集中第i列的数据类型
			int iType = sqlite3_column_type(ptr_, i);
			if (iType == SQLITE_INTEGER)
			{
				json.add_object_value(p, sqlite3_column_int(ptr_, i));
				//row.push_back(DbKey(p, sqlite3_column_int(ptr, i)));
			}
			else if (iType == SQLITE_FLOAT)
			{
				json.add_object_value(p, sqlite3_column_double(ptr_, i));
				//row.push_back(DbKey(p, sqlite3_column_double(ptr, i)));
			}
			else if (iType == SQLITE_TEXT)
			{
				json.add_object_value(p, (const char*)sqlite3_column_text(ptr_, i));
				//row.push_back(DbKey(p, sqlite3_column_text(ptr, i)));
			}
		}
		return 0;
	}
	else
	{
		LOG(LERROR, "get data err ret=", ok, ",msg=", sqlite3_errmsg(db_ptr_));
		return -2;
	}
	
}

int db::DbStmt::get_rows(Json & json)
{
	int size = 0;
	int ret = 0;
	json.init_array();
	while (true)
	{
		Json object;
		ret = get_row(object);
		if (ret != 0)
			break;
		size += 1;
		json.add_array_value(object);
	}
	if (1 == ret)
		return size;
	return ret;
}

int db::DbStmt::exec()
{
	if (ptr_)
	{
		sqlite3_reset(ptr_);
		auto ok = sqlite3_step(ptr_);
		if (ok != SQLITE_OK && ok != SQLITE_DONE)
		{
			LOG(LERROR, "get data err ret=", ok, ",msg=", sqlite3_errmsg(db_ptr_));
			return -2;
		}
		return 0;
	}
	LOG(LERROR, "stmt is nullptr");
	return -1;
}

void db::DbStmt::free()
{
	if (nullptr != ptr_)
	{
		sqlite3_finalize(ptr_);
		ptr_ = nullptr;
	}
}

bool db::DbStmt::is_ok()
{
	if (ptr_)
		return true;
	return false;
}

void db::DbStmt::sql_bind(int pos, const std::string & value)
{
	LOG(LDEBUG, "bind text", pos, value);
	sqlite3_bind_text(ptr_, pos, value.c_str(), -1, SQLITE_TRANSIENT);
}

void db::DbStmt::sql_bind(int pos, const int & value)
{
	LOG(LDEBUG, "bind int", pos, value);
	sqlite3_bind_int(ptr_, pos, value);
}

void db::DbStmt::sql_bind(int pos, const sqlite3_int64 & value)
{
	LOG(LDEBUG, "bind int64", pos, value);
	sqlite3_bind_int64(ptr_, pos, value);
}

void db::DbStmt::sql_bind(int pos, const size_t & value)
{
	LOG(LDEBUG, "bind size_t", pos, value);
	sqlite3_bind_int64(ptr_, pos, value);
}

void db::DbStmt::sql_bind(int pos, const double & value)
{
	LOG(LDEBUG, "bind double", pos, value);
	sqlite3_bind_double(ptr_, pos, value);

}

int db::DbBase::connect(const std::string & url, int busy_time_out)
{
	int ret = sqlite3_open(url.c_str(), &db_ptr_);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "数据库打开失败 路径", url, "返回码", ret);
		return ret;
	}
	sqlite3_busy_timeout(db_ptr_, busy_time_out);
	return ret;
}

int db::DbBase::set_synchronous(int i)
{
	if (db_ptr_)
	{
		auto ret = SQLITE_OK;
		if (0 == i)
		{
			ret = sqlite3_exec(db_ptr_, "PRAGMA synchronous = FULL; ", 0, 0, 0);
		}
		else if(1==i)
		{
			ret = sqlite3_exec(db_ptr_, "PRAGMA synchronous = NORMAL; ", 0, 0, 0);
		}
		else if (2 == i)
		{
			ret = sqlite3_exec(db_ptr_, "PRAGMA synchronous = OFF; ", 0, 0, 0);
		}
		else
		{
			LOG(LERROR, "无选项 i=", i, "，仅支持 0 full、 1 normal、2 off 写同步");
			return - 1;
		}

		if (ret != SQLITE_OK)
		{
			LOG(LERROR, "执行失败");
			return -3;
		}
		return 0;
	}
	return -2;
}

db::DbStmt db::DbBase::new_stmt(const std::string & str)
{
	auto  p = stmt_map_.find(str);
	if (stmt_map_.end() == p)
	{
		auto n = DbStmt(db_ptr_, str);
		stmt_map_.insert(std::make_pair(str, n));
		return n;
	}
	else
	{
		return p->second;
	}
}

int db::DbBase::exec(const std::string & sql)
{
	auto ret = sqlite3_exec(db_ptr_, sql.c_str(), nullptr, nullptr, nullptr);
	
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "sql:", sql, "code:", ret,",msg=", sqlite3_errmsg(db_ptr_));
	}
	return ret;
}

int db::DbBase::close()
{
	if (nullptr == db_ptr_)
		return -1;
	
	for (auto s : stmt_map_)
	{
		s.second.free();
	}

	auto ret =  sqlite3_close(db_ptr_);
	db_ptr_ = nullptr;

	return ret;
}

db::DbBase::~DbBase()
{
	close();
}
