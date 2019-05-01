#ifndef SERVER_SESSION_MGR_H
#define SERVER_SESSION_MGR_H
#include <string>
#include <atomic>
#include <vector>

#include "../basedb/base_db.h"
#include "../user/user_mgr.h"
#include "session.h"

class SessionMgr
{
public:
	int init(const std::string &db_path,int t_out=60*60*24*3);
	int login(SessionInfo &user);
	int logout(const std::string &code);
	int get_session(SessionInfo &session, const std::string &code);
	int create_visit(std::string & session_id);
	std::string get_username(const std::string &session_id, const std::string &notfind="");
	int get_user_permission(const std::string &code,int notfind =3);
	//� ����ʱ��
	int active(const std::string &code);
	//�鿴�Ự�Ƿ���� ��ʱ -1 ������ -2 ��ʧЧ -3 ��ʱ 0 ��Ծ
	int check_session(const std::string &code);
	
	//������ڻỰ
	//ɾ������
	int delete_sesssion();
	//����
	int clear();
	
	//������ȡ��Ϣ
	int get_datas(vec_session_info&sessions, const std::string&_where, const std::string&order = "", int beg = 0, int end = -1);

	~SessionMgr();
private:
	BaseDb db_;
	size_t time_out_;
	std::atomic_int code_p_;
private:
	int get_row(DBTable &table, SessionInfo&it, int row);
	int init_db_table();
	int get_unique_code(SessionInfo &user);
};


#endif