#include "user.h"

UserInfo::UserInfo() :permission(3), status(0)
{
}

int UserInfo::form_json(Json & json)
{
	json.get_value(username, "username");
	json.get_value(password, "password");
	json.get_value(permission, "permission");
	json.get_value(status, "status");
	return 0;
}

Json UserInfo::to_json()
{
	Json json;
	json.add_object_value("username", username);
	json.add_object_value("password", password);
	json.add_object_value("permission", permission);
	json.add_object_value("status", status);
	return json;
}
