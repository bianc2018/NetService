#ifndef PARSER_DEFINE_H
#define PARSER_DEFINE_H

#include <ctime>

#include <map>
#include <vector>
#include <string>

//url��ַ�����֮��ķָ���
const std::string SPLIT_URI_PARAMS("?");
//���������֮��ķָ���
const std::string SPLIT_PARAMS("&");
//��ֵ֮��ķָ���
const std::string SPLIT_KEY_VALUE("=");

const std::string SET_COOKIE("Set-Cookie");

typedef std::vector<std::string> split_vector;
split_vector split(const std::string & s, const std::string & sub_split);

//ʱ��
std::string time_to_string(time_t t = 0, const std::string &fmt = "%Y%m%d%H%M%S", const std::string &type = "LOCAL");
//����ʱ���
#define LOCAL_TIME time(nullptr)

#endif