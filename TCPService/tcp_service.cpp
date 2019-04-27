#include "tcp_service.h"

#include "src/tcp_service_impl.h"
#include "log/log.hpp"

TCP_SERVICE_API void TCP_SERVICE_init(std::string log_path, int thread_num)
{
	SET_LOG_PATH(log_path);
	SET_FILL(" ");
	SET_OUTPUT_LV(4);

	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.start(thread_num);


}

TCP_SERVICE_API void TCP_SERVICE_uninit()
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.stop();
}

TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_server(const std::string & ip, int port, ACCEPT_HANDLER accept_handler, int accept_num )
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	return impl.start_server(ip,port,accept_handler,accept_num);
}

TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_client(const std::string & ip, int port)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	return impl.start_client(ip,port);
}

TCP_SERVICE_API void TCP_SERVICE_async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.async_recv(handle,recv_handler,time_out);
}

TCP_SERVICE_API void TCP_SERVICE_async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.async_send(handle, data, len, send_handler, time_out);
}

void TCP_SERVICE_run()
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.run();
}

TCP_SERVICE_API void TCP_SERVICE_get_remote_address(TCP_HANDLE handle,std::string&ip, int&port)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	auto addr =  impl.get_remote_address(handle);
	ip = addr.first;
	port = addr.second;
}

TCP_SERVICE_API void TCP_SERVICE_get_local_address(TCP_HANDLE handle, std::string&ip, int&port)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	auto addr = impl.get_local_address(handle);
	ip = addr.first;
	port = addr.second;
}

int TCP_SERVICE_get_get_links_handle(TCP_HANDLE server_handle, TCP_HANDLE * p_handle, int p_handle_len, int * all)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	auto link_handles = impl.get_links_handle(server_handle);
	size_t size = link_handles.size();
	size_t return_len = p_handle_len > size ? size : p_handle_len;
	if (nullptr != all)
		*all = size;
	for (int i = 0; i < return_len; i++)
	{
		p_handle[i] = link_handles[i];
	}
	return return_len;
}

TCP_HANDLE TCP_SERVICE_get_get_server_handle(TCP_HANDLE link_handle)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	return impl.get_server_handle(link_handle);
}

TCP_SERVICE_API void TCP_SERVICE_close_link(TCP_HANDLE handle)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.close_client(handle);
}

TCP_SERVICE_API void TCP_SERVICE_close_server(TCP_HANDLE handle)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.close_server(handle);
}

int TCP_SERVICE_set_recv_buff(size_t value)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.set_buff_size(value);
	return 0;
}