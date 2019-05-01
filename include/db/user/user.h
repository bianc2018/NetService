#ifndef SERVER_USER_H
#define SERVER_USER_H
#include <string>
#include <vector>

#include "../../json/json.h"
struct UserInfo
{
	std::string username;
	std::string password;
	int status;
	//权限 0 管理员 1 编辑 2 用户 3 游客
	int permission;
	UserInfo();
	int form_json(Json&json);
	Json to_json();
};
const std::string USER_TABLE_NAME("user");
typedef std::vector<UserInfo> vec_user_info;
#endif