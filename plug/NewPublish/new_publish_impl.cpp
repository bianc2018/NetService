#include "new_publish_impl.h"

#include "ini/ini.h"

using namespace net_service::http;
std::string replace(const std::string &src, const std::string &token, const std::string &rep)
{
	std::string dst;
	size_t pos = 0;
	do
	{
		auto token_pos = src.find(token, pos);
		dst += src.substr(pos, token_pos - pos);

		if (std::string::npos == token_pos)
			break;

		dst += rep;
		pos = token_pos + token.size();

	} while (pos < src.size());

	return dst;
}

NewPublishImpl & NewPublishImpl::instance()
{
	static NewPublishImpl impl;
	return impl;
}

int NewPublishImpl::deal_get_data(HTTP_HANDLE handle)
{
	return 0;
}

int NewPublishImpl::deal_get_datas(HTTP_HANDLE handle)
{
	LOG(LINFO, "deal a /get_datas_action");

	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");

	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	//获取参数 new user session
	std::string type = json.get_string_value("type", "");
	std::string _where = json.get_string_value("where", "");
	int size = json.get_int_value("size", 0); // 数目
	int page = json.get_int_value("page", 0); //页数 1-
	std::string orderby = json.get_string_value("orderby", "publish_time DESC");//"publish_time DESC" 排序;

	int byuser = json.get_int_value("byuser", 0);
	int transfer_content = json.get_int_value("transfer_content", 0); //是否传递内容
	//获取区间 [(page-1)*size,page*size)
	//是否只传递个人新闻，当前用户创建的新闻
	
	//获取区间 [(page-1)*size,page*size)
	//size 不可小于等于0
	if (size <= 0)
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO);
		write_json(handle, result);
		return WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO;
	}
	//开头
	int beg = 0;
	if (page > 0)
		beg = (page - 1)*size;
	//结尾
	int len =  size; //<=0 表示获取全部

	if (byuser == 1)
	{
		std::string username = session_mgr_.get_username(session_id);
		int perm = session_mgr_.get_user_permission(session_id, 3);
		if (perm != 0)
		{
			_where += "AND create_user='" + username + "'";
		}
	}
	//获取数据
	if ("new" == type)
	{
		get_news(result, _where, orderby, beg, len, transfer_content);
	}
	else if ("session" == type)
	{
		get_sessions(result, _where, orderby, beg, len);
	}
	else if ("user" == type)
	{
		get_users(result, _where, orderby, beg, len);
	}
	else
	{
		LOG(LERROR, "not find this data type,", type);
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT);
	}
	
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_get_news(HTTP_HANDLE handle)
{
	LOG(LINFO, "deal a /get_news_action");
	Json news;
	Json json, result;
	std::string session_id = get_cookie(handle,"session_id","");

	auto ret = read_json(handle,json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	//获取参数
	std::string _where = json.get_string_value("where", "");
	int size = json.get_int_value("size", 0); // 数目
	int page = json.get_int_value("page", 0); //页数 1-
	int byuser = json.get_int_value("byuser", 0);
	std::string orderby = json.get_string_value("orderby", "publish_time DESC");//"publish_time DESC" 排序;
	int transfer_content = json.get_int_value("transfer_content", 0); //是否传递内容
	//获取区间 [(page-1)*size,page*size)

	//是否只传递个人新闻，当前用户创建的新闻
	if (byuser == 1)
	{
		std::string username = session_mgr_.get_username(session_id);
		int perm = session_mgr_.get_user_permission(session_id, 3);
		if (perm != 0)
		{
			_where += "AND create_user='" + username + "'";
		}
	}

	//获取区间 [(page-1)*size,page*size)
	//size 不可小于等于0
	if (size <= 0)
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO);
		write_json(handle,result);
		return WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO;
	}
	//开头
	int beg = 0;
	if (page > 0)
		beg = (page - 1)*size;
	//结尾
	int len = size; //<=0 表示获取全部
	//获取数据
	size_t all = 0;
	ret = new_mgr_.get_datas(news, _where, orderby, beg, len,&all);

	if (ret >= 0)
	{
		//返回
		result.add_object_value("news", news);
		result.add_object_value("size", ret);
		result.add_object_value("all", all);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
	write_json(handle,result);
	return ret;
}

