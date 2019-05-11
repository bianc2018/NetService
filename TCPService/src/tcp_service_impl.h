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
			//����
			int start();
			//ֹͣ
			int stop();
			//��������
			TCP_HANDLE start_server(const std::string &ip, int port, ACCEPT_HANDLER accept_handler,int accept_num = 4);
			//��������
			TCP_HANDLE start_client(const std::string &ip, int port);

			//��������
			void async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out);

			//��������
			void async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out);

			//get client address handle is err return ("",0)
			//��ȡԶ�̵�ַ
			std::pair<std::string, int> get_remote_address(TCP_HANDLE handle);
			//��ȡ���ص�ַ
			std::pair<std::string, int> get_local_address(TCP_HANDLE handle);
			
			//�������Ӿ����ȡ���������
			TCP_HANDLE get_server_handle(TCP_HANDLE link);
			//��ȡ�������µ��������Ӿ��
			std::vector<TCP_HANDLE> get_links_handle(TCP_HANDLE server);
			//���ò���
			void set_buff_size(size_t buff_size);
			void set_thread_num(size_t thread_num);
			void set_log_path(std::string log_path);
			void set_log_lv(int lv);
			//�ر�
			void close_server(TCP_HANDLE handle);
			void close_client(TCP_HANDLE handle);
			//��������
			void run_service();
			//����һ������
			void post_task(TASK_HANDLER task);
			~TcpServiceImpl();
		private:
			TcpServiceImpl();
			
			//���ݾ����ȡʵ��
			std::shared_ptr<TcpLink> get_link(TCP_HANDLE handle);
			std::shared_ptr<TcpServer> get_server(TCP_HANDLE handle);

			//accept ��������
			void accept(TCP_HANDLE server_handle, ACCEPT_HANDLER accept_handler,std::shared_ptr<boost::asio::ip::tcp::socket> psock,int err);
			TCP_HANDLE start_link(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
		private:
			//�������б�
			std::map<TCP_HANDLE, std::shared_ptr<TcpServer>> servers_;
			std::mutex lock_servers_;
			//�����б�
			std::map<TCP_HANDLE, std::shared_ptr<TcpLink>> links_;
			std::mutex lock_links_;
			//link<-->server
			std::map<TCP_HANDLE, TCP_HANDLE> link_server_;
			std::mutex lock_link_server_;
			//�첽����
			boost::asio::io_context  service_;
			//������ʹservice��û������ʱ���������첽����
			work_guard work_;
			//�����߳�
			std::vector<run_thread_item> run_thread_vector_;
			
			//��������С
			size_t buff_size_;
			size_t thread_num_;
			std::string log_path_;
			int log_lv_;
			std::atomic<bool> run_flag_;
		};
	}
}
#endif // !TCP_SERVICE_IMPL_H
