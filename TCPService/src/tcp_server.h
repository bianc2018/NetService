//异步的tcp服务器

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <atomic>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/function.hpp>

#define BOOST_ASIO_NO_DEPRECATED
#include "../tcp_define.h"

namespace net_service
{
	namespace tcp
	{
		//服务端 p ip::tcp::v4()
		#define ipv4 boost::asio::ip::tcp::v4()
		//服务
		typedef boost::asio::io_context io_service;
		//套接字
		typedef boost::asio::ip::tcp::socket sock;
		typedef std::shared_ptr<sock> sock_ptr;
		//端点
		typedef boost::asio::ip::tcp::endpoint point;
		//地址 ::address
		typedef boost::asio::ip::address address;
		//接受 acceptor
		typedef boost::asio::ip::tcp::acceptor acceptor;

		//表示一个服务器
		class TcpServer :public boost::noncopyable
		{
		public:
			TcpServer(io_service &service,boost::function<void(TCP_HANDLE handle,std::shared_ptr<boost::asio::ip::tcp::socket> psock,int err)> accept_call_back, std::string ip, int port,int accept_num=4);
			
			int stop();

			~TcpServer();
		public:
			//句柄
			TCP_HANDLE handle_;
		private:
			void accept();
		private:
			
			//异步服务
			io_service &service_;
			//接收连接
			acceptor server_;
			//回调
			boost::function<void(TCP_HANDLE handle, std::shared_ptr<boost::asio::ip::tcp::socket> psock,int err)> accept_handler_;
			//is_run 1 run 0 stop
			std::atomic<bool> is_run;
		};
	}
}
#endif
