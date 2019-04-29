#include "web_server.h"

#ifdef  WIN32
#include <Windows.h>
#endif //  WIN32

#include <boost/filesystem.hpp>

#include "http_service.h"

#include "log/log.hpp"
#include "json/json.h"

void net_service::web::WebServer::http_handler(HTTP_HANDLE handle, int err_code)
{
	//before过滤
	for (auto filter : before_filter_vec_)
	{
		if (0 != filter(handle,err_code))
			return;
	}
	auto uri = http::request_get_uri(handle);
	//处理
	call(uri,handle,err_code);

	//after过滤
	for (auto filter : after_filter_vec_)
	{
		filter(handle, err_code);
	}
	return;
}

net_service::web::WebServer::WebServer(WebSeviceConfigData config)
{
	http_handle_ = http::start_server(config_data_.ip, config_data_.port, \
					std::bind(&WebServer::http_handler, this, std::placeholders::_1, std::placeholders::_2),\
					config_data_.accept_num);
}

net_service::web::WebServer::~WebServer()
{
	http::close_server(http_handle_);
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
			LOG(LERROR, "无法找到函数");
			continue;
		}

		if (0 == type)
		{
			set_before_filter((P_WEN_HANDLER)func);
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
		}
		else if (2 == type)
		{
			set_after_filter((P_WEN_HANDLER)func);
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
		http::response_set_status_code(handle,"404");
		http::response_set_reason_phrase(handle, "not found");
		return HTTP_SET_REQ_NO_EXIST;
	}
	return HTTP_ERROR_CODE_OK;
}

int net_service::web::WebServer::call(const std::string & uri, HTTP_HANDLE handle, int err)
{
	auto it = func_map_.find(uri);
	//找不到，默认处理
	if (it == func_map_.end())
		return deal_default(handle,err);

	return it->second(handle, err);
}

void* net_service::web::WebServer::get_func(const std::string & path, const std::string & name)
{
	void *func_addr = nullptr;
#ifdef  WIN32
	HINSTANCE handle = LoadLibraryA(path.c_str());
	if (handle)
	{
		func_addr = GetProcAddress(handle, name.c_str());
	}
	else
	{
		LOG(LERROR, "加载插件失败,path=", path, ", func_name=", name);
	}
	FreeLibrary(handle);
#endif
	return func_addr;
}
