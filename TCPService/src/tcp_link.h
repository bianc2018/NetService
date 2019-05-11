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
		//超时回调
		typedef void(*time_call_back)();

		//连接，每一个对象实例代表一个连接实例

		class TcpLink
		{
		public:
			//初始化，传入接收到的连接socket
			TcpLink(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
			//解析
			~TcpLink();

			//异步接收数据
			void async_recv(RECV_HANDLER recv_handler, int time_out, int buff_size= FRAME_SIZE);
			//异步发送数据
			void async_send(SEND_HANDLER send_handler, std::shared_ptr<char> buff_ptr, size_t len, int time_out, size_t begin);

			//获取连接对应的套接字
			std::shared_ptr<boost::asio::ip::tcp::socket> get_sock_ptr();
		public:
			//句柄
			TCP_HANDLE handle_;
		public:
			//启动一个计时器
			std::shared_ptr<boost::asio::steady_timer> start_timer(int time_out, boost::function<void()> call);
			//关闭一个计时器
			int stop_timer(std::shared_ptr<boost::asio::steady_timer>);
		private:
			
			//套接字
			std::shared_ptr<boost::asio::ip::tcp::socket> sock_;
		};
	}
}
#endif // !TCP_LINK_H
