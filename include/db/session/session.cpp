#include "session.h"

SessionInfo::SessionInfo() :port(0), status(1), login_time(0), active_time(0), permission(3)
{
	
}

int SessionInfo::from_json(Json & json)
{
	/*
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
	*/
	json.get_value(code, "code");
	json.get_value(ip, "ip");
	json.get_value(port, "port");
	json.get_value(username, "username");
	json.get_value(login_time, "login_time");
	json.get_value(active_time, "active_time");
	json.get_value(status, "status");
	json.get_value(permission, "permission");
	return 0;
}

Json SessionInfo::to_json()
{
	Json json;
	json.add_object_value("code", code);
	json.add_object_value("ip", ip);
	json.add_object_value("port", port);
	json.add_object_value("username", username);
	json.add_object_value("login_time", login_time);
	json.add_object_value("active_time", active_time);
	json.add_object_value("status", status);
	json.add_object_value("permission", permission);
	return json;
}
