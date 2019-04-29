//回调函数
#include <iostream>

#include "web_service.h"

#include "ini/ini.h"

using namespace net_service::web;
int main()
{

	WebSeviceConfigData config;
	
	Ini ini;
	int ret = ini.patser("./config.ini");
	if (0 != ret)
	{
		std::cout << "读取配置文件 ./config.ini 失败" << std::endl;
		return -1;
	}
	config.log_path = ini.get_config_string("log", "path", "./log");

	config.ip = ini.get_config_string("tcp", "ip","0.0.0.0");
	config.port = ini.get_config_int("tcp", "port",8000);
	config.accept_num = ini.get_config_int("tcp", "accept", 16);
	config.recv_buff_size = ini.get_config_int("tcp", "recv_buff_size", 1024*1024*10);
	config.set_buff_size = ini.get_config_int("tcp", "set_buff_size", 1024 * 1024 * 10);

	config.web_root = ini.get_config_string("web", "root", "./web_root");
	

	WEB_HANDLE handle= start_server(config);
	if (nullptr == handle)
	{
		std::cout << "服务启动失败" << std::endl;
		return -2;
	}

	std::string handlers = ini.get_config_string("web", "handler", "./handlers.json");

	ret = load_server_handler(handle, handlers);
	if (0 != ret)
	{
		std::cout << "绑定服务句柄失败,ret=" <<ret<< std::endl;
		close_server(handle);
		handle = 0;
		return -3;
	}

	while (true)
	{
		char c;
		std::cin >> c;
		if (c == 'q')
		{
			break;
		}
	}
}