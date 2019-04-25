#include "tcp_server.h"

#include <mutex>

#include "../include/log/log.hpp"

std::mutex lock;

net_service::tcp::TcpServer::TcpServer(io_service & service, boost::function<void(std::shared_ptr<boost::asio::ip::tcp::socket> psock)> accept_call_back, std::string ip, int port):\
	service_(service), accept_handler_(accept_call_back), server_(service_)
{
	LOG(LINFO, "initing server：ip=", ip, ":", port);

	static TCP_HANDLE record = 0;
	lock.lock();
	record++;
	if (record < 0)
		record = 0;
	handle_ = record;
	lock.unlock();

	boost::asio::ip::tcp::resolver resolver(service_);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(ip, std::to_string(port)).begin();

	boost::system::error_code ec;
	//打开连接
	server_.open(endpoint.protocol(), ec);
	if (ec)
	{
		LOG(LERROR, "打开连接错误", ec.value(), ec.message());
		handle_ = TCP_ERROR_SERVER_OPEN_ERROR;
		return;
	}
	//设置参数，地址可重用
	server_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
	if (ec)
	{
		LOG(LERROR, "设置参数，地址可重用错误", ec.value(), ec.message());
		//TCP_ERROR_SERVER_SET_ERROR
		handle_ = TCP_ERROR_SERVER_SET_ERROR;
		return;
	}
	//绑定地址
	server_.bind(endpoint, ec);
	if (ec)
	{
		LOG(LERROR, "绑定地址错误", ec.value(), ec.message());
		handle_ = TCP_ERROR_SERVER_BIND_ERROR;
		return;
	}

	//监听
	server_.listen();
	is_run = true;
	//接收
	accept();

}
net_service::tcp::TcpServer::~TcpServer()
{
	is_run = false;
}

int net_service::tcp::TcpServer::stop()
{
	//退出系统
	boost::system::error_code ec;
	server_.cancel(ec);
	server_.close(ec);

	LOG(LINFO, "server close");
	return ec.value();
}

void net_service::tcp::TcpServer::accept()
{
	//是否在运行
	if (is_run)
	{
		LOG(LINFO, "Start Listening ");
		//创建客户端套接字
		sock_ptr client(new sock(service_));

		//异步接收连接
		server_.async_accept(*(client), [this, client](const boost::system::error_code& error)
		{
			if (error)
			{
				LOG(LERROR, error.value(), error.message());
			}
			else
			{
				if (is_run)
				{
					if (client->is_open())
					{
						LOG(LINFO, "获得连接：", client->remote_endpoint().address().to_string(), ":", \
							client->remote_endpoint().port());
						//协议栈异步处理连接
						if (accept_handler_ == nullptr)
						{
							LOG(LERROR, "accept_handler_ is a nullptr");
							return;
						}
						accept_handler_(client);
					}
					else
					{
						LOG(LINFO, "获得连接：套接字错误");
					}
				}
			}
			//重复接收
			accept();
		});
	}
}
