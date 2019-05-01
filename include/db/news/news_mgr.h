#ifndef SERVER_NEWS_MGR_H
#define SERVER_NEWS_MGR_H
#include <string>
#include <vector>

#include "../basedb/base_db.h"

#include "new.h"

//�������۹�����
class NewsMgr
{
public:
	int init(const std::string &db_path);
	~NewsMgr();
	//����
	int create(NewInfo &it);
	//����
	int save(NewInfo &it);
	int save_info(NewInfo &it);
	int save_info(const std::string & code,const std::string &key, const std::string &value);
	//������Դ·��
	int set_data_path(const std::string & code,const std::string &value);
	const std::string get_data_path(const std::string & code, const std::string &nofind="");
	//��ȡһ����Ϣ
	std::string get_info(const std::string & code,const std::string &key,const std::string &nofind="");
	int save_content(const std::string &code,const std::string &content);
	//�޻�� 1 ��  -1 ��
	int like(const std::string &username, const std::string &new_id, int blike);
	int get_like_status(const std::string &username, const std::string &new_id);
	//��ȡ���� property!=-1 ����property��ȡ����
	int get_data(NewInfo &it, const std::string &code, int property=-1);
	//������ȡ��Ϣ
	int get_datas(vec_new_info&news, const std::string&_where, const std::string&order="",int beg=0,int end=-1);
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
	int comment(NewInfo &it);
	//clear ����������� δ���������
	int clear();
private:
	BaseDb db_;
private:
	int get_row(DBTable &table,NewInfo&it, int row);
	int init_db_table();
	int get_unique_code(NewInfo &it);
};
#endif