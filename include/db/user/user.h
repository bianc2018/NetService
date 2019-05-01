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
	//Ȩ�� 0 ����Ա 1 �༭ 2 �û� 3 �ο�
	int permission;
	UserInfo();
	int form_json(Json&json);
	Json to_json();
};
const std::string USER_TABLE_NAME("user");
typedef std::vector<UserInfo> vec_user_info;
#endif