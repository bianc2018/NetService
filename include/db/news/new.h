#ifndef SERVER_NEW_H
#define SERVER_NEW_H
#include <string>
#include <vector>

#include "../basedb/base_db.h"
#include "../../json/json.h"
//状态
constexpr auto OK = 0;
//不存在
constexpr auto EXIST = -1;
//未知错误
constexpr auto UNKNOW = -90;

//新闻或评论
struct NewInfo
{
	NewInfo();
	//唯一编码
	std::string code;
	//标题
	std::string title;
	//作者
	std::string author;
	//内容
	std::string content;
	
	//属性 0 新闻 1 评论
	int property;
	//父族 如果该项是一个新闻则 为"" 否则 这项表示其评论的新闻或评论
	std::string parent;
	//状态 0 删除 1 创建 2 保存 3 发布
	int status;
	//评论数 、点赞数 、反对数
	int comment_num;
	int like_num;
	int unlike_num;
	std::string data_path;
	//创建时间 创建人 修改人 修改时间 发布时间 201901052214
	size_t create_time;
	std::string create_user;
	size_t modify_time;
	std::string modify_user;
	size_t publish_time;
	
	int from_json(Json &json);
	Json to_json();
};


typedef std::vector<NewInfo> vec_new_info;
//空字符串
const std::string NONE("");
//新闻表名
const std::string NEW_TABLE_NAME("news");
const std::string NEW_AND_LIKE_TABLE_NAME("new_like");
#endif