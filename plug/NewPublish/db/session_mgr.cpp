#include "session_mgr.h"


#include "log/log.hpp"

int SessionMgr::init(const std::string & db_path, int t_out)
{
	//会话有效时长
	time_out_ = t_out;
	int ret = mgr_.db_init(db_path);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "打开数据失败 url= ", db_path);
		return ret;
	}
	mgr_.set_synchronous(2);
	//查询表项
	//DBTable new_tb = db_.get("select name from sqlite_master where type='table' and name='" + NEW_TABLE_NAME + "';");
	if (false == mgr_.has_table(SESSION_TABLE_NAME))
		ret = init_db_table();

	return ret;
}

int SessionMgr::login(Json & user)
{
	static db::DbStmt stmt = \
		mgr_.new_stmt("INSERT INTO " + SESSION_TABLE_NAME + \
			"('code','ip','port','username','login_time','active_time','status','permission') VALUES(?,?,?,?,?,?,?,?);");
	
	auto code = mgr_.get_unique_code();
	auto login_time = time(nullptr);
	auto active_time = time(nullptr);
	auto status = 1;
	
	if (0 == stmt.bind_exec(code, user.get_string_value("ip"), user.get_int_value("port"), user.get_string_value("username"), \
		login_time, active_time, status, user.get_int_value("permission")))
	{
		user.add_object_value("code", code);
		return 0;
	}
	return -1;
}

int SessionMgr::logout(const std::string & code)
{
	return reset_status(code, 0);
}

int SessionMgr::get_session(Json & session, const std::string & code)
{
	Json json = get_data(code);
	session = json;
	return 0;
}

size_t SessionMgr::get_count()
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + SESSION_TABLE_NAME + " ;");
	Json json;
	stmt.bind_get_rows(json);

	return json.get_array_size();
}

std::string SessionMgr::get_username(const std::string & session_id, const std::string &notfind)
{
	Json  session;
	if (get_session(session, session_id) != 0)
		return notfind;

	return session.get_string_value("username",notfind);
}

int SessionMgr::get_user_permission(const std::string & code, int notfind)
{
	Json  session;
	if (get_session(session, code) != 0)
		return notfind;

	return session.get_int_value("permission", notfind);
}

int SessionMgr::remove_session_by_username(const std::string & username)
{
	db::DbStmt stmt = mgr_.new_stmt("UPDATE " + SESSION_TABLE_NAME + " SET status=0 WHERE username=?;");
	return stmt.bind_exec(username);
}

int SessionMgr::delete_session_by_username(const std::string & username)
{
	db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + SESSION_TABLE_NAME + "  WHERE username=?;");
	return stmt.bind_exec(username);
}

int SessionMgr::active(const std::string & code)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + SESSION_TABLE_NAME + " SET active_time=? WHERE code=? AND status!=0; ");
	//更新活跃时间
	size_t active_time = time(nullptr);
	return stmt.bind_exec(active_time, code);
}
//-1 不存在 -2 已失效 -3 超时 0 活跃
int SessionMgr::check_session(const std::string & code)
{
	//是否存在
	auto  json = get_data(code);
	if (json.get_string_value("code","") == "")
	{
		LOG(LDEBUG, " session 不存在");
		return  -1;
	}

	//是否失效
	int status = json.get_int_value("status");
	if (0 == status)
	{
		LOG(LDEBUG, " session 无效");
		return -2;
	}

	//是否活跃
	size_t active_time = json.get_int_value("active_time");;
	size_t local_time = time(nullptr);
	if (local_time - active_time > time_out_)
	{
		LOG(LDEBUG, " session 过期");
		return -3;
	}
	return 0;
}

int SessionMgr::delete_sesssion()
{
	clear();
	//删除状态为0的数据
	return delete_data_by_status(0);
	
}

int SessionMgr::clear()
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + SESSION_TABLE_NAME + " SET status=0 WHERE username='' AND status!=0; ");
	//游客清空
	int ret = stmt.bind_exec();
	LOG(LDEBUG, ret," clear session visitor");

	//检查超时
	Json sessions = get_all();
	LOG(LDEBUG, sessions.get_array_size()," session size");

	auto sessions_vec = sessions.get_array();
	for (auto session:sessions_vec)
	{
		int active_time = session.get_int_value("active_time");
		int status = session.get_int_value("status");
		//检查活跃时长
		if (status!=0 &&time(nullptr) - active_time > time_out_)
		{
			//超时,退出登录
			std::string code = session.get_string_value("code");
			logout(code);
		}
	}
	return 0;
}

int SessionMgr::get_datas(Json & sessions, const std::string & _where, const std::string & order, int beg, int len, size_t * p_all)
{
	return mgr_.get_datas(sessions,SESSION_TABLE_NAME,_where,order,beg,len,p_all);
}

int SessionMgr::delete_data_by_code(const std::string & code)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + SESSION_TABLE_NAME + "  WHERE code=?;");
	return stmt.bind_exec(code);
}

int SessionMgr::delete_data_by_status(int status)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + SESSION_TABLE_NAME + "  WHERE status=?;");
	return stmt.bind_exec(status);
}

int SessionMgr::get_status(const std::string & code)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT status FROM " + SESSION_TABLE_NAME + " WHERE code=?;");
	Json json;
	stmt.bind_get_row(json,code);
	return json.get_int_value("status");
}

Json SessionMgr::get_data(const std::string & code)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + SESSION_TABLE_NAME + " WHERE code=?;");
	Json json;
	stmt.bind_get_row(json,code);
	LOG(LDEBUG, "get data", json.to_string());
	return json;
}

Json SessionMgr::get_all()
{
	db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM "+SESSION_TABLE_NAME+";");
	Json json;
	stmt.bind_get_rows(json);
	return json;
}

int SessionMgr::reset_status( const std::string & code, int status)
{
	db::DbStmt stmt = mgr_.new_stmt("UPDATE "+SESSION_TABLE_NAME+" SET status=? WHERE code=?;");
	return stmt.bind_exec(status,code);
}

SessionMgr::~SessionMgr()
{
	clear();
}

int SessionMgr::init_db_table()
{
	//创建数据表
	const std::string NEWS_TABLE = "CREATE TABLE session ( \
									code        TEXT PRIMARY KEY \
									UNIQUE\
									NOT NULL,\
									ip          TEXT,\
									port        Integer,\
									username    TEXT NOT NULL,\
									login_time  Integer NOT NULL,\
									active_time Integer NOT NULL,\
									status Byte DEFAULT(1),\
									permission Byte DEFAULT(3)\
									NOT NULL\
									);\
									";
	return mgr_.exec(NEWS_TABLE);
}

