#include "news_mgr.h"

#include "../../log/log.hpp"
#include <boost/filesystem.hpp>

int NewsMgr::init(const std::string & db_path)
{
	int ret = db_.connect(db_path);
	if (ret != SQLITE_OK)
	{
		LOG(LERROR, "打开数据失败 url= ", db_path);
		return ret;
	}

	//查询表项
	DBTable new_tb = db_.get("select name from sqlite_master where type='table' and name='" + NEW_TABLE_NAME + "';");
	if (new_tb.get_rows_num() <= 0)
		ret = init_db_table();

	return ret;
}

NewsMgr::~NewsMgr()
{
	clear();
	db_.close();
}

//INSERT INTO 表名称 VALUES (值1, 值2,....)
//INSERT INTO table_name (列1, 列2,...) VALUES (值1, 值2,....)
int NewsMgr::create(NewInfo & it)
{
	get_unique_code(it);
	
	it.status = 1;
	std::string sql = SQL_INSERT_ARGS(NEW_TABLE_NAME, \
		"('code','title','author','content','property','parent','status','comment_num','like_num','unlike_num','create_time','create_user','modify_time','modify_user')",\
		it.code, it.title, it.author, it.content, it.property, it.parent, it.status, it.comment_num, it.like_num, it.unlike_num,\
		it.create_time, it.create_user,it.modify_time, it.modify_user);
	LOG(LDEBUG, "create \n", sql);
	return db_.exec(sql);
	
}

int NewsMgr::save(NewInfo & it)
{
	//it.modify_time = LOCAL_TIME;
	it.status = 2;
	std::string sql = SQL_UPDATA_STATUS(NEW_TABLE_NAME, \
		SQL_SETS(\
			CONDITION_EQ(it.title),\
			CONDITION_EQ(it.author),\
			CONDITION_EQ(it.content),\
			CONDITION_EQ(it.modify_time), \
			CONDITION_EQ(it.modify_user), \
			CONDITION_EQ(it.status))\
		, CONDITION_EQ(it.code));

	LOG(LDEBUG, "sql\n", sql);

	return db_.exec(sql);
}

int NewsMgr::save_info(const std::string & code,const std::string & key, const std::string & value)
{
	std::string sql = SQL_UPDATA(NEW_TABLE_NAME, "'" + key + "'='" + value + "'", CONDITION_EQ(code));

	LOG(LDEBUG, "sql\n", sql);

	return db_.exec(sql);
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

	std::string sql = SQL_SELECT(NEW_TABLE_NAME, CONDITION_EQ(code));

	DBTable table = db_.get(sql);
	LOG(LDEBUG, table.get_rows_num(), " new  \n", sql);
	if (0 < table.get_rows_num())
	{
		auto ms = table.get_keys();
		for (auto m:ms)
		{
			if (m.second == key)
			{
				return table.get_cell(0, key);
			}
		}
	}
	return nofind;
}

int NewsMgr::save_info(NewInfo & it)
{
	it.modify_time = LOCAL_TIME;
	it.status = 2;
	std::string sql = SQL_UPDATA_STATUS(NEW_TABLE_NAME, \
		SQL_SETS(\
			CONDITION_EQ(it.title), \
			CONDITION_EQ(it.author), \
			CONDITION_EQ(it.modify_time), \
			CONDITION_EQ(it.modify_user), \
			CONDITION_EQ(it.status))\
		, CONDITION_EQ(it.code));

	LOG(LDEBUG, "sql\n", sql);

	return db_.exec(sql);
}

int NewsMgr::save_content(const std::string & code,const std::string & content)
{
	int modify_time = LOCAL_TIME;
	int status = 2;
	std::string modify_user = "";
	std::string sql = SQL_UPDATA_STATUS(NEW_TABLE_NAME, \
		SQL_SETS(\
			CONDITION_EQ(content), \
			CONDITION_EQ(modify_time), \
			CONDITION_EQ(modify_user), \
			CONDITION_EQ(status))\
		, CONDITION_EQ(code));

	LOG(LDEBUG, "sql\n", sql);

	return db_.exec(sql);
}

