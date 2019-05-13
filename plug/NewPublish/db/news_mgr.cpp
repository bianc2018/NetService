#include "news_mgr.h"

#include "log/log.hpp"
#include <boost/filesystem.hpp>

int NewsMgr::init(const std::string & db_path)
{
	int ret = mgr_.db_init(db_path);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "打开数据失败 url= ", db_path);
		return ret;
	}
	mgr_.set_synchronous(1);
	//查询表项
	//DBTable new_tb = db_.get("select name from sqlite_master where type='table' and name='" + NEW_TABLE_NAME + "';");
	if (false == mgr_.has_table(NEW_TABLE_NAME))
		ret = init_db_table();

	return ret;
}

NewsMgr::~NewsMgr()
{
	clear();
	
}

//INSERT INTO 表名称 VALUES (值1, 值2,....)
//INSERT INTO table_name (列1, 列2,...) VALUES (值1, 值2,....)
int NewsMgr::create(Json & it)
{
	static db::DbStmt stmt =\
		mgr_.new_stmt("INSERT INTO "+ NEW_TABLE_NAME +\
		" ('code','title','author','content','property','parent','status','comment_num','like_num','unlike_num','create_time','create_user','modify_time','modify_user') "+\
			"VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
	auto code = mgr_.get_unique_code();
	if (0 == stmt.bind_exec(code, it.get_string_value("title"), it.get_string_value("author"), it.get_string_value("content"), \
		it.get_int_value("property"), it.get_string_value("parent"), 1,0, \
		0, 0, it.get_int_value("create_time"), it.get_string_value("create_user"), \
		it.get_int_value("modify_time"), it.get_string_value("modify_user")))
	{
		it.add_object_value("code", code);
		return 0;
	}
	return -1;
}

int NewsMgr::save(Json & it)
{
	static db::DbStmt stmt = \
		mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + \
			" SET title=?,author=?,content=?,modify_time=?,modify_user=?,status=? WHERE code=?; ");

	return stmt.bind_exec(it.get_string_value("title"), it.get_string_value("author"), it.get_string_value("content"), \
			it.get_int_value("modify_time"), it.get_string_value("modify_user"), 2, it.get_string_value("code"));
	
}

int NewsMgr::save_info(const std::string & code,const std::string & key, const std::string & value)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET "+key+"=? WHERE code=?; ");

	return stmt.bind_exec( value, code);
}

int NewsMgr::save_info(const std::string & code, const std::string & key, const int & value)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET " + key + "=? WHERE code=?; ");

	return stmt.bind_exec(value, code);
}

int NewsMgr::set_data_path(const std::string & code, const std::string & value)
{
	return save_info(code,"data_path",value);
}

const std::string NewsMgr::get_data_path(const std::string & code, const std::string & nofind)
{
	return get_info(code,"data_path",nofind);
}

std::string NewsMgr::get_info(const std::string & code, const std::string & key, const std::string & nofind)
{
	Json json = get_data(code);
	return json.get_string_value(key, nofind);
}

int NewsMgr::get_info(const std::string & code, const std::string & key, const int & nofind)
{
	Json json = get_data(code);
	return json.get_int_value(key,nofind);
}

int NewsMgr::save_info(Json & it)
{
	static db::DbStmt stmt = \
		mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET title=?,author=?,modify_time=?,modify_user=?,status=2 WHERE code=?; ");

	return stmt.bind_exec(it.get_string_value("title"), it.get_string_value("author"),time(nullptr), it.get_string_value("modify_user"), it.get_string_value("code"));

}

int NewsMgr::save_content(const std::string & code,const std::string & content)
{
	static db::DbStmt stmt = \
		mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + \
			" SET content=?,status=2 WHERE code=?; ");

	return stmt.bind_exec(content,code);
}

