#ifndef WEB_SEVICE_H
#define WEB_SEVICE_H

#define WIN 1

#ifdef  WIN
#include <Windows.h>
#endif //  WIN32

#include <map>
#include <functional>
#include <vector>

#include "http_service.h"
#include "web_service.h"
#define SHARED_BUFF_PTR(size) std::shared_ptr<char>(new char[size], std::default_delete<char[]>())
typedef int(*P_WEN_HANDLER)(HTTP_HANDLE handle, int err);

namespace net_service
{
	namespace web
	{
		class WebServer
		{
			//共享指针参数
			typedef std::function<int(HTTP_HANDLE handle, int err)> WEB_HANDLER;
		public:
			//底层调用，用于处理一个连接
			int http_handler(HTTP_HANDLE handle, int err_code);
			
			//初始化
			WebServer(WebSeviceConfigData config);
			//关闭
			~WebServer();

			//设置请求处理回调，uri处理的连接，call_back 回调函数，user_data 用户数据，访问权限perm 权限 perm 最大 0 大于不可访问
			int set_deal_with(const std::string&uri, WEB_HANDLER web_handler);

			//设置过滤函数 按照设置的顺序调用
			//处理之前调用
			int set_before_filter(WEB_HANDLER call_back);
			//处理之后调用
			int set_after_filter(WEB_HANDLER call_back);
			//加载handler
			int load_handler(const std::string config_path = "./handler.json");
		public:

			//配置
			WebSeviceConfigData config_data_;

		private:
			//默认回调函数
			int deal_default(HTTP_HANDLE handle,int err);

			//根据请求uri 分发请求
			int call(const std::string& uri, HTTP_HANDLE handle, int err);

			void* get_func(const std::string &path,const std::string &name);

			//int set_res_mime(HTTP_HANDLE handle,const std::string& uri);
			//不缓存返回 ""
			const std::string & try_page_cache(const std::string uri);
		private:
			//处理回调
			std::map<const std::string, WEB_HANDLER> func_map_;

			//过滤器 //过滤函数 返回0 通过过滤 
			std::vector<WEB_HANDLER> before_filter_vec_;
			std::vector<WEB_HANDLER> after_filter_vec_;

			std::map<std::string, std::string> page_cache_map_;
#ifdef  WIN
			std::map<std::string, HINSTANCE> dll_map_;
#endif
			HTTP_HANDLE http_handle_;
		};
	}
}
#endif