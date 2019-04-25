#include "tcp_server.h"

#include <mutex>

#include "../include/log/log.hpp"

std::mutex lock;

net_service::tcp::TcpServer::TcpServer(io_service & service, boost::function<void(std::shared_ptr<boost::asio::ip::tcp::socket> psock)> accept_call_back, std::string ip, int port):\
	service_(service), accept_handler_(accept_call_back), server_(service_)
{
	LOG(LINFO, "initing server��ip=", ip, ":", port);

	static TCP_HANDLE record = 0;
	lock.lock();
	record++;
	if (record < 0)
		record = 0;
	handle_ = record;
	lock.unlock();

	boost::asio::ip::tcp::resolver resolver(service_);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(ip, std::to_string(port)).begin();

	boost::system::error_code ec;
	//������
	server_.open(endpoint.protocol(), ec);
	if (ec)
	{
		LOG(LERROR, "�����Ӵ���", ec.value(), ec.message());
		handle_ = TCP_ERROR_SERVER_OPEN_ERROR;
		return;
	}
	//���ò�������ַ������
	server_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
	if (ec)
	{
		LOG(LERROR, "���ò�������ַ�����ô���", ec.value(), ec.message());
		//TCP_ERROR_SERVER_SET_ERROR
		handle_ = TCP_ERROR_SERVER_SET_ERROR;
		return;
	}
	//�󶨵�ַ
	server_.bind(endpoint, ec);
	if (ec)
	{
		LOG(LERROR, "�󶨵�ַ����", ec.value(), ec.message());
		handle_ = TCP_ERROR_SERVER_BIND_ERROR;
		return;
	}

	//����
	server_.listen();
	is_run = true;
	//����
	accept();

}
net_service::tcp::TcpServer::~TcpServer()
{
	is_run = false;
}

int net_service::tcp::TcpServer::stop()
{
	//�˳�ϵͳ
	boost::system::error_code ec;
	server_.cancel(ec);
	server_.close(ec);

	LOG(LINFO, "server close");
	return ec.value();
}

void net_service::tcp::TcpServer::accept()
{
	//�Ƿ�������
	if (is_run)
	{
		LOG(LINFO, "Start Listening ");
		//�����ͻ����׽���
		sock_ptr client(new sock(service_));

		//�첽��������
		server_.async_accept(*(client), [this, client](const boost::system::error_code& error)
		{
			if (error)
			{
				LOG(LERROR, error.value(), error.message());
			}
			else
			{
				if (is_run)
				{
					if (client->is_open())
					{
						LOG(LINFO, "������ӣ�", client->remote_endpoint().address().to_string(), ":", \
							client->remote_endpoint().port());
						//Э��ջ�첽��������
						if (accept_handler_ == nullptr)
						{
							LOG(LERROR, "accept_handler_ is a nullptr");
							return;
						}
						accept_handler_(client);
					}
					else
					{
						LOG(LINFO, "������ӣ��׽��ִ���");
					}
				}
			}
			//�ظ�����
			accept();
		});
	}
}
