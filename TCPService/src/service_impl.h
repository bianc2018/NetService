#ifndef TCP_SERVICE_IMPL_H
#define TCP_SERVICE_IMPL_H

#include <map>
#include <string>

#include "tcp_link.h"
#include "tcp_server.h"

namespace net_service
{
	namespace tcp
	{
		typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;
		typedef std::thread run_thread_item;

		class TcpServiceImpl
		{
		public:
			static TcpServiceImpl &instance();
			int start(int thread_num);
			int stop();

			std::shared_ptr<TcpLink> get_link(TCP_HANDLE handle);
			std::shared_ptr<TcpServer> get_server(TCP_HANDLE handle);

			TCP_HANDLE start_server(const std::string &ip, int port, ACCEPT_HANDLER accept_handler);

			TCP_HANDLE start_client(const std::string &ip, int port);

			//接收数据
			void async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out);

			//接收数据
			void async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out);

			void set_buff_size(size_t buff_size);

			void close_server(TCP_HANDLE handle);

			void close_client(TCP_HANDLE handle);

			~TcpServiceImpl();
		private:
			TcpServiceImpl();
			void run();
			//accept 接收连接
			void accept(ACCEPT_HANDLER accept_handler,std::shared_ptr<boost::asio::ip::tcp::socket> psock);
			TCP_HANDLE start_link(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
		private:
			//服务器列表
			std::map<TCP_HANDLE, std::shared_ptr<TcpServer>> servers_;
			//连接列表
			std::map<TCP_HANDLE, std::shared_ptr<TcpLink>> links_;
			//异步服务
			boost::asio::io_context  service_;
			//工作，使service在没有任务时继续运行异步服务；
			work_guard work_;
			//工作线程
			std::vector<run_thread_item> run_thread_vector_;
			//缓存区大小
			size_t buff_size_;
		};
	}
}
#endif // !TCP_SERVICE_IMPL_H
