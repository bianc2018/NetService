#include "base_db.h"
#include "../../log/log.hpp"

BaseDb::BaseDb():db_ptr_(nullptr), stmt_ptr_(nullptr), err_msg_(nullptr)
{
	
}
//SQLITE_OK
int BaseDb::connect(const std::string & url,int busy_time_out)//ms
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

int BaseDb::close()
{
	int ret = 0;
	if (nullptr == db_ptr_)
	{
		sqlite3_close(db_ptr_);
		if (ret != SQLITE_OK)
			LOG(LERROR, "数据库关闭失败 ret", ret);
		db_ptr_ = nullptr;
	}
	clear_err_msg();
	return ret;
}

int BaseDb::exec(const std::string & sql)
{
	int ret = 0;

	lock_.lock();
	clear_err_msg();
	ret = sqlite3_exec(db_ptr_, sql.c_str(), nullptr, nullptr, &err_msg_);
	lock_.unlock();



	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "sql:", sql, "code:", ret, "msg:", err_msg_);
	}
	return ret;
}

int BaseDb::clear_err_msg()
{
	if (err_msg_ != nullptr)
	{
		sqlite3_free(err_msg_);
		err_msg_ = nullptr;
	}
	return 0;
}

DBTable BaseDb::get(const std::string & sql)
{
	int key = 0, item = 0;
	char **data = nullptr;

	int ret = 0;

	lock_.lock();
	clear_err_msg();
	ret = sqlite3_get_table(db_ptr_, sql.c_str(), &data, &item, &key, &err_msg_);
	lock_.unlock();
	
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "sql:", sql, "code:", ret, "msg:", err_msg_);
		return DBTable();
	}
	return DBTable(item, key, data);
}

