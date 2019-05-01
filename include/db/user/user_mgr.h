#ifndef SERVER_USER_MGR_H
#define SERVER_USER_MGR_H
#include <string>
#include "user.h"
#include "../basedb/base_db.h"

class UserMgr
{
public:
	int init(const std::string &db_path);
	//创建用户
	int create(UserInfo &user);
	//激活或取消用户
	int remove_and_active(const std::string &username);
	//删除用户数据
	int delete_user(const std::string &username);
	//修改密码
	int edit_password(const std::string &username,const std::string &new_password);
	//获取用户信息
	int get_user_info(UserInfo &user,const std::string &username);
	//条件获取信息
	int get_users(vec_user_info&users, const std::string&_where, const std::string&order = "", int beg=0,int end=-1);
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
	~UserMgr();
private:
	BaseDb db_;
private:
	int init_db_table();
	int get_row(DBTable &table,UserInfo&it, int row);
};
#endif