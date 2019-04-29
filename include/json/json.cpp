#include "json.h"

#include <fstream>

Json::Json(cJSON *ptr):json_root_(nullptr)
{
	//输入为空，创建一个object
	if (nullptr == ptr)
	{
		json_root_ =  cJSON_CreateObject();
		return;
	}
	//获取对应ptr 的字符串
	char *str = cJSON_Print(ptr);
	//获取失败
	if (str == nullptr)
	{
		json_root_ = cJSON_CreateObject();
		return;
	}
	std::string result(str);
	//释放资源
	cJSON_free(str);
	//字符串转化为json
	from_string(result);
}

Json::Json(const int & value)
{
	json_root_ = cJSON_CreateNumber(value);
}

Json::Json(const size_t & value)
{
	json_root_ = cJSON_CreateNumber(value);
}

Json::Json(const double & value)
{
	json_root_ = cJSON_CreateNumber(value);
}

Json::Json(const std::string & value)
{
	json_root_ = cJSON_CreateString(value.c_str());
}

Json::Json(const char * value)
{
	json_root_ = cJSON_CreateString(value);
}
Json::Json(const std::vector<int>& value)
{
	json_root_ = cJSON_CreateArray();
	for (auto v : value)
	{
		cJSON_AddItemToArray(json_root_, cJSON_CreateNumber(v));
	}
}

Json::Json(const std::vector<double>& value)
{
	json_root_ = cJSON_CreateArray();
	for (auto v : value)
	{
		cJSON_AddItemToArray(json_root_, cJSON_CreateNumber(v));
	}
}

Json::Json(const std::vector<std::string>& value)
{
	json_root_ = cJSON_CreateArray();
	for (auto v : value)
	{
		cJSON_AddItemToArray(json_root_, cJSON_CreateString(v.c_str()));
	}
}

Json::Json(const std::vector<const char *>& value)
{
	json_root_ = cJSON_CreateArray();
	for (auto v : value)
	{
		cJSON_AddItemToArray(json_root_, cJSON_CreateString(v));
	}
}

Json::Json(const std::vector<cJSON *>& value)
{
	json_root_ = cJSON_CreateArray();
	for (auto v : value)
	{
		cJSON_AddItemToArray(json_root_, v);
	}
}

Json::Json(const std::vector<Json>& value)
{
	json_root_ = cJSON_CreateArray();
	for (auto v : value)
	{
		cJSON_AddItemToArray(json_root_, v.move());
	}
}

Json::Json(const Json & j)
{
	json_root_ = nullptr;
	get_from_json(j);
}

Json & Json::operator=(Json & j)
{
	get_from_json(j);

	return *this;
}

int Json::init_array()
{
	clear();
	json_root_ = cJSON_CreateArray();
	return 0;
}

int Json::init_object()
{
	clear();
	json_root_ = cJSON_CreateObject();
	return 0;
}

Json::~Json()
{
	clear();
}

int Json::from_file(const std::string & path)
{
	std::fstream json_file(path, std::ios::in);
	if (!json_file)
		return -1;

	std::string json;
	std::unique_ptr<char[]> buff(new char[BUFF_SIZE]);

	while (true)
	{
		json_file.read(buff.get(), BUFF_SIZE);
		int len = json_file.gcount();
		if (len == 0)
		{
			return from_string(json);
		}
		else if (len < 0)
			return -2;

		json += std::string(buff.get(), len);
	}

	return 0;
}

int Json::to_file(const std::string & path) const
{
	std::fstream json_file(path, std::ios::out);
	if (!json_file)
		return -1;

	std::string json = to_string();
	int pos = 0;
	int len = json.size();

	while (true)
	{
		json_file.write(json.c_str()+pos,len);
		int write_len = json_file.gcount();

		if (write_len == 0)
			return -2;
		else if (write_len < 0)
			return -3;
		else if (write_len >= len)
			return 0;

		pos += write_len;
		len -= write_len;
	}

	return 0;
}

int Json::from_string(const std::string & json)
{
	clear();
	json_root_ = cJSON_Parse(json.c_str());
	if (nullptr == json_root_)
		return -1;
	return 0;
}

std::string Json::to_string()const
{
	if (nullptr == json_root_)
		return std::string();
	char *str = cJSON_Print(json_root_);
	std::string result(str);
	cJSON_free(str);
	return result;
}

std::vector<std::string> Json::split(const std::string & s, const std::string & sub_split)
{
	std::vector<std::string> ret;
	size_t pos = 0;
	std::string str = s + sub_split;
	while (true)
	{
		auto split_pos = str.find(sub_split, pos);
		if (std::string::npos == split_pos)
			break;
		ret.push_back(str.substr(pos, split_pos - pos));
		pos = split_pos + sub_split.size();
	}
	return ret;
}

