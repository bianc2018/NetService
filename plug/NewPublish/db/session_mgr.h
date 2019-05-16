#ifndef SERVER_SESSION_MGR_H
#define SERVER_SESSION_MGR_H
#include <string>
#include <atomic>
#include <vector>

#include "user_mgr.h"
const std::string SESSION_TABLE_NAME("session");
class SessionMgr
{
public:
	int init(const std::string &db_path,int t_out=60*60*24*3);
	int login(Json &user);
	int logout(const std::string &code);
	int get_session(Json &session, const std::string &code);
	size_t get_count();
	std::string get_username(const std::string &session_id, const std::string &notfind="");
	int get_user_permission(const std::string &code,int notfind =3);
	int remove_session_by_username(const std::string&username);
	int delete_session_by_username(const std::string&username);
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
	int get_datas(Json &sessions, const std::string&_where, const std::string&order = "", int beg = 0, int len = -1,size_t *p_all=nullptr);
	//公共
	int delete_data_by_code(const std::string & code);
	int delete_data_by_status(int status);
	int get_status(const std::string & code);
	Json get_data(const std::string & code);
	Json get_all();
	int reset_status(const std::string & code, int status);
	~SessionMgr();
private:
	DbMgr mgr_;
	size_t time_out_= 60 * 60 * 24 * 3;
private:
	int init_db_table();
};


#endif