int NewPublishImpl::deal_get_sessions(HTTP_HANDLE handle)
{
	LOG(LINFO, "deal a /get_sessions_action");
	Json sessions;
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");

	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	//获取参数
	std::string _where = json.get_string_value("where", "");
	int size = json.get_int_value("size", 0); // 数目
	int page = json.get_int_value("page", 0); //页数 1-
	std::string orderby = json.get_string_value("orderby", "publish_time DESC");//"publish_time DESC" 排序;
	//获取区间 [(page-1)*size,page*size)

	//获取区间 [(page-1)*size,page*size)
	//size 不可小于等于0
	if (size <= 0)
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO);
		write_json(handle, result);
		return WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO;
	}
	//开头
	int beg = 0;
	if (page > 0)
		beg = (page - 1)*size;
	//结尾
	int len = size; 
	size_t all = 0;
	ret = session_mgr_.get_datas(sessions, _where, orderby, beg, len,&all);

	if (ret >= 0)
	{
		//返回
		result.add_object_value("sessions", sessions);
		result.add_object_value("size", ret);
		result.add_object_value("all", all);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_get_users(HTTP_HANDLE handle)
{
	LOG(LINFO, "deal a /get_sessions_action");
	Json users;
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");

	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	//获取参数
	std::string _where = json.get_string_value("where", "");
	int size = json.get_int_value("size", 0); // 数目
	int page = json.get_int_value("page", 0); //页数 1-
	std::string orderby = json.get_string_value("orderby", "publish_time DESC");//"publish_time DESC" 排序;
	//获取区间 [(page-1)*size,page*size)

	//获取区间 [(page-1)*size,page*size)
	//size 不可小于等于0
	if (size <= 0)
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO);
		write_json(handle, result);
		return WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO;
	}
	//开头
	int beg = 0;
	if (page > 0)
		beg = (page - 1)*size;
	//结尾
	int len =  size; 
	size_t all = 0;
	ret = user_mgr_.get_users(users, _where, orderby, beg, len,&all);

	if (ret >= 0)
	{
		//返回
		result.add_object_value("users", users);
		result.add_object_value("size", ret);
		result.add_object_value("all", all);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_detele_sessions(HTTP_HANDLE handle)
{
	int ret = session_mgr_.delete_sesssion();
	if (ret != 0)
		ret = WEB_DATA_OPERATION;

	Json result;
	result.add_object_value("result", ret);
	write_json(handle,result);
	return 0;
}

int NewPublishImpl::before_filter(HTTP_HANDLE handle)
{
	auto ret = filter_session(handle);
	if (ret != 0)
		return ret;
	ret = filter_permission(handle);
	if (ret != 0)
		return ret;
	ret = filter_index(handle);
	if (ret != 0)
		return ret;
	return 0;
}

int NewPublishImpl::after_filter(HTTP_HANDLE handle)
{
	//filter_set_cookie(handle);
	
	return 0;
}

NewPublishImpl::NewPublishImpl()
{
	Ini ini;
	auto ret = ini.parser("./config.ini");
	if (0 != ret)
	{
		LOG(LFATAL,"读取配置文件 ./config.ini 失败,使用默认配置初始化");
		
		return ;
	}
	int log_lv = ini.get_config_int("log", "log_lv", 4);
	SET_OUTPUT_LV(log_lv);
	std::string path = ini.get_config_string("NewPublish", "db_new", "./db/server.data.news.db3");
	ret = new_mgr_.init(path);
	if (0 != ret)
	{
		LOG(LFATAL, "new_mgr_ 初始化失败 ,path=",path);
		return;
	}
	path = ini.get_config_string("NewPublish", "db_user", "./db/server.data.users.db3");
	ret = user_mgr_.init(path);
	if (0 != ret)
	{
		LOG(LFATAL, "user_mgr_ 初始化失败 ,path=", path);
		return;
	}
	path = ini.get_config_string("NewPublish", "db_session", "./db/server.data.sessions.db3");
	int tout = ini.get_config_int("NewPublish", "db_session_time_out",60*60*24);
	ret = session_mgr_.init(path, tout);
	if (0 != ret)
	{
		LOG(LFATAL, "session_mgr_ 初始化失败 ,path=", path);
		return;
	}
	//./web_root/cache
	uri_path_ = ini.get_config_string("NewPublish", "uri_path", "/cache");
	web_root_ = ini.get_config_string("NewPublish", "web_root", "./web_root");
	index_page_ = ini.get_config_string("NewPublish", "index", "");

	std::string permission_path = ini.get_config_string("NewPublish", "permission_path", "./permission.json");
	Json permission_json;
	permission_json.from_file(permission_path);
	auto per_array = permission_json.get_array("permission");
	for (auto it : per_array)
	{
		auto uri = it.get_string_value("uri");
		auto permission = it.get_int_value("permission");
		permission_map_.insert(std::make_pair(uri, permission));
	}
	LOG(LINFO, "插件 NewPublishImpl 加载完毕");
}

int NewPublishImpl::write_json(HTTP_HANDLE handle, const Json & json)
{
	std::string body=json.to_string();
	response_set_header(handle, CONTENT_TYPE, get_mime("json"));
	LOG(LDEBUG, "write json\n", body);
	return response_set_body(handle, body);;
}

int NewPublishImpl::read_json(HTTP_HANDLE handle, Json & json)
{
	auto body_len = request_get_body_len(handle);
	if (0 == body_len)
	{
		LOG(LERROR, "输入参数 body_len=0");
		//result.add_object_value("result", WEB_PARAMETERS_INCORRECT);
		return WEB_PARAMETERS_INCORRECT;
	}
	auto buff = SHARED_BUFF_PTR(body_len + 1);
	auto ret = request_get_body(handle, buff.get(), 0, body_len);
	if (ret<0)
	{
		LOG(LERROR, "get request body err,ret=", ret);
		//result.add_object_value("result", WEB_PARAMETERS_INCORRECT);
		return WEB_PARAMETERS_INCORRECT;
	}
	json.from_string(buff.get());
	return 0;
}

std::string NewPublishImpl::get_cookie(HTTP_HANDLE handle, const std::string & key, const std::string & notfond)
{
	std::map<std::string, std::string> *cookie_map = (std::map<std::string, std::string> *)request_get_ext_data(handle, "cookie_map");
	if (nullptr == cookie_map)
		return notfond;
	auto find = cookie_map->find(key);
	if (cookie_map->end() == find)
		return notfond;
	return find->second;
}

int NewPublishImpl::set_cookie(HTTP_HANDLE handle, const std::string & key, const std::string & value)
{
	std::map<std::string, std::string> *set_cookie_map = (std::map<std::string, std::string> *)response_get_ext_data(handle, "set_cookie_map");
	if (nullptr == set_cookie_map)
		return -1;
	set_cookie_map->operator[](key) = value;
	return 0;
}

std::string NewPublishImpl::get_uri_value(HTTP_HANDLE handle, const std::string & key, const std::string & notfond)
{
	std::map<std::string, std::string>* uri_map = (std::map<std::string, std::string>*)request_get_ext_data(handle, "uri_map");
	if (nullptr == uri_map)
		return notfond;
	auto p = uri_map->find(key);
	if (uri_map->end() == p)
		return notfond;
	return p->second;
}

void NewPublishImpl::get_news(Json & result, const std::string & _where, const std::string & orderby, int beg, int len, int tran)
{
	Json news;
	//获取数据
	size_t all = 0;
	auto ret = new_mgr_.get_datas(news, _where, orderby, beg, len,&all);

	if (ret >= 0)
	{
		//返回
		result.add_object_value("news", news);
		result.add_object_value("size", ret);
		result.add_object_value("all",all);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
}

void NewPublishImpl::get_sessions(Json & result, const std::string & _where, const std::string & orderby, int beg, int len)
{
	Json sessions;
	//获取数据
	size_t all = 0;
	auto ret = session_mgr_.get_datas(sessions, _where, orderby, beg, len,&all);

	if (ret >= 0)
	{
		//返回
		result.add_object_value("sessions", sessions);
		result.add_object_value("size", ret);
		result.add_object_value("all", all);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
}

void NewPublishImpl::get_users(Json & result, const std::string & _where, const std::string & orderby, int beg, int len)
{
	Json users;
	//获取数据
	size_t all = 0;
	auto ret = user_mgr_.get_users(users, _where, orderby, beg, len, &all);

	if (ret >= 0)
	{
		//返回
		result.add_object_value("users", users);
		result.add_object_value("size", ret);
		result.add_object_value("all", all);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
}

int NewPublishImpl::filter_session(HTTP_HANDLE handle)
{
	std::string session_id = get_cookie(handle, "session_id");
	//会话
	if (session_id != "")
	{

		if (session_mgr_.check_session(session_id) == 0)
		{
			//更新活跃时间
			session_mgr_.active(session_id);
			//set_cookie(handle,"session_id", session_id);

		}
		else
		{
			set_cookie(handle, "session_id", "");
			LOG(LDEBUG, " cookie=", session_id, " is time out");
		}

	}
	return 0;
}

int NewPublishImpl::filter_permission(HTTP_HANDLE handle)
{
	std::string session_id = get_cookie(handle, "session_id", "");
	//获取用户权限
	int user_perm = 3;
	user_mgr_.get_user_permission(session_mgr_.get_username(session_id), user_perm);

	//获取页面权限
	int page_perm = 3;
	auto uri = request_get_uri(handle);
	auto p = permission_map_.find(uri);
	if (permission_map_.end() != p)
	{
		page_perm = p->second;
	}
	//比较权限
	if (user_perm <= page_perm)
		return 0;
	LOG(LERROR, "权限不足，page_perm=", page_perm, ",user_perm=", user_perm, ",uri=", uri);
	//权限不足
	Json result;
	result.add_object_value("result", WEB_PERMISSION_DENIED);
	//response_set_status_code(handle,"403");
	write_json(handle, result);
	return -1;
}

int NewPublishImpl::filter_index(HTTP_HANDLE handle)
{
	auto uri = request_get_uri(handle);
	if ("/" == uri && index_page_!="")
	{
		uri = index_page_;
		response_set_status_code(handle, "302");
		response_set_header(handle,"Location", uri);
		return -1;
	}
	return 0;
}

split_vector split(const std::string & s, const std::string & sub_split)
{
	split_vector ret;
	size_t pos = 0;
	//保证有至少一个分割结果
	std::string str = s + sub_split;
	while (true)
	{
		auto split_pos = str.find(sub_split, pos);
		if (std::string::npos == split_pos)
			break;
		//分割
		ret.push_back(str.substr(pos, split_pos - pos));
		pos = split_pos + sub_split.size();
	}
	return ret;
}

int NewPublishImpl::deal_get_new(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");

	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	std::string code = json.get_string_value("code");
	Json info;

	ret = new_mgr_.get_data(info, code);

	if (0 == ret)
		result.add_object_value("result", WEB_OK);
	else
		result.add_object_value("result", WEB_DATA_OPERATION);

	result.add_object_value("new", info);
	result.add_object_value("like_status", new_mgr_.get_like_status(session_mgr_.get_username(session_id), code));

	write_json(handle,result);
	return 0;
}

int NewPublishImpl::deal_get_now_user(HTTP_HANDLE handle)
{
	std::string session_id = get_cookie(handle,"session_id");
	Json ret_json;
	Json user;
	std::string username = session_mgr_.get_username(session_id);

	int ret = WEB_DATA_OPERATION;

	if (username != "")
	{
		if (0 == user_mgr_.get_user_info(user, username))
		{
			ret = WEB_OK;
		}
		else
		{
			ret = WEB_DATA_OPERATION;
			session_mgr_.logout(session_id);
		}
	}
	else
		ret = 1;//游客

	ret_json.add_object_value("result", ret);
	ret_json.add_object_value("user", user);

	write_json(handle,ret_json);
	return ret;
}

int NewPublishImpl::deal_login(HTTP_HANDLE handle)
{
	Json login_info;
	Json data;
	std::string session_id = get_cookie(handle, "session_id");
	session_mgr_.logout(session_id);

	read_json(handle, login_info);
	
	auto username = login_info.get_string_value("username", "");
	auto password = login_info.get_string_value("password", "");

	if (username == "" || password == "")
	{
		data.add_object_value("result", WEB_PARAMETERS_INCORRECT_USER_PWD_NULL);
		write_json(handle,data);
		return WEB_PARAMETERS_INCORRECT_USER_PWD_NULL;
	}

	int ret = user_mgr_.check_password(username, password);

	if (ret == 0)
	{
		Json session;
		auto addr = get_client_address(handle);
		session.add_object_value("username", username);
		session.add_object_value("ip", addr.first);
		session.add_object_value("port", addr.second);
		int permission = 3;
		user_mgr_.get_user_permission(username, permission);
		session.add_object_value("permission", permission);
		ret = session_mgr_.login(session);

		//登录成功
		if (0 == ret)
		{
			data.add_object_value("session", session);
			//新的会话
			set_cookie(handle, "session_id", session.get_string_value("code"));
			data.add_object_value("result", WEB_OK);
		}
		else
			data.add_object_value("result", WEB_DATA_OPERATION);
	}
	else
		data.add_object_value("result", WEB_PERMISSION_DENIED_PWD_ERR);


	write_json(handle,data);
}

int NewPublishImpl::deal_register(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");

	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	
	std::string admin_password = json.get_string_value("admin_password");

	auto username = json.get_string_value("username");
	auto password = json.get_string_value("password");
	auto permission = json.get_int_value("permission");
	
	//用户名为空
	if (username == "")
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_USER_PWD_NULL);
		write_json(handle, result);
		return WEB_PARAMETERS_INCORRECT_USER_PWD_NULL;
	}
	//用户名重复
	if (user_mgr_.check_user(username) == 0)
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_USER_REPEAT);
		write_json(handle, result);
		return WEB_PARAMETERS_INCORRECT_USER_REPEAT;
	}
	//创建权限 管理员 编辑 用户 需要管理员权限及密码
	if (permission < 2)
	{
		std::string username =session_mgr_.get_username(session_id);
		int p = session_mgr_.get_user_permission(session_id);
		if (p != 0)
		{
			result.add_object_value("result", WEB_PERMISSION_DENIED);
			write_json(handle, result);
			return WEB_PERMISSION_DENIED;
		}
		
		if (user_mgr_.check_password(username, admin_password) != 0)
		{
			result.add_object_value("result", WEB_PERMISSION_DENIED_ADMIN_PWD_ERR);
			write_json(handle, result);
			return WEB_PERMISSION_DENIED_ADMIN_PWD_ERR;
		}

	}
	Json register_user;
	register_user.add_object_value("username", username);
	register_user.add_object_value("password", password);
	register_user.add_object_value("permission", permission);
	
	ret = user_mgr_.create(register_user);
	if (0 == ret)
		result.add_object_value("result", WEB_OK);
	else
		result.add_object_value("result", WEB_DATA_OPERATION);

	write_json(handle, result);
}

