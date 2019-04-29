//�첽��tcp������

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <atomic>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/function.hpp>

#define BOOST_ASIO_NO_DEPRECATED
#include "../tcp_define.h"

namespace net_service
{
	namespace tcp
	{
		//����� p ip::tcp::v4()
		#define ipv4 boost::asio::ip::tcp::v4()
		//����
		typedef boost::asio::io_context io_service;
		//�׽���
		typedef boost::asio::ip::tcp::socket sock;
		typedef std::shared_ptr<sock> sock_ptr;
		//�˵�
		typedef boost::asio::ip::tcp::endpoint point;
		//��ַ ::address
		typedef boost::asio::ip::address address;
		//���� acceptor
		typedef boost::asio::ip::tcp::acceptor acceptor;

		//��ʾһ��������
		class TcpServer :public boost::noncopyable
		{
		public:
			TcpServer(io_service &service,boost::function<void(TCP_HANDLE handle,std::shared_ptr<boost::asio::ip::tcp::socket> psock,int err)> accept_call_back, std::string ip, int port,int accept_num=4);
			
			int stop();

			~TcpServer();
		public:
			//���
			TCP_HANDLE handle_;
		private:
			void accept();
		private:
			
			//�첽����
			io_service &service_;
			//��������
			acceptor server_;
			//�ص�
			boost::function<void(TCP_HANDLE handle, std::shared_ptr<boost::asio::ip::tcp::socket> psock,int err)> accept_handler_;
			//is_run 1 run 0 stop
			std::atomic<bool> is_run;
		};
	}
}
#endif
