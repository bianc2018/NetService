#ifndef SERVER_USER_MGR_H
#define SERVER_USER_MGR_H
#include <string>
#include "user.h"
#include "../basedb/base_db.h"

class UserMgr
{
public:
	int init(const std::string &db_path);
	//�����û�
	int create(UserInfo &user);
	//�����ȡ���û�
	int remove_and_active(const std::string &username);
	//ɾ���û�����
	int delete_user(const std::string &username);
	//�޸�����
	int edit_password(const std::string &username,const std::string &new_password);
	//��ȡ�û���Ϣ
	int get_user_info(UserInfo &user,const std::string &username);
	//������ȡ��Ϣ
	int get_users(vec_user_info&users, const std::string&_where, const std::string&order = "", int beg=0,int end=-1);
	//��ȡȨ��
	int get_user_permission(const std::string & username, int &permission);
	//�޸�Ȩ��
	int reset_user_permission(const std::string & username, int &permission);
	//�������
	int check_password(const std::string&username,const std::string &password);
	//����û��Ƿ����
	int check_user(const std::string&username);
	//�����ʱ�˻�
	int clear();
	~UserMgr();
private:
	BaseDb db_;
private:
	int init_db_table();
	int get_row(DBTable &table,UserInfo&it, int row);
};
#endif