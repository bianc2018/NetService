#ifndef NEW_PUBLISH_IMPL_H
#define NEW_PUBLISH_IMPL_H

#include <boost/noncopyable.hpp>

#include "http_service.h"

#include "db/news/news_mgr.h"
#include "db/user/user_mgr.h"
#include "db/session/session_mgr.h"

#include "json/json.h"
#include "log/log.hpp"

const std::string SET_COOKIE("Set-Cookie");

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

class NewPublishImpl: public boost::noncopyable
{
public:
	static NewPublishImpl &instance();
public:
	//�ӿ�
	//��ȡ�����б�
	int deal_get_news(HTTP_HANDLE handle);

	//��ȡ�Ự�б�
	int deal_get_sessions(HTTP_HANDLE handle);

	//��ȡ�û��б�
	int deal_get_users(HTTP_HANDLE handle);

	//��ȡ����
	int deal_get_new(HTTP_HANDLE handle);

	//��¼
	int deal_login(HTTP_HANDLE handle);

	//ע��
	int deal_register(HTTP_HANDLE handle);

	//�ǳ�
	int deal_logout(HTTP_HANDLE handle);

	//��ȡ��ǰ�û���Ϣ
	int deal_get_now_user(HTTP_HANDLE handle);

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
public:
	NewsMgr new_mgr_;
	SessionMgr session_mgr_;
	UserMgr user_mgr_;
private:
	NewPublishImpl();

	int write_json(HTTP_HANDLE handle,const  Json& json);
	int read_json(HTTP_HANDLE handle, Json &json);

	std::string get_cookie(HTTP_HANDLE handle,const std::string &key, const std::string &notfond="");
	int set_cookie(HTTP_HANDLE handle, const std::string &key, const std::string &value);

private:
	//���˺���
	//cookie ����
	int filter_session(HTTP_HANDLE handle);
	//Ȩ�޼���
	int filter_permission(HTTP_HANDLE handle);

	//cookie����
	int filter_cookie(HTTP_HANDLE handle);
	int filter_set_cookie(HTTP_HANDLE handle);

};
#endif