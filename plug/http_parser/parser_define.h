#ifndef PARSER_DEFINE_H
#define PARSER_DEFINE_H

#include <ctime>

#include <map>
#include <vector>
#include <string>

//url地址与参数之间的分隔符
const std::string SPLIT_URI_PARAMS("?");
//参数与参数之间的分隔符
const std::string SPLIT_PARAMS("&");
//键值之间的分隔符
const std::string SPLIT_KEY_VALUE("=");

const std::string SET_COOKIE("Set-Cookie");

typedef std::vector<std::string> split_vector;
split_vector split(const std::string & s, const std::string & sub_split);

//时间
std::string time_to_string(time_t t = 0, const std::string &fmt = "%Y%m%d%H%M%S", const std::string &type = "LOCAL");
//本地时间戳
#define LOCAL_TIME time(nullptr)

#endif