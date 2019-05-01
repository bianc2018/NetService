#ifndef SQL_FORMAT_H
#define SQL_FORMAT_H
#include <string>
#include <ctime>
#include <sstream>

#define FIND_CLASS_ATTR "."
//获取属性名 如 A.B 返回 B
std::string _class_attr(const std::string &var_name);

//replace
std::string replace(const std::string &src, const std::string &token, const std::string &rep);
//json_ecode
std::string encode_json_value(const std::string &src);
std::string decode_json_value(const std::string &src);
//数据转化为字符串
template<typename T>
std::string data_to_string(const T&in)
{
	std::ostringstream os;
	os << in;
	return os.str();
}

//字符串拼接 fill 填充
std::string _sql_format(const std::string &fill);
template<typename T, typename ...Types>
std::string _sql_format(const std::string &fill, T first, Types&& ... args)
{
	return data_to_string(first) + fill + _sql_format(fill, std::forward<Types>(args)...);
}
//去除多余的
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
//获取变量名
#define VAR(x) #x
//粘合得到变量 xy
#define GLUE(x,y) x##y

//插入数据
#define SQL_INSERT(table,...) "INSERT INTO "+table+" VALUES "+VALUES(##__VA_ARGS__)+";"
//插入数据 args ('a','b')
#define SQL_INSERT_ARGS(table,args,...) "INSERT INTO "+table+args+" VALUES "+VALUES(##__VA_ARGS__)+";"
//设 d=2 CONDITION(d,'=') 展开 d='2'
#define CONDITION(var,condition) _class_attr(VAR(var))+condition +VALUE(var)
//等于
#define CONDITION_EQ(var) CONDITION(var,"=")
//条件 OP OR AND
#define SQL_WHERE(x,y,op) sql_format(" ",x,op,y)
#define SQL_WHERE_AND(x,y) SQL_WHERE(x,y,"AND")
#define SQL_WHERE_OR(x,y) SQL_WHERE(x,y,"OR")
//集合 x,y
#define SQL_SET(x,y) SQL_WHERE(x,y," , ")
//集合 x,y,z,...
#define SQL_SETS(...) sql_format(",",##__VA_ARGS__)
//更新数据
#define SQL_UPDATA(table,set,_where) "UPDATE "+table+" SET "+set+" WHERE "+_where+";"
//更新数据 有 status!=0 限制
#define SQL_UPDATA_STATUS(table,set,_where) SQL_UPDATA(table,set,_where+" AND status!=0")
//删除数据
#define SQL_DELETE(table,_where) "DELETE FROM "+table+" WHERE "+_where+";"
//设置数据为无效 status=0
#define SQL_REMOVE(table,_where) SQL_UPDATA(table,"status=0",_where)
//查询语句
#define SQL_SELECT(table,_where) "SELECT * FROM "+table+" WHERE "+_where+";"
//查询数据 排除无效数据
#define SQL_SELECT_STATUS(table,_where) SQL_SELECT(table,_where+" AND status!=0")

//时间
std::string time_to_string(time_t t=0,const std::string &fmt="%Y%m%d%H%M%S", const std::string &type="LOCAL");
//本地时间戳
#define LOCAL_TIME time(nullptr)
#endif

