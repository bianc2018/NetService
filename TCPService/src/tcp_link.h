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
		//��ʱ�ص�
		typedef void(*time_call_back)();

		//���ӣ�ÿһ������ʵ������һ������ʵ��

		class TcpLink
		{
		public:
			//��ʼ����������յ�������socket
			TcpLink(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
			//����
			~TcpLink();

			//�첽��������
			void async_recv(RECV_HANDLER recv_handler, int time_out, int buff_size= FRAME_SIZE);
			//�첽��������
			void async_send(SEND_HANDLER send_handler, std::shared_ptr<char> buff_ptr, size_t len, int time_out, size_t begin);

			//��ȡ���Ӷ�Ӧ���׽���
			std::shared_ptr<boost::asio::ip::tcp::socket> get_sock_ptr();
		public:
			//���
			TCP_HANDLE handle_;
		public:
			//����һ����ʱ��
			std::shared_ptr<boost::asio::steady_timer> start_timer(int time_out, boost::function<void()> call);
			//�ر�һ����ʱ��
			int stop_timer(std::shared_ptr<boost::asio::steady_timer>);
		private:
			
			//�׽���
			std::shared_ptr<boost::asio::ip::tcp::socket> sock_;
		};
	}
}
#endif // !TCP_LINK_H
