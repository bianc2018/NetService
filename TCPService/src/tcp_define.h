//网络相关的定义
#ifndef TCP_DEFINE_H
#define TCP_DEFINE_H

#include <iostream>
#include <memory>
const int FRAME_SIZE(4096);

#define TCP_SERVICE_API  extern   "C"     __declspec( dllexport )
//句柄
typedef long long TCP_HANDLE;

//回调函数
//接收到连接
typedef void(*ACCEPT_HANDLER)(TCP_HANDLE handle);
//接收完数据 返回
typedef void(*RECV_HANDLER)(TCP_HANDLE handle, std::shared_ptr<char> recv_data, int recv_len);
//发送完数据
typedef void(*SEND_HANDLER)(TCP_HANDLE handle);

//动态数组指针
#define SHARED_BUFF_PTR(size) std::shared_ptr<char>(new char[size], std::default_delete<char[]>())
#define SHARED_ANY_PTR(type,size) std::shared_ptr<type>(new type[size], std::default_delete<type[]>())

enum TCP_ERROR_CODE
{
	TCP_ERROR_CODE_OK=0,

	TCP_ERROR_CODE_RECV_ERROR = -1,
	TCP_ERROR_CODE_RECV_TIME_OUT = -2,

	TCP_ERROR_CODE_SEND_ERROR = -3,
	TCP_ERROR_CODE_SEND_TIME_OUT = -4,

	//客户端 连接失败
	TCP_ERROR_CLIENT_CONNECT_ERROR = -5,

	//服务端 开启失败
	TCP_ERROR_SERVER_OPEN_ERROR = -6,
	//服务端 set_option 设置参数错误
	TCP_ERROR_SERVER_SET_ERROR = -7,
	//服务端 绑定地址失败
	TCP_ERROR_SERVER_BIND_ERROR = -8,

	//连接句柄无效
	TCP_ERROR_LINK_HANDLE_ERROR = -9,
	//服务句柄无效
	TCP_ERROR_SERVER_HANDLE_ERROR = -9,
};
#endif 