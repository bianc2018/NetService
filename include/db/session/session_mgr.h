#ifndef SERVER_SESSION_MGR_H
#define SERVER_SESSION_MGR_H
#include <string>
#include <atomic>
#include <vector>

#include "../basedb/base_db.h"
#include "../user/user_mgr.h"
#include "session.h"

class SessionMgr
{
public:
	int init(const std::string &db_path,int t_out=60*60*24*3);
	int login(SessionInfo &user);
	int logout(const std::string &code);
	int get_session(SessionInfo &session, const std::string &code);
	int create_visit(std::string & session_id);
	std::string get_username(const std::string &session_id, const std::string &notfind="");
	int get_user_permission(const std::string &code,int notfind =3);
	//活动 更新时间
	int active(const std::string &code);
	//查看会话是否过期 超时 -1 不存在 -2 已失效 -3 超时 0 活跃
	int check_session(const std::string &code);
	
	//清理过期会话
	//删除数据
	int delete_sesssion();
	//置零
	int clear();
	
	//条件获取信息
	int get_datas(vec_session_info&sessions, const std::string&_where, const std::string&order = "", int beg = 0, int end = -1);

	~SessionMgr();
private:
	BaseDb db_;
	size_t time_out_;
	std::atomic_int code_p_;
private:
	int get_row(DBTable &table, SessionInfo&it, int row);
	int init_db_table();
	int get_unique_code(SessionInfo &user);
};


#endif