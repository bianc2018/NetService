//回调函数
#include <iostream>
#include "Windows.h"

#include "web_service.h"

#include "ini/ini.h"

using namespace net_service::web;
#define VERSION "v2.0 @"##__TIMESTAMP__
void disable_console_edit()
{
#ifdef _WIN32
	//禁止编辑界面
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;//移除快速编辑模式
	mode &= ~ENABLE_INSERT_MODE;//移除插入模式
	mode &= ~ENABLE_MOUSE_INPUT;//移除鼠标操作
	SetConsoleMode(hStdin, mode);//设置
#endif
}
void print_version_info()
{
	//打印初始化信息
	std::cout << "/***************************************************************/" << std::endl;
	std::cout << "/*" << std::endl;
	std::cout << "/*		name   =" << "NewPublish" << std::endl;
	std::cout << "/*		author =" << "HANQUAN LIAN" << std::endl;
	std::cout << "/*		version=" << VERSION << std::endl;
	std::cout << "/*" << std::endl;
	std::cout << "/**************************************************************/" << std::endl << std::endl;
}

int main()
{
	print_version_info();
	WebSeviceConfigData config;
	
	Ini ini;
	int ret = ini.parser("./config.ini");
	if (0 != ret)
	{
		std::cout << "读取配置文件 ./config.ini 失败" << std::endl;
		return -1;
	}
	config.log_path = ini.get_config_string("log", "path", "./log");

	config.ip = ini.get_config_string("tcp", "ip","0.0.0.0");
	config.port = ini.get_config_int("tcp", "port",8000);
	config.accept_num = ini.get_config_int("tcp", "accept_num", 16);
	config.recv_buff_size = ini.get_config_int("tcp", "recv_buff_size", 1024*1024*10);
	config.send_buff_size = ini.get_config_int("tcp", "send_buff_size", 1024 * 1024 * 10);
	config.timeout = ini.get_config_int("tcp", "timeout", 1024 * 1024 * 10);

	config.mime_path = ini.get_config_string("http", "mime_path");
	config.reason_path = ini.get_config_string("http", "reason_path");

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
		//close_server(handle);
		//handle = 0;
		//return -3;
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