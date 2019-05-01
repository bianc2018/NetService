#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H
#include <string>
#include "../basedb/base_db.h"
#include "../../json/json.h"

struct SessionInfo
{
	SessionInfo();
	//编号 可以是cookie
	//ip+username+login time
	std::string code;
	//用户ip
	std::string ip;
	//端口
	int port;
	//用户信息
	std::string username;
	//登录信息
	size_t login_time;
	size_t active_time;
	//
	int status;
	//权限 0 管理员 1 编辑 2 用户 3 游客
	int permission;

	int from_json(Json &json);
	Json to_json();
};
const std::string SESSION_TABLE_NAME("session");

typedef std::vector<SessionInfo> vec_session_info;

#endif