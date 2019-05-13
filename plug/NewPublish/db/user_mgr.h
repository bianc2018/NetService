#ifndef SERVER_USER_MGR_H
#define SERVER_USER_MGR_H
#include <string>

#include "db_mgr.h"
const std::string USER_TABLE_NAME("user");
class UserMgr
{
public:
	int init(const std::string &db_path);
	//创建用户
	int create(Json &user);
	//激活或取消用户
	int remove_and_active(const std::string &username);
	//删除用户数据
	int delete_user(const std::string &username);
	//修改密码
	int edit_password(const std::string &username,const std::string &new_password);
	//获取用户信息
	int get_user_info(Json &user,const std::string &username);
	//条件获取信息
	int get_users(Json&users, const std::string&_where, const std::string&order = "", int beg=0,int len=-1,size_t *p_all=nullptr);
	//get_count()
	size_t get_count();
	//获取权限
	int get_user_permission(const std::string & username, int &permission);
	//修改权限
	int reset_user_permission(const std::string & username, int &permission);
	//检查密码
	int check_password(const std::string&username,const std::string &password);
	//检查用户是否存在
	int check_user(const std::string&username);
	//清除临时账户
	int clear();
	//公共
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