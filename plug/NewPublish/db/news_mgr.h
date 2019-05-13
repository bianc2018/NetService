#ifndef SERVER_NEWS_MGR_H
#define SERVER_NEWS_MGR_H
#include <string>
#include <vector>

#include "db_mgr.h"
const std::string NEW_TABLE_NAME("news");
const std::string NEW_AND_LIKE_TABLE_NAME("new_like");

//新闻评论管理类
class NewsMgr
{
public:
	int init(const std::string &db_path);
	~NewsMgr();
	//创建
	int create(Json &it);
	//保存
	int save(Json &it);
	int save_info(Json &it);
	int save_info(const std::string & code,const std::string &key, const std::string &value);
	int save_info(const std::string & code, const std::string &key,const int &value);
	//设置资源路径
	int set_data_path(const std::string & code,const std::string &value);
	const std::string get_data_path(const std::string & code, const std::string &nofind="");
	//获取一项信息
	std::string get_info(const std::string & code,const std::string &key,const std::string &nofind);
	int get_info(const std::string & code, const std::string &key, const int &nofind);
	int save_content(const std::string &code,const std::string &content);
	//赞或踩 1 赞  -1 踩
	int like(const std::string &username, const std::string &new_id, int blike);
	int get_like_status(const std::string &username, const std::string &new_id);
	//获取数据 property!=-1 根据property获取数据
	int get_data(Json &it, const std::string &code, int property=-1);
	//条件获取信息
	int get_datas(Json&news, const std::string&_where, const std::string&order="",int beg=0,int len=-1,size_t *p_all = nullptr, int transfer_content=0);
	size_t get_count(const std::string&_where);
	size_t get_count();
	//移除  不显示 内容保留
	int remove(const std::string &code);
	//删除 删除数据
	int delete_it(const std::string &code);
	//删除 评论
	int delete_comment(const std::string &code);
	//删除 点赞
	int delete_like(const std::string &code);
	//查询状态 -1 不存在 其他见NewMgr
	int get_status(const std::string &code);
	//重置状态
	int reset_status(const std::string &code, const int status);
	//评论
	int comment(Json &it);
	//clear 清除无用数据 未保存的数据
	int clear();
	//公共部分
	int delete_data_by_code(const std::string & code);
	int delete_data_by_status(int status);
	
	Json get_data(const std::string & code);
private:
	DbMgr mgr_;
private:
	int init_db_table();
};
#endif