int NewPublishImpl::deal_logout(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");

	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}
	//获取参数

	//本人退出
	std::string dst_session_id = json.get_string_value("session_id", "");
	if ("" == dst_session_id || session_id == dst_session_id)
	{
		if (0 != session_mgr_.logout(session_id))
			ret = WEB_DATA_OPERATION; //返回值非零

	}//删除会话
	else
	{
		int per = session_mgr_.get_user_permission(session_id);
		if (per == 0)
		{
			if (0 != session_mgr_.logout(dst_session_id))
				ret = WEB_DATA_OPERATION; //返回值非零

		}
		else//权限不足
			ret = WEB_PERMISSION_DENIED;
	}
	//返回结果
	result.add_object_value("result", ret);
	write_json(handle, result);
}

int NewPublishImpl::deal_create_new(HTTP_HANDLE handle)
{
	Json result;
	std::string session_id = get_cookie(handle, "session_id", "");
	Json _new;
	auto author = session_mgr_.get_username(session_id);
	_new.add_object_value("author", author);
	_new.add_object_value("create_user", author);
	_new.add_object_value("create_time", (size_t)time(nullptr));
	
	int ret = new_mgr_.create(_new);
	if (0 == ret)
		result.add_object_value("result", WEB_OK);
	else
		result.add_object_value("result", WEB_DATA_OPERATION);
	result.add_object_value("new", _new);

	write_json(handle, result);
	return 0;
}

