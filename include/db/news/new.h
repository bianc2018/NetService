#ifndef SERVER_NEW_H
#define SERVER_NEW_H
#include <string>
#include <vector>

#include "../basedb/base_db.h"
#include "../../json/json.h"
//״̬
constexpr auto OK = 0;
//������
constexpr auto EXIST = -1;
//δ֪����
constexpr auto UNKNOW = -90;

//���Ż�����
struct NewInfo
{
	NewInfo();
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
	std::string data_path;
	//����ʱ�� ������ �޸��� �޸�ʱ�� ����ʱ�� 201901052214
	size_t create_time;
	std::string create_user;
	size_t modify_time;
	std::string modify_user;
	size_t publish_time;
	
	int from_json(Json &json);
	Json to_json();
};


typedef std::vector<NewInfo> vec_new_info;
//���ַ���
const std::string NONE("");
//���ű���
const std::string NEW_TABLE_NAME("news");
const std::string NEW_AND_LIKE_TABLE_NAME("new_like");
#endif