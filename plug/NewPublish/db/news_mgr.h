#ifndef SERVER_NEWS_MGR_H
#define SERVER_NEWS_MGR_H
#include <string>
#include <vector>

#include "db_mgr.h"
const std::string NEW_TABLE_NAME("news");
const std::string NEW_AND_LIKE_TABLE_NAME("new_like");

//�������۹�����
class NewsMgr
{
public:
	int init(const std::string &db_path);
	~NewsMgr();
	//����
	int create(Json &it);
	//����
	int save(Json &it);
	int save_info(Json &it);
	int save_info(const std::string & code,const std::string &key, const std::string &value);
	int save_info(const std::string & code, const std::string &key,const int &value);
	//������Դ·��
	int set_data_path(const std::string & code,const std::string &value);
	const std::string get_data_path(const std::string & code, const std::string &nofind="");
	//��ȡһ����Ϣ
	std::string get_info(const std::string & code,const std::string &key,const std::string &nofind);
	int get_info(const std::string & code, const std::string &key, const int &nofind);
	int save_content(const std::string &code,const std::string &content);
	//�޻�� 1 ��  -1 ��
	int like(const std::string &username, const std::string &new_id, int blike);
	int get_like_status(const std::string &username, const std::string &new_id);
	//��ȡ���� property!=-1 ����property��ȡ����
	int get_data(Json &it, const std::string &code, int property=-1);
	//������ȡ��Ϣ
	int get_datas(Json&news, const std::string&_where, const std::string&order="",int beg=0,int len=-1,size_t *p_all = nullptr, int transfer_content=0);
	size_t get_count(const std::string&_where);
	size_t get_count();
	//�Ƴ�  ����ʾ ���ݱ���
	int remove(const std::string &code);
	//ɾ�� ɾ������
	int delete_it(const std::string &code);
	//ɾ�� ����
	int delete_comment(const std::string &code);
	//ɾ�� ����
	int delete_like(const std::string &code);
	//��ѯ״̬ -1 ������ ������NewMgr
	int get_status(const std::string &code);
	//����״̬
	int reset_status(const std::string &code, const int status);
	//����
	int comment(Json &it);
	//clear ����������� δ���������
	int clear();
	//��������
	int delete_data_by_code(const std::string & code);
	int delete_data_by_status(int status);
	
	Json get_data(const std::string & code);
private:
	DbMgr mgr_;
private:
	int init_db_table();
};
#endif