int NewPublishImpl::deal_save_new(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	Json new_data = json.get_object("new");
	if (ret == 0)
	{
		new_data.add_object_value("modify_user", session_mgr_.get_username(session_id));
		new_data.add_object_value("modify_time", (size_t)time(nullptr));
		//+转化为%20
		new_data.add_object_value("content", replace(new_data.get_string_value("content"), "+", "%20"));
		
		ret = new_mgr_.save(new_data);
		if (0 == ret)
			result.add_object_value("result", WEB_OK);
		else
			result.add_object_value("result", WEB_DATA_OPERATION);
	}
	else
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT);
	}
	write_json(handle, result);
	return 0;
}

int NewPublishImpl::deal_upload_new_image(HTTP_HANDLE handle)
{
	std::string uris;
	Json form_data,result;
	int ret = 1;
	const std::string new_id = get_uri_value(handle,"new_id");
	std::string *form_json = (std::string*)request_get_ext_data(handle, "form_json");
	if (nullptr == form_json)
	{
		result.add_object_value("error", WEB_PARAMETERS_INCORRECT);
		result.add_object_value("message", "表单解析出错");
		//message
		write_json(handle,result);
		return WEB_PARAMETERS_INCORRECT;
	}
	LOG(LINFO, "get form json\n", *form_json);
	form_data.from_string(*form_json);
	auto form = form_data.get_array();
	for (auto it : form)
	{
		if (1==it.get_int_value("is_cache") )
		{
			std::string data_path = new_mgr_.get_data_path(new_id);
			std::string uri = uri_path_ +"/"+ new_id + "/";

			if (data_path == "")
			{
				std::string new_path = web_root_ + uri;

				if (!boost::filesystem::exists(new_path))
				{
					boost::filesystem::create_directory(new_path);
				}
				new_mgr_.set_data_path(new_id, new_path);
				data_path = new_path;
			}
			//获取后缀
			std::string ext = ".jpg";
			std::string filename = it.get_string_value("Content-Disposition.filename");
			auto p = filename.find_last_of(".");
			if (std::string::npos != p)
				ext = filename.substr(p);
			if (ext != ".jpg"&&ext != ".ico"&&ext != ".gif"&&ext != ".JPG"&&ext != ".png"&&ext != ".bmp")
			{
				ret = 1;
				break;
			}
			auto filepath = it.get_string_value("path");
			auto uuid = it.get_string_value("uuid");
			data_path += uuid+ext;
			uri += uuid + ext;
			//看文件是否存在
			if (!boost::filesystem::exists(data_path))
				boost::filesystem::rename(filepath, data_path);//移动
			else
				boost::filesystem::remove(filepath);//删除
			uris += uri + ",";
			ret = 0;
		}
	}

	result.add_object_value("error", ret);
	if (ret == 0)
		result.add_object_value("url", uris.substr(0, uris.size() - 1));
	else
		result.add_object_value("message", "不支持的文件类型");
	//message
	write_json(handle,result);
	return 0;
}

