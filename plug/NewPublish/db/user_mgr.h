#ifndef SERVER_USER_MGR_H
#define SERVER_USER_MGR_H
#include <string>

#include "db_mgr.h"
const std::string USER_TABLE_NAME("user");
class UserMgr
{
public:
	int init(const std::string &db_path);
	//�����û�
	int create(Json &user);
	//�����ȡ���û�
	int remove_and_active(const std::string &username);
	//ɾ���û�����
	int delete_user(const std::string &username);
	//�޸�����
	int edit_password(const std::string &username,const std::string &new_password);
	//��ȡ�û���Ϣ
	int get_user_info(Json &user,const std::string &username);
	//������ȡ��Ϣ
	int get_users(Json&users, const std::string&_where, const std::string&order = "", int beg=0,int len=-1,size_t *p_all=nullptr);
	//get_count()
	size_t get_count();
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
	//����
	int delete_data_by_username(const std::string & username);
	int delete_data_by_status(int status);
	int get_status(const std::string & username);
	Json get_data(const std::string & username);
	int reset_status(const std::string & username, int status);
	~UserMgr();
private:
	DbMgr mgr_;
private:
	int init_db_table();
};
#endif