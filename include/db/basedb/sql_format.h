#ifndef SQL_FORMAT_H
#define SQL_FORMAT_H
#include <string>
#include <ctime>
#include <sstream>

#define FIND_CLASS_ATTR "."
//��ȡ������ �� A.B ���� B
std::string _class_attr(const std::string &var_name);

//replace
std::string replace(const std::string &src, const std::string &token, const std::string &rep);
//json_ecode
std::string encode_json_value(const std::string &src);
std::string decode_json_value(const std::string &src);
//����ת��Ϊ�ַ���
template<typename T>
std::string data_to_string(const T&in)
{
	std::ostringstream os;
	os << in;
	return os.str();
}

//�ַ���ƴ�� fill ���
std::string _sql_format(const std::string &fill);
template<typename T, typename ...Types>
std::string _sql_format(const std::string &fill, T first, Types&& ... args)
{
	return data_to_string(first) + fill + _sql_format(fill, std::forward<Types>(args)...);
}
//ȥ�������
template<typename ...Types>
std::string sql_format(const std::string &fill,Types&& ... args)
{
	std::string ret = _sql_format(fill, std::forward<Types>(args)...);
	return ret.substr(0,ret.size()-fill.size());
}
// 'x'
#define VALUE(x) "'"+data_to_string(x)+"'"
//('1','2')
#define VALUES(...) "('"+sql_format("','",##__VA_ARGS__)+"')"
//��ȡ������
#define VAR(x) #x
//ճ�ϵõ����� xy
#define GLUE(x,y) x##y

//��������
#define SQL_INSERT(table,...) "INSERT INTO "+table+" VALUES "+VALUES(##__VA_ARGS__)+";"
//�������� args ('a','b')
#define SQL_INSERT_ARGS(table,args,...) "INSERT INTO "+table+args+" VALUES "+VALUES(##__VA_ARGS__)+";"
//�� d=2 CONDITION(d,'=') չ�� d='2'
#define CONDITION(var,condition) _class_attr(VAR(var))+condition +VALUE(var)
//����
#define CONDITION_EQ(var) CONDITION(var,"=")
//���� OP OR AND
#define SQL_WHERE(x,y,op) sql_format(" ",x,op,y)
#define SQL_WHERE_AND(x,y) SQL_WHERE(x,y,"AND")
#define SQL_WHERE_OR(x,y) SQL_WHERE(x,y,"OR")
//���� x,y
#define SQL_SET(x,y) SQL_WHERE(x,y," , ")
//���� x,y,z,...
#define SQL_SETS(...) sql_format(",",##__VA_ARGS__)
//��������
#define SQL_UPDATA(table,set,_where) "UPDATE "+table+" SET "+set+" WHERE "+_where+";"
//�������� �� status!=0 ����
#define SQL_UPDATA_STATUS(table,set,_where) SQL_UPDATA(table,set,_where+" AND status!=0")
//ɾ������
#define SQL_DELETE(table,_where) "DELETE FROM "+table+" WHERE "+_where+";"
//��������Ϊ��Ч status=0
#define SQL_REMOVE(table,_where) SQL_UPDATA(table,"status=0",_where)
//��ѯ���
#define SQL_SELECT(table,_where) "SELECT * FROM "+table+" WHERE "+_where+";"
//��ѯ���� �ų���Ч����
#define SQL_SELECT_STATUS(table,_where) SQL_SELECT(table,_where+" AND status!=0")

//ʱ��
std::string time_to_string(time_t t=0,const std::string &fmt="%Y%m%d%H%M%S", const std::string &type="LOCAL");
//����ʱ���
#define LOCAL_TIME time(nullptr)
#endif

