#ifndef SERVER_NEWS_MGR_H
#define SERVER_NEWS_MGR_H
#include <string>
#include <vector>

#include "../basedb/base_db.h"

#include "new.h"

//新闻评论管理类
class NewsMgr
{
public:
	int init(const std::string &db_path);
	~NewsMgr();
	//创建
	int create(NewInfo &it);
	//保存
	int save(NewInfo &it);
	int save_info(NewInfo &it);
	int save_info(const std::string & code,const std::string &key, const std::string &value);
	//设置资源路径
	int set_data_path(const std::string & code,const std::string &value);
	const std::string get_data_path(const std::string & code, const std::string &nofind="");
	//获取一项信息
	std::string get_info(const std::string & code,const std::string &key,const std::string &nofind="");
	int save_content(const std::string &code,const std::string &content);
	//赞或踩 1 赞  -1 踩
	int like(const std::string &username, const std::string &new_id, int blike);
	int get_like_status(const std::string &username, const std::string &new_id);
	//获取数据 property!=-1 根据property获取数据
	int get_data(NewInfo &it, const std::string &code, int property=-1);
	//条件获取信息
	int get_datas(vec_new_info&news, const std::string&_where, const std::string&order="",int beg=0,int end=-1);
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
	int comment(NewInfo &it);
	//clear 清除无用数据 未保存的数据
	int clear();
private:
	BaseDb db_;
private:
	int get_row(DBTable &table,NewInfo&it, int row);
	int init_db_table();
	int get_unique_code(NewInfo &it);
};
#endif