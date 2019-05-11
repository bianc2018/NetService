#include "tcp_service_impl.h"

#include "../include/log/log.hpp"

net_service::tcp::TcpServiceImpl & net_service::tcp::TcpServiceImpl::instance()
{
	static TcpServiceImpl impl;
	return impl;
}

int net_service::tcp::TcpServiceImpl::start()
{
	SET_LOG_PATH(log_path_);
	SET_FILL(" ");
	SET_OUTPUT_LV(log_lv_);
	run_flag_ = true;
	//运行工作线程
	//run_thread_vector_.reserve(thread_num_);
	for (int i = 0; i < thread_num_; ++i)
	{
		//运行
		//run_thread_vector_.emplace_back(&boost::asio::io_context::run, &service_);
		run_thread_vector_.emplace_back(&TcpServiceImpl::run_service, this);
	}
	return 0;
}

int net_service::tcp::TcpServiceImpl::stop()
{
	run_flag_ = false;
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
	std::lock_guard<std::mutex> lock(lock_links_);
	auto pos = links_.find(handle);
	if (links_.end() == pos)
		return nullptr;

	return pos->second;
}

std::shared_ptr<net_service::tcp::TcpServer> net_service::tcp::TcpServiceImpl::get_server(TCP_HANDLE handle)
{
	std::lock_guard<std::mutex> lock(lock_servers_);
	auto pos = servers_.find(handle);
	if (servers_.end() == pos)
		return nullptr;

	return pos->second;
}

TCP_HANDLE net_service::tcp::TcpServiceImpl::start_server(const std::string & ip, int port, ACCEPT_HANDLER accept_handler, int accept_num)
{
	if (!run_flag_)
		start();

	//TcpServer(io_service &service,ACCEPT_CALL_BACK accept_call_back, std::string ip, int port);
	//启动一个服务器
	std::shared_ptr<TcpServer> server = \
		std::make_shared<TcpServer>(service_,boost::bind(&TcpServiceImpl::accept,this, _1, accept_handler,_2,_3), ip, port,accept_num);
	//记录
	std::lock_guard<std::mutex> lock(lock_servers_);
	servers_.insert(std::make_pair(server->handle_, server));
	return server->handle_;
}

TCP_HANDLE net_service::tcp::TcpServiceImpl::start_client(const std::string & ip, int port)
{
	if (!run_flag_)
		start();

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
		recv_handler(handle,nullptr,0, TCP_ERROR_LINK_HANDLE_ERROR);
		return ;
	}
	link->async_recv(recv_handler, time_out, buff_size_);
}

void net_service::tcp::TcpServiceImpl::accept(TCP_HANDLE server_handle, ACCEPT_HANDLER accept_handler, std::shared_ptr<boost::asio::ip::tcp::socket> psock, int err)
{
	auto handle = start_link(psock);
	std::lock_guard<std::mutex> lock(lock_link_server_);
	link_server_.insert(std::make_pair(handle, server_handle));
	LOG(LDEBUG, "accept ", server_handle, handle, ",all link=", links_.size());
	accept_handler(server_handle, handle,err);
}

TCP_HANDLE net_service::tcp::TcpServiceImpl::start_link(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
	std::shared_ptr<TcpLink> link = std::make_shared<TcpLink>(sock);

	std::lock_guard<std::mutex> lock(lock_links_);
	links_.insert(std::make_pair(link->handle_, link));
	return link->handle_;
}

void net_service::tcp::TcpServiceImpl::async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out)
{
	auto link = get_link(handle);
	if (nullptr == link)
	{
		send_handler(handle,TCP_ERROR_LINK_HANDLE_ERROR);
		return;
	}
	link->async_send(send_handler, data, len, time_out, 0);
}

std::pair<std::string, int> net_service::tcp::TcpServiceImpl::get_remote_address(TCP_HANDLE handle)
{
	auto link = get_link(handle);
	if(nullptr==link)
		return std::pair<std::string, int>("",0);
	std::string ip = link->get_sock_ptr()->remote_endpoint().address().to_string();
	int port = link->get_sock_ptr()->remote_endpoint().port();
	return std::pair<std::string, int>(ip,port);
}

