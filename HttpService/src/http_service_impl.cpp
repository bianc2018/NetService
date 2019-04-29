#include "http_service_impl.h"

#include "boost/bind.hpp"
#include "boost/asio.hpp"

#include "log/log.hpp"

net_service::http::HttpServiceImpl & net_service::http::HttpServiceImpl::instance()
{
	static HttpServiceImpl impl;
	return impl;
}


HTTP_HANDLE net_service::http::HttpServiceImpl::start_server(const std::string & ip, int port, SERVER_HANDLER server_handler, int accept_num)
{
	auto handle = tcp::start_server(ip, port,\
		std::bind(&HttpServiceImpl::accept_handler,this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,server_handler)\
		,accept_num);

	if (handle <= 0)
	{
		LOG(LERROR, "tcp server start err,tcp ret=", handle);
		return HTTP_ERROR_CODE_TCP_SERVER_ERROR;
	}
	return handle;
}

HTTP_HANDLE net_service::http::HttpServiceImpl::start_client(const std::string & ip, int port)
{
	auto handle = tcp::start_client(ip,port);
	if (handle <= 0)
	{
		LOG(LERROR, "tcp client start err,tcp ret=", handle);
		return HTTP_ERROR_CODE_TCP_CLIENT_ERROR;
	}
	return handle;
}

void net_service::http::HttpServiceImpl::close_server(HTTP_HANDLE handle)
{
	auto links = tcp::get_links_handle(handle);
	for (auto link : links)
	{
		req_map_.erase(link);
		res_map_.erase(link);
	}
	tcp::close_server(handle);
}

void net_service::http::HttpServiceImpl::close_link(HTTP_HANDLE handle)
{
	tcp::close_link(handle);
	req_map_.erase(handle);
	res_map_.erase(handle);
}

void net_service::http::HttpServiceImpl::set_time_out(size_t time_out)
{
	time_out_ = time_out;
}

void net_service::http::HttpServiceImpl::set_recv_buff_size(size_t recv_buff_size)
{
	tcp::set_recv_buff(recv_buff_size);
}

void net_service::http::HttpServiceImpl::set_send_buff_size(size_t send_buff_size)
{
	send_buff_size_ = send_buff_size;
}

void net_service::http::HttpServiceImpl::set_log_path(const std::string & log_path)
{
	tcp::set_log_path(log_path);
}

void net_service::http::HttpServiceImpl::set_thread_num(size_t thread_num)
{
	tcp::set_thread_num(thread_num);
}

void net_service::http::HttpServiceImpl::run()
{
	tcp::run();
}

net_service::http::req_ptr net_service::http::HttpServiceImpl::new_req(HTTP_HANDLE handle)
{
	auto request = std::make_shared<HttpRequest>();
	req_map_.insert(std::make_pair(handle, request));
	return 0;
}

net_service::http::res_ptr net_service::http::HttpServiceImpl::new_res(HTTP_HANDLE handle)
{
	auto response = std::make_shared<HttpResponse>();
	res_map_.insert(std::make_pair(handle, response));
	return response;
}

net_service::http::req_ptr net_service::http::HttpServiceImpl::get_req(HTTP_HANDLE handle)
{
	auto finded = req_map_.find(handle);
	if (req_map_.end() == finded)
		return nullptr;
	return finded->second;
}

net_service::http::res_ptr net_service::http::HttpServiceImpl::get_res(HTTP_HANDLE handle)
{
	auto finded = res_map_.find(handle);
	if (res_map_.end() == finded)
		return nullptr;
	return finded->second;
}

net_service::http::HttpServiceImpl::~HttpServiceImpl()
{
	
}

net_service::http::HttpServiceImpl::HttpServiceImpl() :time_out_(5)
{
	set_log_path("./log");

	set_recv_buff_size(1024 * 1024 * 20);

	set_send_buff_size(1024 * 1024 * 20);

	set_time_out(60);

	set_thread_num(16);
}

