#ifndef TCP_SERVICE_IMPL_H
#define TCP_SERVICE_IMPL_H

#include <map>
#include <string>
#include <mutex>

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
			//启动
			int start();
			//停止
			int stop();
			//开启服务
			TCP_HANDLE start_server(const std::string &ip, int port, ACCEPT_HANDLER accept_handler,int accept_num = 4);
			//开启连接
			TCP_HANDLE start_client(const std::string &ip, int port);

			//接收数据
			void async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out);

			//接收数据
			void async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out);

			//get client address handle is err return ("",0)
			//获取远程地址
			std::pair<std::string, int> get_remote_address(TCP_HANDLE handle);
			//获取本地地址
			std::pair<std::string, int> get_local_address(TCP_HANDLE handle);
			
			//根据连接句柄获取服务器句柄
			TCP_HANDLE get_server_handle(TCP_HANDLE link);
			//获取服务器下的所有连接句柄
			std::vector<TCP_HANDLE> get_links_handle(TCP_HANDLE server);
			//设置参数
			void set_buff_size(size_t buff_size);
			void set_thread_num(size_t thread_num);
			void set_log_path(std::string log_path);
			void set_log_lv(int lv);
			//关闭
			void close_server(TCP_HANDLE handle);
			void close_client(TCP_HANDLE handle);
			//启动服务
			void run_service();
			//推送一个任务
			void post_task(TASK_HANDLER task);
			~TcpServiceImpl();
		private:
			TcpServiceImpl();
			
			//根据句柄获取实例
			std::shared_ptr<TcpLink> get_link(TCP_HANDLE handle);
			std::shared_ptr<TcpServer> get_server(TCP_HANDLE handle);

			//accept 接收连接
			void accept(TCP_HANDLE server_handle, ACCEPT_HANDLER accept_handler,std::shared_ptr<boost::asio::ip::tcp::socket> psock,int err);
			TCP_HANDLE start_link(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
		private:
			//服务器列表
			std::map<TCP_HANDLE, std::shared_ptr<TcpServer>> servers_;
			std::mutex lock_servers_;
			//连接列表
			std::map<TCP_HANDLE, std::shared_ptr<TcpLink>> links_;
			std::mutex lock_links_;
			//link<-->server
			std::map<TCP_HANDLE, TCP_HANDLE> link_server_;
			std::mutex lock_link_server_;
			//异步服务
			boost::asio::io_context  service_;
			//工作，使service在没有任务时继续运行异步服务；
			work_guard work_;
			//工作线程
			std::vector<run_thread_item> run_thread_vector_;
			
			//缓存区大小
			size_t buff_size_;
			size_t thread_num_;
			std::string log_path_;
			int log_lv_;
			std::atomic<bool> run_flag_;
		};
	}
}
#endif // !TCP_SERVICE_IMPL_H
