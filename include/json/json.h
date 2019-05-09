#ifndef SERVER_JSON_H
#define SERVER_JSON_H
extern "C"
{
	//����cJson�ӿ�
#include "cJson.h"
};

#include <string>
#include <vector>
//�ļ���ȡBUFF �Ĵ�С
#define BUFF_SIZE 1024

class Json
{
public:
	//cJSON * ��ʼ�� 
	Json(cJSON *ptr=nullptr);
	//�� vector ��ʼ������
	Json(const std::vector<cJSON *>& value);
	Json(const std::vector<Json>& value);
	//��ʼ��Ϊobject
	Json(const int& value);
	Json(const size_t& value);
	Json(const double& value);
	Json(const std::string& value);
	Json(const char* value);
	//��ʼ��Ϊ����
	Json(const std::vector<int>& value);
	Json(const std::vector<double>& value);
	Json(const std::vector<std::string>& value);
	Json(const std::vector<const char *>& value);
	//��������
	Json(const Json & j);
	Json &operator=(Json & j);
	//��ʼ��Ϊobject
	int init_object();
	//��ʼ�� ����
	int init_array();
	//�ͷ���Դ
	~Json();
	//ͨ���ļ���ʼ��
	int from_file(const std::string&path);
	//��json��ת��Ϊ�ַ������浽�ļ���
	int to_file(const std::string&path)const;
	//ͨ���ַ�����ʼ��
	int from_string(const std::string&json);
	//ת��Ϊ�ַ���
	std::string to_string()const;
	//���һ���ڵ㣬path_Ϊ·�� ""Ϊ���ڵ� �� "a.b.c#2" ֵ��ȡ 
	//���ڵ��¼�ֵΪa�Ľڵ���ӽڵ�ļ�ֵΪb�Ľڵ���ӽڵ�ļ�ֵΪc��������±�Ϊ1��0��ʼ����object�Ľڵ�
	cJSON* get_object(const std::string& path="");
	//��ȡ����ڵ㣬�������ÿһ��ת��ΪJson ���ؼ���,path_�� get_object
	std::vector<Json> get_array(const std::string& path = "");
	//���ظ��ڵ�������ӽڵ�ļ�ֵ
	std::vector<std::string> get_root_keys();
	//��ȡ�ڵ��ֵ��path_�� get_object
	int get_value(int&value, const std::string& path = "");
	int get_value(size_t&value, const std::string& path = "");
	int get_value(double&value, const std::string& path = "");
	int get_value(std::string&value, const std::string& path = "");

	int get_int_value(const std::string& path = "",const int& notfind=0);
	double get_double_value(const std::string& path = "",const double& notfind=0);
	std::string get_string_value( const std::string& path = "", const std::string& notfind = "");

	//��path_ ������һ���յ�object
	int set_object(const std::string& key, const std::string& path = "");
	//��path_ ������һ���յ�array
	int set_array(const std::string& key, const std::string& path = "");
	//��path_ �����һ��Json
	int add_object_value(const std::string& key, Json value, const std::string& path = "");
	//��path_ �����һ������
	int add_array_value( std::vector<Json>value, const std::string& path = "");
	//��path_ ���������һ��Json
	int add_array_value(Json value, const std::string& path = "");
	
	size_t get_array_size(const std::string& path = "");

	//��ȡ json_root_
	cJSON *get_json_root();
	//ת�� json_root_ �����json_root_������Ϊ��
	cJSON *move();
	Json& move(Json &json);
private:
	//�ͷ�json_root_
	int clear();
	//�ַ��ָ�
	static std::vector<std::string> split(const std::string& s, const std::string&sub_split);
	//���
	int get_from_json(const Json& j);
private:
	cJSON *json_root_;
};

#endif