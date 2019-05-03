#include "web_server.h"

#include <boost/filesystem.hpp>

#include "http_service.h"

#include "log/log.hpp"
#include "json/json.h"

int net_service::web::WebServer::http_handler(HTTP_HANDLE handle, int err_code)
{
	//before过滤
	for (auto filter : before_filter_vec_)
	{
		if (0 != filter(handle,err_code))
			return -1;
	}
	auto uri = http::request_get_uri(handle);
	//处理
	LOG(LDEBUG, "get a request uri=", uri);
	call(uri,handle,err_code);

	//after过滤
	for (auto filter : after_filter_vec_)
	{
		filter(handle, err_code);
	}
	return 0;
}

net_service::web::WebServer::WebServer(WebSeviceConfigData config):config_data_(config)
{
	http::set_log_path(config.log_path);
	http::set_recv_buff_size(config.recv_buff_size);
	http::set_send_buff_size(config.send_buff_size);
	http::set_thread_num(config.accept_num);
	http::set_time_out(config.timeout);

	//init mime
	Json mime, reason;
	mime.from_file(config.mime_path);
	auto mime_key = mime.get_root_keys();
	for (auto key : mime_key)
	{
		http::set_mime(key, mime.get_string_value(key, HTML_MIME));
	}
	reason.from_file(config.reason_path);
	auto reason_key = reason.get_root_keys();
	for (auto key : reason_key)
	{
		http::set_reason(key, reason.get_string_value(key, UNKNOW_REASON));
	}

	http_handle_ = http::start_server(config_data_.ip, config_data_.port, \
					std::bind(&WebServer::http_handler, this, std::placeholders::_1, std::placeholders::_2),\
					config_data_.accept_num);
}

net_service::web::WebServer::~WebServer()
{
	http::close_server(http_handle_);

#ifdef  WIN
	//释放
	for (auto p : dll_map_)
	{
		FreeLibrary(p.second);
	}
	//FreeLibrary(handle);
#endif
}

int net_service::web::WebServer::set_deal_with(const std::string & uri, WEB_HANDLER web_handler)
{
	func_map_.insert(std::make_pair(uri, web_handler));

	return 0;
}

int net_service::web::WebServer::set_before_filter(WEB_HANDLER call_back)
{
	before_filter_vec_.push_back(call_back);
	return 0;
}

int net_service::web::WebServer::set_after_filter(WEB_HANDLER call_back)
{
	after_filter_vec_.push_back(call_back);
	return 0;
}

int net_service::web::WebServer::load_handler(const std::string config_path)
{
	Json config;
	int ret = config.from_file(config_path);
	if (0 != ret)
	{
		LOG(LERROR,"json 加载失败！path=", config_path,",ret=",ret);
		return -1;
	}
	std::string version = config.get_string_value("http_service_version","");
	if (version != HTTP_SERVICE_VERSION)
	{
		LOG(LERROR, "json 加载失败！reason=本系统只支持 version:", HTTP_SERVICE_VERSION, ",现在加载的版本是 ", version);
		return -2;
	}
	auto handlers =  config.get_array("handlers");

	for (auto handler : handlers)
	{
		int type = handler.get_int_value("type", -1);
		if (type < 0 || type>3)
		{
			LOG(LERROR, "不支持的type=", type);
			continue;
		}
		
		std::string dll_path = handler.get_string_value("dll", "");
		if ("" == dll_path)
		{
			LOG(LERROR, "无法找到dll键值");
			continue;
		}

		std::string func_name = handler.get_string_value("func", "");
		if ("" == dll_path)
		{
			LOG(LERROR, "无法找到func键值");
			continue;
		}

		auto func = get_func(dll_path, func_name);
		if (nullptr == func)
		{
			LOG(LERROR, "无法找到函数 ", dll_path,"::",func_name);
			continue;
		}

		if (0 == type)
		{
			set_before_filter((P_WEN_HANDLER)func);
			LOG(LINFO, "load handle:", dll_path, ":", func_name, "，type(前置过滤器) ok");
		}
		else if (1 == type)
		{
			std::string uri = handler.get_string_value("uri", "");
			if ("" == uri)
			{
				LOG(LERROR, "无法找到uri键值");
				continue;
			}
			set_deal_with(uri, (P_WEN_HANDLER)func);
			LOG(LINFO, "load handle:", dll_path, ":", func_name, "，type(处理器 绑定uri=",uri,") ok");
		}
		else if (2 == type)
		{
			set_after_filter((P_WEN_HANDLER)func);
			LOG(LINFO, "load handle:", dll_path, ":", func_name, "，type(后置过滤器) ok");
		}
	}
	return 0;
}

int net_service::web::WebServer::deal_default(HTTP_HANDLE handle, int err)
{
	auto uri = http::request_get_uri(handle);
	if (uri == "")
	{
		LOG(LERROR, "uri no find,handle=", handle);
		return HTTP_SET_REQ_NO_EXIST;
	}

	if(0!=http::response_set_body_by_file(handle, config_data_.web_root + uri))
	{
		LOG(LERROR, "404 uri=", config_data_.web_root + uri);
		
		auto ret1 = http::response_set_status_code(handle,"404");
		auto ret2 = http::response_set_reason_phrase(handle, "not found");

		LOG(LDEBUG, "404 handle=", handle, ret1, ret2);
		return HTTP_SET_REQ_NO_EXIST;
	}
	return HTTP_ERROR_CODE_OK;
}

int net_service::web::WebServer::call(const std::string & uri, HTTP_HANDLE handle, int err)
{
	LOG(LINFO, "deal request uri=", uri);
	auto it = func_map_.find(uri);
	//找不到，默认处理
	if (it == func_map_.end())
		return deal_default(handle,err);

	return it->second(handle, err);
}

void* net_service::web::WebServer::get_func(const std::string & path, const std::string & name)
{
	void *func_addr = nullptr;
#ifdef  WIN
	HINSTANCE handle;
	auto p = dll_map_.find(path);
	if (dll_map_.end() == p)
	{
		handle = LoadLibraryA(path.c_str());
		dll_map_[path]= handle;
	}
	else
	{
		handle = p->second;
	}

	if (handle)
	{
		func_addr = GetProcAddress(handle, name.c_str());
	}
	else
	{
		LOG(LERROR, "加载插件失败,path=", path, ", func_name=", name);
	}
	//FreeLibrary(handle);
#endif
	return func_addr;
}
