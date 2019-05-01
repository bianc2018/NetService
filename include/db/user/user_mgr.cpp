#include "user_mgr.h"
#include "../../log/log.hpp"

int UserMgr::init(const std::string & db_path)
{
	int ret = db_.connect(db_path);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "打开数据失败 url= ", db_path);
		return ret;
	}
	//查询表项
	DBTable new_tb = db_.get("select name from sqlite_master where type='table' and name='" + USER_TABLE_NAME + "';");
	if (new_tb.get_rows_num() <= 0)
	{
		ret = init_db_table();
	}
	if (check_user("admin")!=0)
	{
		//创建用户
		UserInfo user;
		user.username = "admin";
		user.password = "admin";
		user.permission = 0;
		create(user);
#if 1
		UserInfo d;
		d.username = "admin";
		int ds = get_user_info(d,d.username);
		LOG(LDEBUG, ds, " get_user_info");
		ds = check_password(d.password, "admin");
		LOG(LDEBUG, ds, " check_password");
		get_user_permission(d.username, ds);
		LOG(LDEBUG, ds, " get_user_permission");
#endif
	}
	clear();
	return ret;
}

int UserMgr::create(UserInfo & user)
{
	//创建用户
	std::string sql = SQL_INSERT_ARGS(USER_TABLE_NAME,"('username','password','permission','status')",user.username,user.password,user.permission,1);
	
	int ret= db_.exec(sql);
	LOG(LDEBUG, ret, " user \n", sql);
	return ret;
}

int UserMgr::remove_and_active(const std::string & username)
{
	UserInfo user;
	get_user_info(user, username);
	std::string sql;
	//已取消的激活
	if (0 == user.status)
	{
		user.status = 1;
		sql = SQL_UPDATA(USER_TABLE_NAME, CONDITION_EQ(user.status), CONDITION_EQ(username));
	}
	//已激活的取消
	else
		sql= SQL_REMOVE(USER_TABLE_NAME, CONDITION_EQ(username));
	
	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " user \n", sql);
	return ret;
}
//删除用户
int UserMgr::delete_user(const std::string & username)
{
	std::string sql = SQL_DELETE(USER_TABLE_NAME,CONDITION_EQ(username));

	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " user \n", sql);
	return ret;
}
//重置密码
int UserMgr::edit_password(const std::string & username, const std::string & new_password)
{
	std::string password = new_password;
	std::string sql = SQL_UPDATA(USER_TABLE_NAME, CONDITION_EQ(password), CONDITION_EQ(username));

	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " user \n", sql);
	return ret;
}


int UserMgr::get_user_info(UserInfo & user, const std::string & username)
{
	std::string sql = SQL_SELECT(USER_TABLE_NAME, CONDITION_EQ(username));
	LOG(LDEBUG, "user \n", sql);
	DBTable tb = db_.get(sql);
	if (0 == tb.get_rows_num())
		return -1;

	GET_CELL_0(tb, user.username);
	//GET_CELL_0(tb, user.password);
	GET_CELL_INT0(tb, user.permission);
	GET_CELL_INT0(tb, user.status);
	return 0;
}

int UserMgr::get_users(vec_user_info & users, const std::string & _where, const std::string & order, int beg, int end)
{
	std::string sql;
	std::string other = "1=1 ";
	if (_where != "")
		other += " AND " + _where;

	if ("" != order)
		other += " Order By " + order;

	//if (limit > 0)
	//	other += " LIMIT " + std::to_string(limit);

	sql = SQL_SELECT(USER_TABLE_NAME, other);
	DBTable table = db_.get(sql);
	int row_num = table.get_rows_num();
	LOG(LDEBUG, row_num, " new  \n", sql);
	if (0 > row_num)
		return -1;
	else
	{
		if (end <= 0 || end>row_num)
			end = row_num;
		if (beg < 0||beg>end)
			beg = 0;

		for (int i = beg; i < end; i++)
		{
			UserInfo info;
			get_row(table, info, i);
			users.push_back(info);
		}
		return row_num;
	}
}

int UserMgr::get_user_permission(const std::string & username,int &permission)
{
	UserInfo user;
	permission = 3;
	//用户名为空 ,则是临时用户
	if (username == "")
		return permission;

	int ret = get_user_info(user,username);
	//获取用户信息成功
	if (0 == ret)
		permission = user.permission;

	return permission;
}
//重置用户权限
int UserMgr::reset_user_permission(const std::string & username, int & permission)
{
	std::string sql = SQL_UPDATA(USER_TABLE_NAME, CONDITION_EQ(permission), CONDITION_EQ(username));
	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " user \n", sql);
	return ret;
}

int UserMgr::check_password(const std::string & username, const std::string & password)
{
	std::string sql = SQL_SELECT_STATUS(USER_TABLE_NAME, SQL_WHERE_AND(CONDITION_EQ(username), CONDITION_EQ(password)));
	LOG(LDEBUG, "user \n", sql);
	DBTable tb = db_.get(sql);
	if (tb.get_rows_num() <= 0)
		return -1;
	return 0;
}

int UserMgr::check_user(const std::string & username)
{
	std::string sql = SQL_SELECT_STATUS(USER_TABLE_NAME,CONDITION_EQ(username));
	LOG(LDEBUG, "user \n", sql);
	DBTable tb = db_.get(sql);
	if (tb.get_rows_num() <= 0)
		return -1;
	return 0;
}

int UserMgr::clear()
{
	int permission = 3;
	std::string sql = SQL_DELETE(USER_TABLE_NAME, CONDITION_EQ(permission));

	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " user \n", sql);
	return ret;
}

UserMgr::~UserMgr()
{
	db_.close();
}

int UserMgr::init_db_table()
{
	//创建数据表
	const std::string sql = "CREATE TABLE user (\
									username   TEXT PRIMARY KEY\
									UNIQUE\
									NOT NULL,\
									password   TEXT,\
									permission TEXT DEFAULT(3)\
									NOT NULL,\
									status TEXT DEFAULT(1)\
									NOT NULL\
									);\
									";
	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, "user \n", sql);
	return ret;
}

int UserMgr::get_row(DBTable & table, UserInfo & it, int row)
{
	try
	{
		GET_CELL(table, it.username, row);
		//GET_CELL(table, it.password, row);
		GET_CELL_INT(table, it.status, row);
		GET_CELL_INT(table, it.permission, row);
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}
