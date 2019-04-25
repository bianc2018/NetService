#include "tcp_service.h"

#include "src/service_impl.h"

TCP_SERVICE_API void TCP_SERVICE_init(int thread_num)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.start(thread_num);
}

TCP_SERVICE_API void TCP_SERVICE_uninit()
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	impl.stop();
}

TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_server(const std::string & ip, int port, ACCEPT_HANDLER accept_handler)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();
	return impl.start_server(ip,port,accept_handler);
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

TCP_SERVICE_API int TCP_SERVICE_set(const std::string key, int value)
{
	net_service::tcp::TcpServiceImpl& impl = net_service::tcp::TcpServiceImpl::instance();

	if ("buff_size" == key)
	{
		impl.set_buff_size(value);
		return 0;
	}
	return -1;
}
