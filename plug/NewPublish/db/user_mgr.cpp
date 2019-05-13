#include "user_mgr.h"
#include "log/log.hpp"

int UserMgr::init(const std::string & db_path)
{
	int ret = mgr_.db_init(db_path);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "打开数据失败 url= ", db_path);
		return ret;
	}
	mgr_.set_synchronous(1);
	//查询表项
	//DBTable new_tb = db_.get("select name from sqlite_master where type='table' and name='" + NEW_TABLE_NAME + "';");
	if (false == mgr_.has_table(USER_TABLE_NAME))
		ret = init_db_table();

	if (check_user("admin")!=0)
	{
		//创建用户
		Json user;
		user.add_object_value("username", "admin");
		user.add_object_value("password", "admin");
		user.add_object_value("permission", 0);
		create(user);
#if 1
		Json d;
		user.add_object_value("username", "admin");

		int ds = get_user_info(d,"admin");
		LOG(LDEBUG, ds, " get_user_info");
		ds = check_password(d.get_string_value("username"), "admin");
		LOG(LDEBUG, ds, " check_password");
		get_user_permission(d.get_string_value("username"), ds);
		LOG(LDEBUG, ds, " get_user_permission");
#endif
	}
	clear();
	return ret;
}

int UserMgr::create(Json & user)
{
	static db::DbStmt stmt = \
		mgr_.new_stmt("INSERT INTO " + USER_TABLE_NAME + \
			" ('username','password','permission','status') VALUES(?,?,?,?);");

	//创建用户
	return stmt.bind_exec(user.get_string_value("username"), user.get_string_value("password"), user.get_int_value("permission"), 1);

}

int UserMgr::remove_and_active(const std::string & username)
{
	auto status = get_status( username);
	//已取消的激活
	if (0 == status)
	{
		status = 1;
		
	}
	//已激活的取消
	else
	{
		status = 0;
	}
	return reset_status(username,status);
}
//删除用户
int UserMgr::delete_user(const std::string & username)
{
	return delete_data_by_username(username);
}
//重置密码
int UserMgr::edit_password(const std::string & username, const std::string & new_password)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + USER_TABLE_NAME + " SET password=? WHERE username=?; ");

	return stmt.bind_exec(new_password, username);
}


int UserMgr::get_user_info(Json & user, const std::string & username)
{
	Json json = get_data(username);
	user.move(json);
	return 0;
}

int UserMgr::get_users(Json & users, const std::string & _where, const std::string & order, int beg, int len, size_t * p_all)
{
	return mgr_.get_datas(users,USER_TABLE_NAME,_where,order,beg,len,p_all);
}

size_t UserMgr::get_count()
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + USER_TABLE_NAME + " ;");
	Json json;
	stmt.bind_get_rows(json);

	return json.get_array_size();
}

int UserMgr::get_user_permission(const std::string & username,int &permission)
{
	Json user;
	permission = 3;
	//用户名为空 ,则是临时用户
	if (username == "")
		return permission;

	int ret = get_user_info(user,username);
	//获取用户信息成功
	if (0 == ret)
		permission = user.get_int_value("permission", 3);

	return permission;
}
//重置用户权限
int UserMgr::reset_user_permission(const std::string & username, int & permission)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + USER_TABLE_NAME + " SET permission=? WHERE username=?; ");
	return stmt.bind_exec(permission,username);
}

int UserMgr::check_password(const std::string & username, const std::string & password)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + USER_TABLE_NAME + " WHERE username = ? AND password=? AND status!=0;");
	Json json;
	if (stmt.bind_get_row(json, username, password) == 0)
		return 0;
	return -1;
}

int UserMgr::check_user(const std::string & username)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + USER_TABLE_NAME + " WHERE username = ? AND status!=0;");
	Json json;
	if (stmt.bind_get_row(json,username) == 0)
		return 0;
	return -1;
}

int UserMgr::clear()
{
	//static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + USER_TABLE_NAME + " SET status=0 WHERE permission=3;");
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + USER_TABLE_NAME + "  WHERE permission=3;");
	return stmt.bind_exec();
}

int UserMgr::delete_data_by_username(const std::string & username)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + USER_TABLE_NAME + "  WHERE username=?;");
	return stmt.bind_exec(username);
}

int UserMgr::delete_data_by_status(int status)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + USER_TABLE_NAME + "  WHERE status=?;");
	return stmt.bind_exec(status);
}

int UserMgr::get_status(const std::string & username)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT status FROM " + USER_TABLE_NAME + " WHERE username=?;");
	Json json;
	stmt.bind_get_row(json,username);
	return json.get_int_value("status");
}

Json UserMgr::get_data(const std::string & username)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + USER_TABLE_NAME + " WHERE username=?;");
	Json json;
	stmt.bind_get_row(json,username);
	return json;
}

int UserMgr::reset_status(const std::string & username, int status)
{
	db::DbStmt stmt = mgr_.new_stmt("UPDATE " + USER_TABLE_NAME + " SET status=? WHERE username=?;");
	Json json;
	return stmt.bind_exec(status, username);
}


UserMgr::~UserMgr()
{
	//db_.close();
}

int UserMgr::init_db_table()
{
	//创建数据表
	const std::string sql = "CREATE TABLE user (\
									username   TEXT PRIMARY KEY\
									UNIQUE\
									NOT NULL,\
									password   TEXT,\
									permission Byte DEFAULT(3)\
									NOT NULL,\
									status Byte DEFAULT(1)\
									NOT NULL\
									);\
									";
	int ret = mgr_.exec(sql);
	LOG(LDEBUG, ret, "user \n", sql);
	return ret;
}