int NewPublishImpl::deal_new_like(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}
	int blike = json.get_int_value("blike", -1000);
	std::string new_id = json.get_string_value("new_id");
	std::string username = session_mgr_.get_username(session_id);

	//参数不对
	if (username == "" || blike == -1000 || new_id == "")
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT);
		write_json(handle, result);
		return WEB_PARAMETERS_INCORRECT;
	}

	int num = new_mgr_.like(username, new_id, blike);

	if (num != 0)
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
		write_json(handle, result);
		return WEB_DATA_OPERATION;
	}

	result.add_object_value("result", WEB_OK);
	write_json(handle, result);
	return WEB_OK;
}

int NewPublishImpl::deal_add_new_comment(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}
	Json _new;
	auto author = session_mgr_.get_username(session_id);
	_new.add_object_value("author", author);
	_new.add_object_value("create_user", author);
	_new.add_object_value("modify_user", author);
	
	_new.add_object_value("parent", json.get_string_value("new_id"));
	_new.add_object_value("property",1);
	_new.add_object_value("content", json.get_string_value("comment"));
	_new.add_object_value("title", "comment");
	
	ret = new_mgr_.comment(_new);
	if (0 == ret)
		result.add_object_value("result", WEB_OK);
	else
		result.add_object_value("result", WEB_DATA_OPERATION);

	write_json(handle, result);
}

