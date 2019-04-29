#include "web_service.h"

#include "web_server.h"

WEB_SERVICE_API WEB_HANDLE net_service::web::start_server(WebSeviceConfigData config)
{
	WebServer * handle = new WebServer(config);
	return handle;
}

WEB_SERVICE_API int net_service::web::load_server_handler(WEB_HANDLE handle, const std::string & path)
{
	WebServer * web = (WebServer*)(handle);
	return web->load_handler(path);
}

WEB_SERVICE_API void net_service::web::close_server(WEB_HANDLE handle)
{
	WebServer * web = (WebServer*)(handle);
	delete web;
	web = nullptr;
}
