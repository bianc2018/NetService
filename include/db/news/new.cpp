#include "new.h"
/*
//Ψһ����
	std::string code;
	//����
	std::string title;
	//����
	std::string author;
	//����
	std::string content;

	//���� 0 ���� 1 ����
	int property;
	//���� ���������һ�������� Ϊ"" ���� �����ʾ�����۵����Ż�����
	std::string parent;
	//״̬ 0 ɾ�� 1 ���� 2 ���� 3 ����
	int status;
	//������ �������� ��������
	int comment_num;
	int like_num;
	int unlike_num;
	//����ʱ�� ������ �޸��� �޸�ʱ�� ����ʱ�� 201901052214
	size_t create_time;
	std::string create_user;
	size_t modify_time;
	std::string modify_user;
	size_t publish_time;
*/
NewInfo::NewInfo():property(0), status(1), comment_num(0), like_num(0),unlike_num(0),modify_time(0),create_time(0), publish_time(0)
{
}
int NewInfo::from_json(Json & json)
{
	json.get_value(code, "code");
	json.get_value(title, "title");
	json.get_value(author, "author");
	json.get_value(content, "content");
	json.get_value(property, "property");
	json.get_value(parent, "parent");
	json.get_value(status, "status");
	json.get_value(comment_num, "comment_num");
	json.get_value(like_num, "like_num");
	json.get_value(unlike_num, "unlike_num");
	json.get_value(data_path, "data_path");
	json.get_value(create_time, "create_time");
	json.get_value(create_user, "create_user");
	json.get_value(modify_time, "modify_time");
	json.get_value(modify_user, "modify_user");
	json.get_value(publish_time, "publish_time");
	return 0;
}

Json NewInfo::to_json()
{
	Json json;
	json.add_object_value("code", code);
	json.add_object_value("title", title);
	json.add_object_value("author", author);
	json.add_object_value("content", content);
	json.add_object_value("property", property);
	json.add_object_value("parent", parent);
	json.add_object_value("status", status);
	json.add_object_value("comment_num", comment_num);
	json.add_object_value("like_num", like_num);
	json.add_object_value("unlike_num", unlike_num);
	json.add_object_value("data_path", data_path);
	json.add_object_value("create_time", create_time);
	json.add_object_value("create_user", create_user);
	json.add_object_value("modify_time", modify_time);
	json.add_object_value("modify_user", modify_user);
	json.add_object_value("publish_time", publish_time);
	return json;
}