int NewsMgr::like(const std::string & username, const std::string & new_id, int status)
{
	static db::DbStmt insert_stmt = mgr_.new_stmt("INSERT INTO "+ NEW_AND_LIKE_TABLE_NAME +"('username','new_id','status')  VALUES(?,?,?)");
	static db::DbStmt delete_stmt = mgr_.new_stmt("DELETE FROM " + NEW_AND_LIKE_TABLE_NAME + "  WHERE username=? AND new_id=?;");
	static db::DbStmt update_stmt = mgr_.new_stmt("UPDATE " + NEW_AND_LIKE_TABLE_NAME + " SET status=? WHERE username=? AND new_id=?;");
	static db::DbStmt update_like_stmt = mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET like_num=?,unlike_num=? WHERE code=?;");
	//参数不对
	if (-1 != status && 1 != status)
		return -1;

	//1 已赞同 0 未操作 -1  已反对
	int _status = get_like_status(username, new_id);

	Json now_new;
	get_data(now_new, new_id);
	int like_num = now_new.get_int_value("like_num", 0);
	int unlike_num = now_new.get_int_value("unlike_num", 0);
	std::string sql;

	//重复操作 如已赞同 继续点赞 则是取消点赞
	if (_status == status)
	{
		//删除操作
		//sql = SQL_DELETE(NEW_AND_LIKE_TABLE_NAME, SQL_WHERE_AND(CONDITION_EQ(username), CONDITION_EQ(new_id)));
		
		auto ret = delete_stmt.bind_exec(username, new_id);
		if (ret != 0)
			return -2;
		//点赞或反对数目减少
		if (status == 1)
			--like_num;
		else if(status == -1)
			--unlike_num;
	}
	else
	{
		if (_status == 0)
		{
			//sql = SQL_INSERT(NEW_AND_LIKE_TABLE_NAME, new_id, username, status);
			
			auto ret = insert_stmt.bind_exec(username, new_id, status);
			if (ret != 0)
				return -2;
		}
		else
		{
			//sql = SQL_UPDATA(NEW_AND_LIKE_TABLE_NAME, CONDITION_EQ(status), SQL_WHERE_AND(CONDITION_EQ(username), CONDITION_EQ(new_id)));
			
			auto ret = update_stmt.bind_exec(status, username, new_id);
			if (ret != 0)
				return -2;
			//取消相反的
			if (status == 1)
				--unlike_num;
			else if(status==-1)
				--like_num;
		}
		//增加
		if (status == 1)
			++like_num;
		else if(status == -1)
			++unlike_num;
	}

	auto ret = update_like_stmt.bind_exec(like_num, unlike_num, new_id);
	if (ret != 0)
		return -2;

	return 0;
}

int NewsMgr::get_like_status(const std::string & username, const std::string & new_id)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT status FROM "+ NEW_AND_LIKE_TABLE_NAME +" WHERE username=? AND new_id=?;");
	Json json;
	stmt.bind_get_row(json,username, new_id);
	return json.get_int_value("status");
}

int NewsMgr::get_data(Json & it, const std::string & code,int property)
{
	static db::DbStmt stmt1 = mgr_.new_stmt("SELECT * FROM " + NEW_TABLE_NAME + " WHERE code = ? AND status!=0;");
	static db::DbStmt stmt2 = mgr_.new_stmt("SELECT * FROM " + NEW_TABLE_NAME + " WHERE code = ? AND property=? AND status!=0;");
	try
	{
		std::string sql;
		if (property == -1)
		{
			return stmt1.bind_get_row(it,code);
		}
		else
		{
			return stmt2.bind_get_row(it, code, property);
		}
	
	}
	catch (...)
	{
		return -3;
	}
}



int NewsMgr::get_datas(Json & news, const std::string & _where, const std::string & order, int beg, int len, size_t * p_all, int transfer_content)
{
	if (transfer_content == 1)
		return mgr_.get_datas(news, NEW_TABLE_NAME, _where, order, beg, len, p_all);

	std::string str = _where;
	if (_where == "")
		str = "1=1";
	
	db::DbStmt stmt = mgr_.new_stmt(\
		"SELECT code,title,author,property,parent,status,comment_num,like_num,unlike_num,create_time,create_user,modify_time,modify_user,publish_time FROM " + NEW_TABLE_NAME + " WHERE " + str + "  ORDER BY " + order + " limit ?,?;");
	if (p_all != nullptr)
	{
		db::DbStmt stmt_count = mgr_.new_stmt("SELECT * FROM " + NEW_TABLE_NAME + " WHERE   " + str + ";");
		Json d;
		*p_all = stmt_count.bind_get_rows(d);
	}

	return stmt.bind_get_rows(news, beg, len);
}

size_t NewsMgr::get_count(const std::string & _where)
{
	db::DbStmt stmt = mgr_.new_stmt\
		("SELECT * FROM " + NEW_TABLE_NAME + \
			" WHERE  1=1 AND " + _where + ";");

	//stmt.bind(_where, order, beg, len);
	Json news;
	return stmt.bind_get_rows(news);
}

size_t NewsMgr::get_count()
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM " + NEW_TABLE_NAME + " ;");
	Json json;
	
	return stmt.bind_get_rows(json);
}

