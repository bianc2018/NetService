#ifndef TCP_LINK_H
#define TCP_LINK_H
#include "tcp_link.h"

#include <map>
#include <atomic>

#include <boost/bind.hpp>
#include "boost/function.hpp"
#include <boost/asio.hpp>

#include "../tcp_define.h"
namespace net_service
{
	namespace tcp
	{
		typedef void(*time_call_back)();

		class TcpLink
		{
		public:
			TcpLink(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
			~TcpLink();
			void async_recv(RECV_HANDLER recv_handler, int time_out, int buff_size= FRAME_SIZE);
			void async_send(SEND_HANDLER send_handler, std::shared_ptr<char> buff_ptr, size_t len, int time_out, size_t begin);

			std::shared_ptr<boost::asio::ip::tcp::socket> get_sock_ptr();
		public:
			TCP_HANDLE handle_;
		public:
			std::shared_ptr<boost::asio::steady_timer> start_timer(int time_out, boost::function<void()> call);
			int stop_timer(std::shared_ptr<boost::asio::steady_timer>);
		private:
			
			//Ì×½Ó×Ö
			std::shared_ptr<boost::asio::ip::tcp::socket> sock_;
		};
	}
}
#endif // !TCP_LINK_H