int NewsMgr::like(const std::string & username, const std::string & new_id, int status)
{
	//参数不对
	if (-1 != status && 1 != status)
		return -1;

	//1 已赞同 0 未操作 -1  已反对
	int _status = get_like_status(username, new_id);

	NewInfo now_new;
	get_data(now_new, new_id);
	std::string sql;

	//重复操作 如已赞同 继续点赞 则是取消点赞
	if (_status == status)
	{
		//删除操作
		sql = SQL_DELETE(NEW_AND_LIKE_TABLE_NAME, SQL_WHERE_AND(CONDITION_EQ(username), CONDITION_EQ(new_id)));

		//点赞或反对数目减少
		if (status == 1)
			--now_new.like_num;
		else if(status == -1)
			--now_new.unlike_num;
	}
	else
	{
		if (_status == 0)
			sql = SQL_INSERT(NEW_AND_LIKE_TABLE_NAME, new_id, username, status);
		else
		{
			sql = SQL_UPDATA(NEW_AND_LIKE_TABLE_NAME, CONDITION_EQ(status), SQL_WHERE_AND(CONDITION_EQ(username), CONDITION_EQ(new_id)));

			//取消相反的
			if (status == 1)
				--now_new.unlike_num;
			else if(status==-1)
				--now_new.like_num;
		}
		//增加
		if (status == 1)
			++now_new.like_num;
		else if(status == -1)
			++now_new.unlike_num;
	}

	int ret = db_.exec(sql);
	if (ret != 0)
		return -1;
	const std::string &code = new_id;
	ret = db_.exec(SQL_UPDATA(NEW_TABLE_NAME,SQL_SETS(CONDITION_EQ(now_new.like_num), CONDITION_EQ(now_new.unlike_num)),CONDITION_EQ(code)));
	if (ret != 0)
		return -2;
	return 0;
}

int NewsMgr::get_like_status(const std::string & username, const std::string & new_id)
{
	//查询操作
	std::string sql=SQL_SELECT(NEW_AND_LIKE_TABLE_NAME,SQL_WHERE_AND(CONDITION_EQ(new_id),CONDITION_EQ(username)));
	DBTable tb = db_.get(sql);
	//不存在，用户未进行操作
	if (tb.get_rows_num() <= 0)
		return 0;
	//否则status表示用户对新闻的态度
	int status=0;
	GET_CELL_INT0(tb, status);
	return status;
}

int NewsMgr::get_data(NewInfo & it, const std::string & code,int property)
{

	try
	{
		std::string sql;
		if(property==-1)
			sql = SQL_SELECT_STATUS(NEW_TABLE_NAME, CONDITION_EQ(code));
		else
			sql = SQL_SELECT_STATUS(NEW_TABLE_NAME, SQL_WHERE_AND(CONDITION_EQ(code),CONDITION_EQ(property)));
		
		DBTable table = db_.get(sql);
		LOG(LDEBUG, table.get_rows_num()," new  \n", sql);
		if (0 >= table.get_rows_num())
			return -1;
		else
		{
			get_row(table, it, 0);
			return 0;
		}
	}
	catch (...)
	{
		return -3;
	}
}

int NewsMgr::get_datas(vec_new_info & news, const std::string & _where, const std::string & order, int beg, int end)
{
	
	std::string sql;
	std::string other = "1=1 ";
	//条件
	if (_where != "")
		other += " AND " + _where;
	//排序
	if (NONE != order)
		other += " Order By " + order;

	//限制
	//if (limit > 0)
	//	other += " LIMIT " + std::to_string(limit);

	sql = SQL_SELECT(NEW_TABLE_NAME, other);
	DBTable table = db_.get(sql);
	int row_num = table.get_rows_num();
	LOG(LDEBUG, row_num, " new  \n", sql);
	if (0 > row_num)
		return -1;
	else
	{
		//是否越界
		if (end <= 0 || end>row_num)
			end = row_num;
		if (beg < 0||beg>end)
			beg = 0;

		for (int i = beg; i < end; i++)
		{
			NewInfo info;
			get_row(table, info, i);
			news.push_back(info);
		}
		return row_num;
	}
}

