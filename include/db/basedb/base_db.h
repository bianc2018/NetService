#ifndef BASE_DB_H
#define BASE_DB_H

extern "C"
{
#include "../../sql/sqlite3.h"
};

#include <string>
#include <map>
#include "db_table.h"
#include "sql_format.h"
#include <mutex>

class BaseDb
{
public:
	BaseDb();
	//连接数据库 url 数据库地址 busy_time_out 每次操作，数据库忙时等待数据 
	int connect(const std::string &url, int busy_time_out = 1000);
	//关闭
	int close();
	//执行 更新 删除 创建
	int exec(const std::string &url);
	//执行 查询
	DBTable get(const std::string &url);
	//预定义 未实现
//	int prepare();
private:
	//清除错误信息
	int clear_err_msg();
private:
	std::mutex lock_;
	sqlite3 *db_ptr_;
	sqlite3_stmt*stmt_ptr_;
	char * err_msg_;
};

#endif //!BASE_DB_H