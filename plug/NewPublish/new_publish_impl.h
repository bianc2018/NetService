#ifndef NEW_PUBLISH_IMPL_H
#define NEW_PUBLISH_IMPL_H

#include <boost/noncopyable.hpp>

#include "http_service.h"

#include "db/news_mgr.h"
#include "db/user_mgr.h"
#include "db/session_mgr.h"

#include "json/json.h"
#include "log/log.hpp"



//字符分割结果
typedef std::vector<std::string> split_vector;
//字符串分割函数 考虑单独起来
split_vector split(const std::string& s, const std::string&sub_split);

//错误码
enum WEB_FUNC_ERR_CODE
{
	//成功
	WEB_OK = 0,
	//权限不足
	WEB_PERMISSION_DENIED = -1000,
	//传入参数错误
	WEB_PARAMETERS_INCORRECT = -1001,
	//数据操作出错
	WEB_DATA_OPERATION = -1002,

	//每页信息条数 不可小于等于0
	WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO = -1003,
	//用户名或者密码为空
	WEB_PARAMETERS_INCORRECT_USER_PWD_NULL = -1004,
	//用户名重复
	WEB_PARAMETERS_INCORRECT_USER_REPEAT = -1005,
	//密码错误
	WEB_PERMISSION_DENIED_PWD_ERR = -1006,
	//管理员密码错误
	WEB_PERMISSION_DENIED_ADMIN_PWD_ERR = -1007,
	//不可以修改admin用户信息
	WEB_PERMISSION_DENIED_MOD_ADMIN = -1008,
};
#define SHARED_BUFF_PTR(size) std::shared_ptr<char>(new char[size], std::default_delete<char[]>())
#define SHARED_ANY_PTR(type,size) std::shared_ptr<type>(new type[size], std::default_delete<type[]>())
std::string replace(const std::string &src, const std::string &token, const std::string &rep);
class NewPublishImpl: public boost::noncopyable
{
public:
	static NewPublishImpl &instance();
public:
	int deal_get_data(HTTP_HANDLE handle);
	int deal_get_datas(HTTP_HANDLE handle);
	
	int deal_get_news(HTTP_HANDLE handle);
	int deal_get_sessions(HTTP_HANDLE handle);
	int deal_get_users(HTTP_HANDLE handle);
	
	//获取新闻
	int deal_get_new(HTTP_HANDLE handle);
	//获取当前用户信息
	int deal_get_now_user(HTTP_HANDLE handle);

	//登录
	int deal_login(HTTP_HANDLE handle);
	//注册
	int deal_register(HTTP_HANDLE handle);
	//登出
	int deal_logout(HTTP_HANDLE handle);

	//创建新闻
	int deal_create_new(HTTP_HANDLE handle);

	//保存新闻
	int deal_save_new(HTTP_HANDLE handle);

	//上传图片
	int deal_upload_new_image(HTTP_HANDLE handle);

	//点赞
	int deal_new_like(HTTP_HANDLE handle);

	//增加评论，可以考虑合并在创建新闻那
	int deal_add_new_comment(HTTP_HANDLE handle);

	//移除新闻数据
	int deal_remove_new(HTTP_HANDLE handle);

	//移除用户数据
	int deal_remove_user(HTTP_HANDLE handle);

	//删除用户数据
	int deal_delete_user(HTTP_HANDLE handle);

	//删除新闻
	int deal_delete_new(HTTP_HANDLE handle);

	//重置密码
	int deal_reset_password(HTTP_HANDLE handle);

	//重置用户权限
	//reset_user_permission.action
	int deal_reset_user_permission(HTTP_HANDLE handle);

	//重置新闻信息
	//reset_new_status.action
	int deal_reset_new_status(HTTP_HANDLE handle);

	//清理过期会话数据
	///detele_sessions.action
	int deal_detele_sessions(HTTP_HANDLE handle);

	int before_filter(HTTP_HANDLE handle);
	int after_filter(HTTP_HANDLE handle);
private:
	NewsMgr new_mgr_;
	SessionMgr session_mgr_;
	UserMgr user_mgr_;

	//权限
	std::map<std::string, int> permission_map_;
	//路径
	std::string uri_path_;
	std::string web_root_;

	std::string index_page_;
private:
	NewPublishImpl();

	int write_json(HTTP_HANDLE handle,const  Json& json);
	int read_json(HTTP_HANDLE handle, Json &json);

	std::string get_cookie(HTTP_HANDLE handle,const std::string &key, const std::string &notfond="");
	int set_cookie(HTTP_HANDLE handle, const std::string &key, const std::string &value);

	std::string get_uri_value(HTTP_HANDLE handle, const std::string &key, const std::string &notfond = "");

	void get_news(Json &data,const std::string& _where,const std::string &orderby,int beg,int end,int tran=0);
	void get_sessions(Json &data, const std::string& _where, const std::string &orderby, int beg, int end);
	void get_users(Json &data, const std::string& _where, const std::string &orderby, int beg, int end);
private:
	//过滤函数
	//cookie 更新
	int filter_session(HTTP_HANDLE handle);
	//权限鉴别
	int filter_permission(HTTP_HANDLE handle);
	//首页重定位
	int filter_index(HTTP_HANDLE handle);
	
};
#endif