#include "session_mgr.h"


#include "../../log/log.hpp"

int SessionMgr::init(const std::string & db_path, int t_out)
{
	//会话有效时长
	time_out_ = t_out;
	code_p_ = 0;
	int ret = db_.connect(db_path);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "打开数据失败 url= ", db_path);
		return ret;
	}
	//查询表项
	DBTable new_tb = db_.get("select name from sqlite_master where type='table' and name='" + SESSION_TABLE_NAME + "';");
	if (new_tb.get_rows_num() <= 0)
		ret = init_db_table();

	return ret;
}

int SessionMgr::login(SessionInfo & user)
{
	get_unique_code(user);
	
	user.login_time = LOCAL_TIME;
	user.active_time = LOCAL_TIME;
	user.status = 1;
	
	std::string sql = SQL_INSERT_ARGS(SESSION_TABLE_NAME, "('code','ip','port','username','login_time','active_time','status','permission')",user.code,user.ip,user.port,user.username,user.login_time,user.active_time,user.status, user.permission);
	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " session ", sql);
	return ret;
}

int SessionMgr::logout(const std::string & code)
{
	std::string status= "0";
	std::string sql = SQL_UPDATA_STATUS(SESSION_TABLE_NAME, CONDITION_EQ(status), CONDITION_EQ(code));

	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " session ", sql);
	return ret;
}

int SessionMgr::get_session(SessionInfo & session, const std::string & code)
{
	//int ret = check_session(code);
	//if (0 == ret)
	//{
	std::string sql = SQL_SELECT_STATUS(SESSION_TABLE_NAME, CONDITION_EQ(code));
	DBTable tb = db_.get(sql);
	if (tb.get_rows_num() > 0)
	{
		get_row(tb, session, 0);
		return 0;
	}
	//ret = -1;
//}
//return ret;
	return -1;
}

int SessionMgr::create_visit(std::string & session_id)
{
	SessionInfo info;
	info.username = "";
	int ret = login(info);
	if(0== ret)
		session_id = info.code;
	return ret;
}

std::string SessionMgr::get_username(const std::string & session_id, const std::string &notfind)
{
	SessionInfo  session;
	if (get_session(session, session_id) != 0)
		return notfind;

	return session.username;
}

int SessionMgr::get_user_permission(const std::string & code, int notfind)
{
	SessionInfo  session;
	if (get_session(session, code) != 0)
		return notfind;

	return session.permission;
}





int SessionMgr::active(const std::string & code)
{
	//更新活跃时间
	size_t active_time = LOCAL_TIME;
	std::string sql = SQL_UPDATA_STATUS(SESSION_TABLE_NAME, CONDITION_EQ(active_time), CONDITION_EQ(code));

	int ret = db_.exec(sql);
	LOG(LDEBUG, ret, " session ", sql);
	return ret;
}
//-1 不存在 -2 已失效 -3 超时 0 活跃
int SessionMgr::check_session(const std::string & code)
{
	//是否存在
	std::string sql = SQL_SELECT(SESSION_TABLE_NAME,CONDITION_EQ(code));
	int ret=0;
	DBTable tb = db_.get(sql);
	LOG(LDEBUG,  " session ", sql);
	if (tb.get_rows_num() <= 0)
	{
		LOG(LDEBUG, " session 不存在");
		return  -1;
	}

	//是否失效
	int status = 0;
	GET_CELL_INT0(tb, status);
	if (0 == status)
	{
		LOG(LDEBUG, " session 无效");
		return -2;
	}

	//是否活跃
	size_t active_time;
	size_t local_time = LOCAL_TIME;
	GET_CELL_INT0(tb, active_time);//s
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
	int status = 0;
	std::string sql = SQL_DELETE(SESSION_TABLE_NAME, CONDITION_EQ(status));
	int ret = db_.exec(sql);
	if (ret == 0)
		return 0;
	else
		return -1;
}

int SessionMgr::clear()
{
	//游客清空
	std::string username = "";
	int status = 0;
	std::string sql = SQL_UPDATA_STATUS(SESSION_TABLE_NAME,CONDITION_EQ(status), CONDITION_EQ(username));
	int ret = db_.exec(sql);
	LOG(LDEBUG, ret," session ", sql);

	//检查超时
	sql = SQL_SELECT_STATUS(SESSION_TABLE_NAME, "1=1");
	DBTable tb = db_.get(sql);
	LOG(LDEBUG, tb.get_rows_num()," session ", sql);
	int active_time = 0;
	for (int i = 0; i < tb.get_rows_num(); i++)
	{
		GET_CELL_INT(tb, active_time, i);
		//检查活跃时长
		if (LOCAL_TIME - active_time > time_out_)
		{
			//超时,退出登录
			std::string code;
			GET_CELL(tb, code, i);
			logout(code);
		}
	}
	return 0;
}

int SessionMgr::get_datas(vec_session_info & sessions, const std::string & _where, const std::string & order, int beg, int end)
{
	std::string sql;
	std::string other = "1=1 ";
	//条件
	if (_where != "")
		other += " AND " + _where;
	//排序
	if ("" != order)
		other += " Order By " + order;

	sql = SQL_SELECT(SESSION_TABLE_NAME, other);
	DBTable table = db_.get(sql);
	int row_num = table.get_rows_num();
	LOG(LDEBUG, row_num, " new  \n", sql);
	if (0 > row_num)
		return -1;
	else
	{
		//是否越界
		if (end <= 0 || end > row_num)
			end = row_num;
		if (beg < 0 || beg>end)
			beg = 0;

		for (int i = beg; i < end; i++)
		{
			SessionInfo info;
			get_row(table, info, i);
			sessions.push_back(info);
		}
		return row_num;
	}
}

SessionMgr::~SessionMgr()
{
	clear();
	db_.close();
}

int SessionMgr::get_row(DBTable & table, SessionInfo & it, int row)
{
	GET_CELL(table, it.code, row);
	GET_CELL(table, it.ip, row);
	GET_CELL(table, it.username, row);

	GET_CELL_INT(table, it.port, row);
	GET_CELL_INT(table, it.status, row);
	GET_CELL_INT(table, it.login_time, row);
	GET_CELL_INT(table, it.active_time, row);
	GET_CELL_INT(table, it.permission, row);
	return 0;
}

int SessionMgr::init_db_table()
{
	//创建数据表
	const std::string NEWS_TABLE = "CREATE TABLE session ( \
									code        TEXT PRIMARY KEY \
									UNIQUE\
									NOT NULL,\
									ip          TEXT,\
									port        TEXT,\
									username    TEXT NOT NULL,\
									login_time  TEXT NOT NULL,\
									active_time TEXT NOT NULL,\
									status TEXT DEFAULT(1),\
									permission TEXT DEFAULT(3)\
									NOT NULL\
									);\
									";
	return db_.exec(NEWS_TABLE);
}

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

int SessionMgr::get_unique_code(SessionInfo & user)
{
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); // 这里是两个() ，因为这里是调用的 () 的运算符重载
	user.code = boost::uuids::to_string(a_uuid);
	////ip+username+login time
	//std::hash<std::string> hash_string;
	/*
	int i = 0;
	int p = ++code_p_;
	while (true)
	{
		std::string code = sql_format("",time_to_string(), p,i);
		if (-1 == check_session(code))
		{
			user.code = code;
			return 0;
		}
		i++;
	}
	*/
	return 0;
}
