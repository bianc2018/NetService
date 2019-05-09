#ifndef SERVER_JSON_H
#define SERVER_JSON_H
extern "C"
{
	//导入cJson接口
#include "cJson.h"
};

#include <string>
#include <vector>
//文件读取BUFF 的大小
#define BUFF_SIZE 1024

class Json
{
public:
	//cJSON * 初始化 
	Json(cJSON *ptr=nullptr);
	//从 vector 初始化数组
	Json(const std::vector<cJSON *>& value);
	Json(const std::vector<Json>& value);
	//初始化为object
	Json(const int& value);
	Json(const size_t& value);
	Json(const double& value);
	Json(const std::string& value);
	Json(const char* value);
	//初始化为数组
	Json(const std::vector<int>& value);
	Json(const std::vector<double>& value);
	Json(const std::vector<std::string>& value);
	Json(const std::vector<const char *>& value);
	//拷贝复制
	Json(const Json & j);
	Json &operator=(Json & j);
	//初始化为object
	int init_object();
	//初始化 数组
	int init_array();
	//释放资源
	~Json();
	//通过文件初始化
	int from_file(const std::string&path);
	//将json，转化为字符串，存到文件中
	int to_file(const std::string&path)const;
	//通过字符串初始化
	int from_string(const std::string&json);
	//转化为字符串
	std::string to_string()const;
	//获得一个节点，path_为路径 ""为根节点 如 "a.b.c#2" 值获取 
	//根节点下键值为a的节点的子节点的键值为b的节点的子节点的键值为c的数组的下标为1（0开始）的object的节点
	cJSON* get_object(const std::string& path="");
	//获取数组节点，将数组的每一项转化为Json 返回集合,path_见 get_object
	std::vector<Json> get_array(const std::string& path = "");
	//返回根节点的所有子节点的键值
	std::vector<std::string> get_root_keys();
	//获取节点的值，path_见 get_object
	int get_value(int&value, const std::string& path = "");
	int get_value(size_t&value, const std::string& path = "");
	int get_value(double&value, const std::string& path = "");
	int get_value(std::string&value, const std::string& path = "");

	int get_int_value(const std::string& path = "",const int& notfind=0);
	double get_double_value(const std::string& path = "",const double& notfind=0);
	std::string get_string_value( const std::string& path = "", const std::string& notfind = "");

	//在path_ 下设置一个空的object
	int set_object(const std::string& key, const std::string& path = "");
	//在path_ 下设置一个空的array
	int set_array(const std::string& key, const std::string& path = "");
	//在path_ 下添加一个Json
	int add_object_value(const std::string& key, Json value, const std::string& path = "");
	//在path_ 下添加一个数组
	int add_array_value( std::vector<Json>value, const std::string& path = "");
	//在path_ 下数组添加一个Json
	int add_array_value(Json value, const std::string& path = "");
	
	size_t get_array_size(const std::string& path = "");

	//获取 json_root_
	cJSON *get_json_root();
	//转移 json_root_ 本身的json_root_将会置为空
	cJSON *move();
	Json& move(Json &json);
private:
	//释放json_root_
	int clear();
	//字符分割
	static std::vector<std::string> split(const std::string& s, const std::string&sub_split);
	//深拷贝
	int get_from_json(const Json& j);
private:
	cJSON *json_root_;
};

#endif