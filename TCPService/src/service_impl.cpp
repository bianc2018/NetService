#include "service_impl.h"

#include "../include/log/log.hpp"

net_service::tcp::TcpServiceImpl & net_service::tcp::TcpServiceImpl::instance()
{
	static TcpServiceImpl impl;
	return impl;
}

int net_service::tcp::TcpServiceImpl::start(int thread_num)
{
	//运行工作线程
	run_thread_vector_.reserve(thread_num);
	for (int i = 0; i < thread_num; ++i)
	{
		//运行
		//run_thread_vector_.emplace_back(&boost::asio::io_context::run, &service_);
		run_thread_vector_.emplace_back(&TcpServiceImpl::run, this);
	}
	return 0;
}

int net_service::tcp::TcpServiceImpl::stop()
{
	service_.stop();
	//等待线程关闭
	for (auto &t : run_thread_vector_)
	{
		if(t.joinable())
		t.join();
	}
	return 0;
}

std::shared_ptr<net_service::tcp::TcpLink> net_service::tcp::TcpServiceImpl::get_link(TCP_HANDLE handle)
{
	auto pos = links_.find(handle);
	if (links_.end() == pos)
		return nullptr;

	return pos->second;
}

std::shared_ptr<net_service::tcp::TcpServer> net_service::tcp::TcpServiceImpl::get_server(TCP_HANDLE handle)
{
	auto pos = servers_.find(handle);
	if (servers_.end() == pos)
		return nullptr;

	return pos->second;
}


TCP_HANDLE net_service::tcp::TcpServiceImpl::start_server(const std::string & ip, int port, ACCEPT_HANDLER accept_handler)
{
	//TcpServer(io_service &service,ACCEPT_CALL_BACK accept_call_back, std::string ip, int port);
	std::shared_ptr<TcpServer> server = \
		std::make_shared<TcpServer>(service_,boost::bind(&TcpServiceImpl::accept,this, accept_handler,_1), ip, port);
	servers_.insert(std::make_pair(server->handle_, server));
	return server->handle_;
}

TCP_HANDLE net_service::tcp::TcpServiceImpl::start_client(const std::string & ip, int port)
{
	std::shared_ptr<boost::asio::ip::tcp::socket> sock = std::make_shared<boost::asio::ip::tcp::socket>(service_);

	boost::asio::ip::tcp::resolver resolver(service_);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(ip, std::to_string(port)).begin();
	boost::system::error_code ec;

	sock->connect(endpoint, ec);
	if (ec)
	{
		LOG(LERROR, "连接失败", ec.value(), ec.message());
		return TCP_ERROR_CLIENT_CONNECT_ERROR;
	}
	return start_link(sock);
}

void net_service::tcp::TcpServiceImpl::async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out)
{
	auto link = get_link(handle);
	if (nullptr == link)
	{
		recv_handler(TCP_ERROR_LINK_HANDLE_ERROR,nullptr,0);
		return ;
	}
	link->async_recv(recv_handler, time_out);
}

TCP_HANDLE net_service::tcp::TcpServiceImpl::start_link(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
	std::shared_ptr<TcpLink> link = std::make_shared<TcpLink>(sock);
	links_.insert(std::make_pair(link->handle_, link));
	return link->handle_;
}

void net_service::tcp::TcpServiceImpl::async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out)
{
	auto link = get_link(handle);
	if (nullptr == link)
	{
		send_handler(TCP_ERROR_LINK_HANDLE_ERROR);
		return;
	}
	link->async_send(send_handler, data, len, time_out, 0);
}

void net_service::tcp::TcpServiceImpl::set_buff_size(size_t buff_size)
{
	buff_size_ = buff_size;
}

void net_service::tcp::TcpServiceImpl::close_server(TCP_HANDLE handle)
{
	servers_.erase(handle);
}

void net_service::tcp::TcpServiceImpl::close_client(TCP_HANDLE handle)
{
	links_.erase(handle);
}

net_service::tcp::TcpServiceImpl::~TcpServiceImpl()
{
	stop();
}

net_service::tcp::TcpServiceImpl::TcpServiceImpl():work_(boost::asio::make_work_guard(service_)), buff_size_(4096)
{
	
}

void net_service::tcp::TcpServiceImpl::run()
{
	service_.run();
}

void net_service::tcp::TcpServiceImpl::accept(ACCEPT_HANDLER accept_handler, std::shared_ptr<boost::asio::ip::tcp::socket> psock)
{
	auto handle = start_link(psock);
	accept_handler(handle);
}