int NewsMgr::remove(const std::string & code)
{
	std::string parent = get_info(code, "parent");

	if (parent != "")
	{
		//评论数减少
		int comment_num =std::stoi(get_info(parent, "comment_num","0"));
		if (comment_num > 0)
		{
			--comment_num;
			if (save_info(parent, "comment_num", std::to_string(comment_num)) != 0)
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
	std::string sql = SQL_DELETE(NEW_TABLE_NAME, CONDITION_EQ(code));
	LOG(LDEBUG, "delete\n", sql);

	return db_.exec(sql);
}

int NewsMgr::delete_comment(const std::string & code)
{
	const std::string  parent = code;
	std::string sql = SQL_DELETE(NEW_TABLE_NAME,CONDITION_EQ(parent));
	LOG(LDEBUG, "sql \n", sql);
	return db_.exec(sql);
}

int NewsMgr::delete_like(const std::string & code)
{
	const std::string  new_id = code;
	std::string sql = SQL_DELETE(NEW_AND_LIKE_TABLE_NAME, CONDITION_EQ(new_id));
	LOG(LDEBUG, "sql \n", sql);
	return db_.exec(sql);
}

int NewsMgr::get_status(const std::string & code)
{

	std::string sql = SQL_SELECT_STATUS(NEW_TABLE_NAME, CONDITION_EQ(code));

	LOG(LDEBUG, "get status \n", sql);
	DBTable table = db_.get(sql);
	if (0 >= table.get_rows_num())
		return -1;
	else
	{
		std::string status = table.get_cell(0, "status");
		if ("" == status)
			return -2;
		return std::stoi(status);
	}
}

int NewsMgr::reset_status(const std::string & code, const int status)
{
	if (status < 0 || status>3)
		return -1;
	size_t publish_time = 0;
	if (status == 3)
	{
		publish_time = LOCAL_TIME;
		NewInfo n;
		get_data(n, code);
		//作者或标题为空
		if (n.title == "" || n.author == "")
			return -2;
	}
	
	std::string sql = SQL_UPDATA(NEW_TABLE_NAME,SQL_SETS(CONDITION_EQ(status),CONDITION_EQ(publish_time)), CONDITION_EQ(code));
	LOG(LDEBUG, "reset_status \n", sql);
	return db_.exec(sql);
}

int NewsMgr::comment( NewInfo & it)
{
	//std::string sql = SQL_UPDATA(NEW_TABLE_NAME)
	NewInfo info; 
	//创建
	if (create(it) != 0)
		return -1;
	//发布
	if (reset_status(it.code,3) != 0)
		return -2;
	//父节点 评论数+1
	if (get_data(info, it.parent) != 0)
		return -3;

	info.comment_num = info.comment_num + 1;
	std::string sql = SQL_UPDATA(NEW_TABLE_NAME,CONDITION_EQ(info.comment_num),CONDITION_EQ(info.code));

	return db_.exec(sql);
}


int NewsMgr::clear()
{
	//清理冗余数据
	int status=0;
	std::string sql = SQL_UPDATA(NEW_TABLE_NAME, CONDITION_EQ(status), "status=1");
	LOG(LDEBUG, "new \n", sql);
	return db_.exec(sql);
}

int NewsMgr::get_row(DBTable & table, NewInfo & it, int row)
{
	//获取一行数据
	try
	{
		GET_CELL(table, it.code, row);
		GET_CELL(table, it.author, row);
		GET_CELL(table, it.title, row);
		GET_CELL(table, it.content, row);
		GET_CELL(table, it.parent, row);
		GET_CELL(table, it.data_path, row);

		GET_CELL_INT(table, it.property, row);
		GET_CELL_INT(table, it.comment_num, row);
		GET_CELL_INT(table, it.like_num, row);
		GET_CELL_INT(table, it.unlike_num, row);
		GET_CELL_INT(table, it.property, row);
		GET_CELL_INT(table, it.comment_num, row);
		GET_CELL_INT(table, it.status, row);

		GET_CELL_INT(table, it.create_time, row);
		GET_CELL(table, it.create_user, row);
		GET_CELL_INT(table, it.modify_time, row);
		GET_CELL(table, it.modify_user, row);
		GET_CELL_INT(table, it.publish_time, row);
	}
	catch (...)
	{
		return -1;
	}
	return 0;
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
								property     TEXT,				\
								parent       TEXT DEFAULT \"\",	\
								status       TEXT NOT NULL		\
												  DEFAULT (0),	\
								comment_num  TEXT DEFAULT (0),	\
								like_num     TEXT DEFAULT (0)	\
												  NOT NULL,		\
								unlike_num   TEXT DEFAULT (0)	\
												  NOT NULL,		\
								data_path TEXT				,\
								create_time  TEXT,				\
								create_user  TEXT,				\
								modify_time  TEXT,				\
								modify_user  TEXT,				\
								publish_time TEXT				\
							);";
	int ret =  db_.exec(NEWS_TABLE);
	if (ret != 0)
		return -1;

	//点赞，
	const std::string NEW_LIKE_TABLE = "CREATE TABLE "+NEW_AND_LIKE_TABLE_NAME+" (					\
																				new_id         TEXT 	NOT NULL,		\
																				username        TEXT,				\
																				status          TEXT,\
																				PRIMARY KEY (new_id,username)\
																			);";
	return db_.exec(NEW_LIKE_TABLE);
}

int  NewsMgr::get_unique_code(NewInfo & it)
{
	std::hash<std::string> hash_string;
	int i = 0;
	while (true)
	{
		std::string code_str = sql_format("", it.create_user, it.title, LOCAL_TIME, i);
		//哈希值
		std::string i_code = std::to_string(hash_string(code_str));
		if (-1 == get_status(i_code))
		{
			it.code = i_code;
			return 0;
		}
		i++;
	}
	return 0;
}