int NewPublishImpl::deal_remove_new(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	const std::string new_id = json.get_string_value("new_id");

	if (new_id == "")
		ret = WEB_PARAMETERS_INCORRECT;
	else
	{
		ret = new_mgr_.remove(new_id);
		if (0 != ret)
			ret = WEB_DATA_OPERATION;
	}

	result.add_object_value("result", ret);
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_remove_user(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	const std::string username = json.get_string_value("username");
	if (username == "")
	{
		ret = WEB_PARAMETERS_INCORRECT_USER_PWD_NULL;
	}
	//不可删除 admin
	else if (username == "admin")
	{
		ret = WEB_PERMISSION_DENIED_MOD_ADMIN;
	}
	else
	{
		ret = user_mgr_.remove_and_active(username);
		if (ret != 0)
			ret = WEB_DATA_OPERATION;
	}

	result.add_object_value("result", ret);
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_delete_user(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	const std::string username = json.get_string_value("username");
	if (username == "")
	{
		ret = WEB_PARAMETERS_INCORRECT_USER_PWD_NULL;
	}
	else if (username == "admin")
	{
		ret = WEB_PERMISSION_DENIED_MOD_ADMIN;
	}
	else
	{
		ret = user_mgr_.delete_user(username);
		if (ret != WEB_OK)
			ret = WEB_DATA_OPERATION;
	}
	result.add_object_value("result", ret);
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_delete_new(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	const std::string new_id = json.get_string_value("new_id");
	if (new_id == "")
		ret = WEB_PARAMETERS_INCORRECT;
	else
	{
		ret = new_mgr_.delete_it(new_id);
		if (ret != WEB_OK)
			ret = WEB_DATA_OPERATION;
	}
	result.add_object_value("result", ret);
	write_json(handle, result);
	return 0;
}

int NewPublishImpl::deal_reset_password(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	std::string username = json.get_string_value("username");
	std::string now_username = session_mgr_.get_username(session_id);
	std::string new_password = json.get_string_value("new_password");
	std::string password = json.get_string_value("password");

	//自己修改自己的密码
	if (username == "" || username == now_username)
	{
		if (user_mgr_.check_password(now_username, password) != 0)
			ret = WEB_PERMISSION_DENIED_PWD_ERR;
		else
		{
			ret = user_mgr_.edit_password(username, new_password);
			if (ret != WEB_OK)
				ret = WEB_DATA_OPERATION;
		}
	}
	else //修改其他用户的密码
	{
		//检查权限 只有管理员才可以修改其他用户的密码
		int per_n = 3/*当前用户权限*/;
		user_mgr_.get_user_permission(now_username, per_n);
		if (per_n == 0)
		{
			if (user_mgr_.check_password(now_username, password) != 0)
				ret = WEB_PERMISSION_DENIED_ADMIN_PWD_ERR;
			else
			{
				ret = user_mgr_.edit_password(username, new_password);
				if (ret != WEB_OK)
					ret = WEB_DATA_OPERATION;
			}
		}
		else
			ret = WEB_PERMISSION_DENIED;

	}
	result.add_object_value("result", ret);
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_reset_user_permission(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	std::string username = json.get_string_value("username");
	int permission = json.get_int_value("permission");

	if (username == "admin")
		ret = WEB_PERMISSION_DENIED_MOD_ADMIN;
	else
	{
		ret = user_mgr_.reset_user_permission(username, permission);
		if (ret != WEB_OK)
			ret = WEB_DATA_OPERATION;
	}

	result.add_object_value("result", ret);
	write_json(handle, result);
	return ret;
}

int NewPublishImpl::deal_reset_new_status(HTTP_HANDLE handle)
{
	Json json, result;
	std::string session_id = get_cookie(handle, "session_id", "");
	auto ret = read_json(handle, json);
	if (0 != ret)
	{
		LOG(LERROR, "get json err,ret=", ret);
		result.add_object_value("result", ret);
		write_json(handle, result);
		return ret;
	}

	int status = json.get_int_value("status", -1);
	std::string code = json.get_string_value("new_id");

	if (status == -1 && code == "")
	{
		ret = WEB_PARAMETERS_INCORRECT;
	}
	else if (status < 0 || status>3)
	{
		ret = WEB_PARAMETERS_INCORRECT;
	}
	else
	{
		ret = new_mgr_.reset_status(code, status);
		if (ret != WEB_OK)
			ret = WEB_DATA_OPERATION;
	}

	result.add_object_value("result", ret);
	write_json(handle, result);
	return ret;
}