void net_service::http::HttpServiceImpl::accept_handler(TCP_HANDLE server_handle, TCP_HANDLE link_handle, int err, SERVER_HANDLER server_handler)
{
	if (TCP_ERROR_CODE_OK == err)
	{
		req_ptr req = new_req(link_handle);
		tcp::async_recv(link_handle, \
			std::bind(&HttpServiceImpl::recv_request_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, server_handler, req),\
			time_out_);
	}
	else
	{
		LOG(LERROR, "关闭一个连接 ret=", err);
		close_link(link_handle);
	}
}

void net_service::http::HttpServiceImpl::recv_request_handler(TCP_HANDLE handle, std::shared_ptr<char> recv_data, int recv_len, int err, SERVER_HANDLER server_handler, req_ptr request)
{
	if (TCP_ERROR_CODE_OK == err)
	{
		auto ret = request->parser(recv_data.get(), recv_len);

		//出错
		if (PARSER_ERROR == ret)
		{
			LOG(LERROR, "请求解析错误");
			close_link(handle);
			return;
		}
		//解析完毕，但未完结
		else if (PARSER_HEAD == ret || PARSER_BODY == ret)
		{
			tcp::async_recv(handle, \
				std::bind(&HttpServiceImpl::recv_request_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, server_handler, request), \
				time_out_);
			return;

		}//解析完毕，字节已接收完了
		else if (PARSER_OVER == ret)
		{
			auto response = new_res(handle);
			LOG(LINFO, "接收到一个请求,", handle);
			//默认短链接
			auto ka = request->get_head_value("Connection", "close");
			response->set_head_value("Connection", ka);

			if (nullptr == server_handler)
			{
				LOG(LERROR, "opt->server_handler is nullptr,link_handle=", handle);
				close_link(handle);
				return;
			}
			//处理
			server_handler(handle, HTTP_ERROR_CODE_OK);

			//发送
			send_response(handle, response,server_handler,0);
		}
	}
	else
	{
		LOG(LERROR, "关闭一个连接 ret=", err);
		close_link(handle);
	}
}

void net_service::http::HttpServiceImpl::send_response(TCP_HANDLE handle, res_ptr response, SERVER_HANDLER server_handler, size_t pos)
{
	//申请buff
	auto buff_ptr = SHARED_BUFF_PTR(send_buff_size_);
	//读数据
	int readlen = response->get_content(buff_ptr.get(),pos, send_buff_size_);
	pos += readlen;
	//读完
	if (0 == readlen)
	{
		if ("close" == response->get_head_value("Connection", "close"))
		{
			close_link(handle);
			return;
		}
		else
		{
			req_ptr req = new_req(handle);
			tcp::async_recv(handle, \
				std::bind(&HttpServiceImpl::recv_request_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, server_handler, req), \
				time_out_);
			return;
		}
	}
	else if (readlen < 0)//或出错
	{
		LOG(LERROR, "response err,code=", readlen);
		close_link(handle);
		return;
	}
	
	tcp::async_send(handle, buff_ptr, \
		readlen,std::bind(&HttpServiceImpl::send_response_handler,this,std::placeholders::_1, std::placeholders::_2,server_handler,response,pos),\
		time_out_);
}


void net_service::http::HttpServiceImpl::send_response_handler(TCP_HANDLE handle, int err, SERVER_HANDLER server_handler, res_ptr response, size_t pos)
{
	if (TCP_ERROR_CODE_OK == err)
	{
		send_response(handle, response,server_handler,pos);
	}
	else
	{
		LOG(LERROR, "关闭一个连接 ret=", err);
		close_link(handle);
	}
}


std::vector<std::pair<std::string, int>> net_service::http::HttpServiceImpl::resolver_query(const std::string & url)
{
	std::vector<std::pair<std::string, int>> addrs;

	boost::asio::io_service io_serv;
	boost::asio::ip::tcp::resolver::query q(url, "http");
	boost::asio::ip::tcp::resolver rslvr(io_serv);
	boost::asio::ip::tcp::resolver::iterator iter = rslvr.resolve(q);
	boost::asio::ip::tcp::resolver::iterator end;
	
	for (; iter != end; iter++)
	{
		auto ip = iter->endpoint().address().to_string();
		auto port = iter->endpoint().port();
		LOG(LDEBUG, "get ip=", ip, port, "from", url);
		addrs.push_back(std::make_pair(ip, port));
	}

	return addrs;
}
