#include "new_publish_impl.h"

#include "ini/ini.h"

using namespace net_service::http;

NewPublishImpl & NewPublishImpl::instance()
{
	static NewPublishImpl impl;
	return impl;
}

int NewPublishImpl::deal_get_news(HTTP_HANDLE handle)
{
	LOG(LINFO, "deal a /get_news_action");
	vec_new_info news;
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

	//��ȡ����
	std::string _where = json.get_string_value("where", "");
	int size = json.get_int_value("size", 0); // ��Ŀ
	int page = json.get_int_value("page", 0); //ҳ�� 1-
	int byuser = json.get_int_value("byuser", 0);
	std::string orderby = json.get_string_value("orderby", "publish_time DESC");//"publish_time DESC" ����;
	int transfer_content = json.get_int_value("transfer_content", 0); //�Ƿ񴫵�����
	//��ȡ���� [(page-1)*size,page*size)

	//�Ƿ�ֻ���ݸ������ţ���ǰ�û�����������
	if (byuser == 1)
	{
		std::string username = session_mgr_.get_username(session_id);
		int perm = session_mgr_.get_user_permission(session_id, 3);
		if (perm != 0)
		{
			_where += "AND create_user='" + username + "'";
		}
	}

	//��ȡ���� [(page-1)*size,page*size)
	//size ����С�ڵ���0
	if (size <= 0)
	{
		result.add_object_value("result", WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO);
		write_json(handle,result);
		return WEB_PARAMETERS_INCORRECT_PAGE_SIZE_ZERO;
	}
	//��ͷ
	int beg = 0;
	if (page > 0)
		beg = (page - 1)*size;
	//��β
	int end = page * size; //<=0 ��ʾ��ȡȫ��
	//��ȡ����
	ret = new_mgr_.get_datas(news, _where, orderby, beg, end);

	if (ret >= 0)
	{
		Json arr;
		arr.init_array();
		for (auto n : news)
		{
			//����������
			if (transfer_content == 0)
				n.content = "";
			arr.add_array_value(n.to_json());
		}
		//����
		result.add_object_value("news", arr);
		result.add_object_value("size", news.size());
		result.add_object_value("all", ret);
		result.add_object_value("result", WEB_OK);
	}
	else
	{
		result.add_object_value("result", WEB_DATA_OPERATION);
	}
	write_json(handle,result);
	return ret;
}




int NewPublishImpl::before_filter(HTTP_HANDLE handle)
{
	auto ret = filter_session(handle);
	if (ret != 0)
		return ret;
	ret = filter_permission(handle);
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
	auto ret = ini.patser("./config.ini");
	if (0 != ret)
	{
		LOG(LERROR,"��ȡ�����ļ� ./config.ini ʧ��,ʹ��Ĭ�����ó�ʼ��");
		//new_mgr_.init("./db/server.data.news.db3");
		//user_mgr_.init("./db/server.data.users.db3");
		//session_mgr_.init("./db/server.data.sessions.db3");
		return ;
	}
	std::string path = ini.get_config_string("NewPublish", "db_new", "./db/server.data.news.db3");
	ret = new_mgr_.init(path);
	if (0 != ret)
	{
		LOG(LERROR, "new_mgr_ ��ʼ��ʧ�� ,path=",path);
		return;
	}
	path = ini.get_config_string("NewPublish", "db_user", "./db/server.data.users.db3");
	ret = user_mgr_.init(path);
	if (0 != ret)
	{
		LOG(LERROR, "user_mgr_ ��ʼ��ʧ�� ,path=", path);
		return;
	}
	path = ini.get_config_string("NewPublish", "db_session", "./db/server.data.sessions.db3");
	int tout = ini.get_config_int("NewPublish", "db_session_time_out",60*60*24);
	ret = session_mgr_.init(path, tout);
	if (0 != ret)
	{
		LOG(LERROR, "session_mgr_ ��ʼ��ʧ�� ,path=", path);
		return;
	}
	LOG(LINFO, "��� NewPublishImpl �������");
}

int NewPublishImpl::write_json(HTTP_HANDLE handle, const Json & json)
{
	std::string body=json.to_string();
	response_set_header(handle, CONTENT_TYPE, get_mime("json"));
	LOG(LINFO, "write json\n", body);
	return response_set_body(handle, body);;
}

int NewPublishImpl::read_json(HTTP_HANDLE handle, Json & json)
{
	auto body_len = std::stoi(request_get_header(handle, BODY_LEN, "0"));
	if (0 == body_len)
	{
		LOG(LERROR, "������� body_len=0");
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
	std::map<std::string, std::string> *set_cookie_map = (std::map<std::string, std::string> *)request_get_ext_data(handle, "set_cookie_map");
	if (nullptr == set_cookie_map)
		return -1;
	set_cookie_map->operator[](key) = value;
	return 0;
}

int NewPublishImpl::filter_session(HTTP_HANDLE handle)
{
	std::string session_id = get_cookie(handle, "session_id");
	//�Ự
	if (session_id != "")
	{

		if (session_mgr_.check_session(session_id) == 0)
		{
			//���»�Ծʱ��
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
	return 0;
}


split_vector split(const std::string & s, const std::string & sub_split)
{
	split_vector ret;
	size_t pos = 0;
	//��֤������һ���ָ���
	std::string str = s + sub_split;
	while (true)
	{
		auto split_pos = str.find(sub_split, pos);
		if (std::string::npos == split_pos)
			break;
		//�ָ�
		ret.push_back(str.substr(pos, split_pos - pos));
		pos = split_pos + sub_split.size();
	}
	return ret;
}

int NewPublishImpl::deal_get_now_user(HTTP_HANDLE handle)
{
	std::string session_id = get_cookie(handle,"session_id");
	Json ret_json;
	UserInfo user;
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
		ret = 1;//�ο�

	ret_json.add_object_value("result", ret);
	ret_json.add_object_value("user", user.to_json());

	write_json(handle,ret_json);
	return ret;
}
