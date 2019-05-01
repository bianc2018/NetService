#ifndef BASE_DB_H
#define BASE_DB_H

extern "C"
{
#include "../../sql/sqlite3.h"
};

#include <string>
#include <map>
#include "db_table.h"
#include "sql_format.h"
#include <mutex>

class BaseDb
{
public:
	BaseDb();
	//�������ݿ� url ���ݿ��ַ busy_time_out ÿ�β��������ݿ�æʱ�ȴ����� 
	int connect(const std::string &url, int busy_time_out = 1000);
	//�ر�
	int close();
	//ִ�� ���� ɾ�� ����
	int exec(const std::string &url);
	//ִ�� ��ѯ
	DBTable get(const std::string &url);
	//Ԥ���� δʵ��
//	int prepare();
private:
	//���������Ϣ
	int clear_err_msg();
private:
	std::mutex lock_;
	sqlite3 *db_ptr_;
	sqlite3_stmt*stmt_ptr_;
	char * err_msg_;
};

#endif //!BASE_DB_H