int NewsMgr::remove(const std::string & code)
{
	std::string parent = get_info(code, "parent","");

	if (parent != "")
	{
		//评论数减少
		auto comment_num = get_info(parent, "comment_num", 0);
		LOG(LDEBUG, "comment_num = ",comment_num);
		if (comment_num > 0)
		{
			--comment_num;
			if (save_info(parent, "comment_num", comment_num) != 0)
				return -1;
		}
	}
	//设置status=0
	return reset_status(code, 0);
}
//DELETE FROM Person WHERE LastName =
int NewsMgr::delete_it(const std::string & code)
{
	std::string path = get_data_path(code);
	if (path != "")
	{
		//数据存在
		if (boost::filesystem::exists(path))
		{
			//删除数据
			auto ret = boost::filesystem::remove_all(path);
			LOG(LDEBUG, "delete 数据\n", ret);
		}
	}
	//删除评论
	delete_comment(code);
	//删除点赞
	delete_like(code);
	//删除新闻本体
	//std::string sql = SQL_DELETE(NEW_TABLE_NAME, CONDITION_EQ(code));
	//LOG(LDEBUG, "delete\n", sql);
	return delete_data_by_code( code);
}

int NewsMgr::delete_comment(const std::string & code)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + NEW_TABLE_NAME + "  WHERE parent=?;");
	return stmt.bind_exec(code);
}

int NewsMgr::delete_like(const std::string & code)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM " + NEW_AND_LIKE_TABLE_NAME + "  WHERE new_id=?;");
	return stmt.bind_exec(code);
}

int NewsMgr::reset_status(const std::string & code, const int status)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET status=?,publish_time=? WHERE code=?;");
	if (status < 0 || status>3)
		return -1;
	size_t publish_time = 0;
	if (status == 3)
	{
		publish_time = time(nullptr);
		Json n;
		get_data(n, code);
		//作者或标题为空
		if (n.get_string_value("title") == "" || n.get_string_value("author") == "")
			return -2;
	}
	return stmt.bind_exec(status, publish_time, code);
}

int NewsMgr::comment( Json & it)
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET comment_num=? WHERE code=?;");
	//std::string sql = SQL_UPDATA(NEW_TABLE_NAME)
	Json info; 
	//创建
	if (create(it) != 0)
		return -1;
	//发布
	if (reset_status(it.get_string_value("code"),3) != 0)
		return -2;

	//父节点 评论数+1
	if (get_data(info, it.get_string_value("parent")) != 0)
		return -3;

	int comment_num = info.get_int_value("comment_num")+1;
	return stmt.bind_exec(comment_num, it.get_string_value("parent"));

}


int NewsMgr::clear()
{
	static db::DbStmt stmt = mgr_.new_stmt("UPDATE " + NEW_TABLE_NAME + " SET status=0 WHERE status=1;");
	//清理冗余数据
	return stmt.bind_exec();
}

int NewsMgr::delete_data_by_code(const std::string & value)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM "+NEW_TABLE_NAME+"  WHERE code=?;");
	return stmt.bind_exec(value);
}

int NewsMgr::delete_data_by_status(int status)
{
	static db::DbStmt stmt = mgr_.new_stmt("DELETE FROM "+NEW_TABLE_NAME+"  WHERE status=?;");
	return stmt.bind_exec(status);
}

int NewsMgr::get_status(const std::string & value)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT status FROM "+NEW_TABLE_NAME+" WHERE code=?;");
	Json json;
	stmt.bind_get_row(json,value);
	return json.get_int_value("status");
}

Json NewsMgr::get_data(const std::string & value)
{
	static db::DbStmt stmt = mgr_.new_stmt("SELECT * FROM "+NEW_TABLE_NAME+" WHERE code=?;");
	Json json;
	stmt.bind_get_row(json,value);
	LOG(LDEBUG, "get \n", json.to_string());
	return json;
}


int NewsMgr::init_db_table()
{
	//创建数据表
	const std::string NEWS_TABLE = "CREATE TABLE news (					\
								code         TEXT PRIMARY KEY	\
												  UNIQUE		\
												  NOT NULL,		\
								title        TEXT,				\
								author       TEXT,				\
								content      TEXT NOT NULL,		\
								property     Byte,				\
								parent       TEXT DEFAULT \"\",	\
								status       Byte NOT NULL		\
												  DEFAULT (0),	\
								comment_num  Integer DEFAULT (0),	\
								like_num     Integer DEFAULT (0)	\
												  NOT NULL,		\
								unlike_num   Integer DEFAULT (0)	\
												  NOT NULL,		\
								data_path TEXT				,\
								create_time  Integer	DEFAULT (0),			\
								create_user  TEXT,				\
								modify_time  Integer	DEFAULT (0),			\
								modify_user  TEXT,				\
								publish_time Integer DEFAULT (0)				\
							);";
	int ret =  mgr_.exec(NEWS_TABLE);
	if (ret != 0)
		return -1;

	//点赞，
	const std::string NEW_LIKE_TABLE = "CREATE TABLE "+NEW_AND_LIKE_TABLE_NAME+" (					\
																				new_id         TEXT 	NOT NULL,		\
																				username        TEXT,				\
																				status          Integer DEFAULT (0),\
																				PRIMARY KEY (new_id,username)\
																			);";
	return mgr_.exec(NEW_LIKE_TABLE);
}

