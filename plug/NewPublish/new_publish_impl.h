#ifndef NEW_PUBLISH_IMPL_H
#define NEW_PUBLISH_IMPL_H

#include <boost/noncopyable.hpp>

#include "http_service.h"

#include "db/news_mgr.h"
#include "db/user_mgr.h"
#include "db/session_mgr.h"

#include "json/json.h"
#include "log/log.hpp"



//�ַ��ָ���
typedef std::vector<std::string> split_vector;
//�ַ����ָ�� ���ǵ�������
split_vector split(const std::string& s, const std::string&sub_split);

//������
enum WEB_FUNC_ERR_CODE
{
	//�ɹ�
	WEB_OK = 0,
	//Ȩ�޲���
	WEB_PERMISSION_DENIED = -1000,
	//�����������
	WEB_PARAMETERS_INCORRECT = -1001,
	//���ݲ�������
	WEB_DATA_OPERATION = -1002,

	//ÿҳ��Ϣ���� ����С�ڵ���0
	WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO = -1003,
	//�û�����������Ϊ��
	WEB_PARAMETERS_INCORRECT_USER_PWD_NULL = -1004,
	//�û����ظ�
	WEB_PARAMETERS_INCORRECT_USER_REPEAT = -1005,
	//�������
	WEB_PERMISSION_DENIED_PWD_ERR = -1006,
	//����Ա�������
	WEB_PERMISSION_DENIED_ADMIN_PWD_ERR = -1007,
	//�������޸�admin�û���Ϣ
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
	
	//��ȡ����
	int deal_get_new(HTTP_HANDLE handle);
	//��ȡ��ǰ�û���Ϣ
	int deal_get_now_user(HTTP_HANDLE handle);

	//��¼
	int deal_login(HTTP_HANDLE handle);
	//ע��
	int deal_register(HTTP_HANDLE handle);
	//�ǳ�
	int deal_logout(HTTP_HANDLE handle);

	//��������
	int deal_create_new(HTTP_HANDLE handle);

	//��������
	int deal_save_new(HTTP_HANDLE handle);

	//�ϴ�ͼƬ
	int deal_upload_new_image(HTTP_HANDLE handle);

	//����
	int deal_new_like(HTTP_HANDLE handle);

	//�������ۣ����Կ��Ǻϲ��ڴ���������
	int deal_add_new_comment(HTTP_HANDLE handle);

	//�Ƴ���������
	int deal_remove_new(HTTP_HANDLE handle);

	//�Ƴ��û�����
	int deal_remove_user(HTTP_HANDLE handle);

	//ɾ���û�����
	int deal_delete_user(HTTP_HANDLE handle);

	//ɾ������
	int deal_delete_new(HTTP_HANDLE handle);

	//��������
	int deal_reset_password(HTTP_HANDLE handle);

	//�����û�Ȩ��
	//reset_user_permission.action
	int deal_reset_user_permission(HTTP_HANDLE handle);

	//����������Ϣ
	//reset_new_status.action
	int deal_reset_new_status(HTTP_HANDLE handle);

	//������ڻỰ����
	///detele_sessions.action
	int deal_detele_sessions(HTTP_HANDLE handle);

	int before_filter(HTTP_HANDLE handle);
	int after_filter(HTTP_HANDLE handle);
private:
	NewsMgr new_mgr_;
	SessionMgr session_mgr_;
	UserMgr user_mgr_;

	//Ȩ��
	std::map<std::string, int> permission_map_;
	//·��
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
	//���˺���
	//cookie ����
	int filter_session(HTTP_HANDLE handle);
	//Ȩ�޼���
	int filter_permission(HTTP_HANDLE handle);
	//��ҳ�ض�λ
	int filter_index(HTTP_HANDLE handle);
	
};
#endif