int Json::get_from_json(const Json & j)
{
	//先转化为字符串，再由字符串转化为cJson*
	from_string(j.to_string());
	return 0;
}
//...#
cJSON* Json::get_object(const std::string & path)
{
	if (path == "" or path == ".")
		return json_root_;
	//分割
	std::vector<std::string>names =  split(path, ".");
	//逐层访问
	cJSON *ptr = json_root_;
	for (auto name : names)
	{
		if (ptr == nullptr)
			break;
		//分割
		std::vector<std::string>sub_array = split(name, "#");
		//非数组
		if (1 == sub_array.size())
		{
			ptr = cJSON_GetObjectItem(ptr, name.c_str());
		}//数组
		else if (2 == sub_array.size())
		{
			ptr = cJSON_GetObjectItem(ptr, sub_array[0].c_str());
			if (nullptr == ptr)
				break;
			//获取数组项
			ptr = cJSON_GetArrayItem(ptr, std::stoi(sub_array[1]));
		}
	}
	//返回
	return ptr;
}

std::vector<Json> Json::get_array(const std::string & path)
{
	std::vector<Json> result;
	auto sub = get_object(path);
	if (nullptr == sub)
		return result;
	if(cJSON_Array != sub->type)
		return result;
	int len = cJSON_GetArraySize(sub);

	for (int i = 0; i < len; i++)
		result.push_back(Json(cJSON_GetArrayItem(sub, i)));

	return result;
}

std::vector<std::string> Json::get_root_keys()
{
	cJSON *ptr = json_root_->child;
	std::vector<std::string> keys;
	while (ptr != nullptr)
	{
		char *key = ptr->string;
		if (key != nullptr)
		{
			keys.push_back(key);
		}
		ptr = ptr->next;
	}
	return keys;
}

int Json::get_value(int & value, const std::string & path)
{
	auto sub = get_object(path);
	if (nullptr == sub)
		return -1;
	if (cJSON_Number == sub->type)
	{
		value = sub->valueint;
		return 0;
	}
	return -2;
}

int Json::get_value(size_t & value, const std::string & path)
{
	int i = 0;
	int ret = get_value(i, path);
	value = static_cast<size_t>(i);
	return ret;
}

int Json::get_value(double & value, const std::string & path)
{
	auto sub = get_object(path);
	if (nullptr == sub)
		return -1;
	if (cJSON_Number == sub->type)
	{
		value = sub->valuedouble;
		return 0;
	}
	return -2;
}

int Json::get_value(std::string & value, const std::string & path)
{
	auto sub = get_object(path);
	if (nullptr == sub)
		return -1;
	//type==cJSON_String  and type == cJSON_Raw
	if (cJSON_String == sub->type || cJSON_Raw == sub->type)
	{
		value = std::string(sub->valuestring);
		return 0;
	}
	return -2;
}

int Json::get_int_value(const std::string & path, const int & notfind)
{
	int value = notfind;
	int ret = get_value(value, path);
	return value;
}

double Json::get_double_value(const std::string & path, const double & notfind)
{
	double value = notfind;
	int ret = get_value(value, path);
	return value;
}

std::string Json::get_string_value(const std::string & path, const std::string & notfind)
{
	std::string  value = notfind;
	int ret = get_value(value, path);
	return value;
}

int Json::set_object(const std::string & key, const std::string & path)
{
	cJSON *object = cJSON_CreateObject();
	return add_object_value(key,object,path);
}

int Json::set_array(const std::string & key, const std::string & path)
{
	cJSON *object = cJSON_CreateArray();
	return add_object_value(key, object, path);
}

int Json::add_object_value(const std::string & key, Json  value, const std::string & path)
{
	auto sub = get_object(path);
	if (nullptr == sub)
		return -1;
	if (cJSON_Object != sub->type)
		return -1;
	cJSON_AddItemToObject(sub, key.c_str(), value.move());
	return 0;
}

int Json::add_array_value(std::vector<Json> value, const std::string & path)
{
	auto sub = get_object(path);
	if (nullptr == sub)
		return -1;
	if (cJSON_Array != sub->type)
		return -2;
	for (auto v : value)
		cJSON_AddItemToArray(sub, v.move());
	return 0;
}

int Json::add_array_value( Json value, const std::string & path)
{
	auto sub = get_object(path);
	if (nullptr == sub)
		return -1;
	if (cJSON_Array != sub->type)
		return -2;
	cJSON_AddItemToArray(sub, value.move());
	return 0;
}


cJSON * Json::get_json_root()
{
	return json_root_;
}

cJSON * Json::move()
{
	cJSON * p = json_root_;
	json_root_ = nullptr;
	return p;
}

int Json::clear()
{
	if (json_root_ != nullptr )
	{

		cJSON_Delete(json_root_);
		json_root_ = nullptr;
	}
	return 0;
}
