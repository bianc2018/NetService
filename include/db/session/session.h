#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H
#include <string>
#include "../basedb/base_db.h"
#include "../../json/json.h"

struct SessionInfo
{
	SessionInfo();
	//��� ������cookie
	//ip+username+login time
	std::string code;
	//�û�ip
	std::string ip;
	//�˿�
	int port;
	//�û���Ϣ
	std::string username;
	//��¼��Ϣ
	size_t login_time;
	size_t active_time;
	//
	int status;
	//Ȩ�� 0 ����Ա 1 �༭ 2 �û� 3 �ο�
	int permission;

	int from_json(Json &json);
	Json to_json();
};
const std::string SESSION_TABLE_NAME("session");

typedef std::vector<SessionInfo> vec_session_info;

#endif