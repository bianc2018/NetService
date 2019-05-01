#ifndef PARSER_DEFINE_H
#define PARSER_DEFINE_H
#include <map>
#include <vector>
#include <string>

//url地址与参数之间的分隔符
const std::string SPLIT_URI_PARAMS("?");
//参数与参数之间的分隔符
const std::string SPLIT_PARAMS("&");
//键值之间的分隔符
const std::string SPLIT_KEY_VALUE("=");

typedef std::vector<std::string> split_vector;
split_vector split(const std::string & s, const std::string & sub_split);
#endif