std::pair<std::string, int> net_service::tcp::TcpServiceImpl::get_local_address(TCP_HANDLE handle)
{
	auto link = get_link(handle);
	if (nullptr == link)
		return std::pair<std::string, int>("", 0);
	std::string ip = link->get_sock_ptr()->local_endpoint().address().to_string();
	int port = link->get_sock_ptr()->local_endpoint().port();
	return std::pair<std::string, int>(ip, port);
}

TCP_HANDLE net_service::tcp::TcpServiceImpl::get_server_handle(TCP_HANDLE link)
{
	std::lock_guard<std::mutex> lock(lock_link_server_);
	auto f = link_server_.find(link);
	if(link_server_.end()==f)
		return f->second;
	return TCP_ERROR_CODE_INVALID_HANDLE;
}

std::vector<TCP_HANDLE> net_service::tcp::TcpServiceImpl::get_links_handle(TCP_HANDLE server)
{
	std::vector<TCP_HANDLE> links;
	std::lock_guard<std::mutex> lock(lock_link_server_);
	for (auto it : link_server_)
	{
		if (it.second == server)
			links.push_back(it.first);
	}
	return links;
}

void net_service::tcp::TcpServiceImpl::set_buff_size(size_t buff_size)
{
	buff_size_ = buff_size;
}

void net_service::tcp::TcpServiceImpl::set_thread_num(size_t thread_num)
{
	thread_num_ = thread_num;
}

void net_service::tcp::TcpServiceImpl::set_log_path(std::string log_path)
{
	log_path_ = log_path;
}

void net_service::tcp::TcpServiceImpl::set_log_lv(int lv)
{
	log_lv_ = lv;
}

void net_service::tcp::TcpServiceImpl::close_server(TCP_HANDLE handle)
{
	//停止服务
	auto p = servers_.find(handle);
	p->second->stop();

	auto links = get_links_handle(handle);
	
	{
		//关闭连接
		{
			std::lock_guard<std::mutex> lock1(lock_links_);
			for (auto link : links)
			{
				auto p = links_.find(link);
				if (p != links_.end())
					links_.erase(p);
			}
		}
		//去除联系
		{
			std::lock_guard<std::mutex> lock2(lock_link_server_);
			for (auto link : links)
			{
				auto p = link_server_.find(link);
				if (p != link_server_.end())
					link_server_.erase(p);
			}
		}
	}
	//关闭服务器
	{
		std::lock_guard<std::mutex> lock(lock_servers_);
		if (p != servers_.end())
			servers_.erase(p);
	}
}

void net_service::tcp::TcpServiceImpl::close_client(TCP_HANDLE handle)
{
	{
		std::lock_guard<std::mutex> lock1(lock_links_);
		auto p = links_.find(handle);
		if(p!=links_.end())
			links_.erase(p);
	}
	//LOG(LDEBUG, "close link");
	{
		std::lock_guard<std::mutex> lock2(lock_link_server_);
		auto p = link_server_.find(handle);
		if (p != link_server_.end())
			link_server_.erase(p);
	}
	LOG(LDEBUG, "close a link(",handle,") ,all link = ",links_.size());
}

net_service::tcp::TcpServiceImpl::~TcpServiceImpl()
{
	stop();
}

net_service::tcp::TcpServiceImpl::TcpServiceImpl() :\
work_(boost::asio::make_work_guard(service_)), buff_size_(4096), thread_num_(16), \
log_path_("./log"),run_flag_(false)
{
	
}

void net_service::tcp::TcpServiceImpl::run_service()
{
	LOG(LDEBUG, "run_service");
	service_.run();
	//LOG(LINFO, "run_service end");

}

void net_service::tcp::TcpServiceImpl::post_task(TASK_HANDLER task)
{
	service_.post(task);
}

