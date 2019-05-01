#include "tcp_link.h"

#include <mutex>

#include "../include/log/log.hpp"

std::mutex net_service::tcp::TcpLink::lock;

net_service::tcp::TcpLink::TcpLink(std::shared_ptr<boost::asio::ip::tcp::socket> sock):sock_(sock)
{
	static TCP_HANDLE record = 1;
	lock.lock();
	record++;
	if (record < 0)
		record = 1;
	handle_ = record;
	lock.unlock();
}

net_service::tcp::TcpLink::~TcpLink()
{
	//LOG(LDEBUG, "close timer", timers_.size());
	lock.lock();
	for (auto timer : timers_)
	{
		boost::system::error_code ec;
		//LOG(LDEBUG, "close timer");
		timer->cancel(ec);
	}
	//LOG(LDEBUG, "close timer in");
	timers_.clear();
	lock.unlock();
	//LOG(LDEBUG, "close timer out");
}

void net_service::tcp::TcpLink::async_recv(RECV_HANDLER recv_handler, int time_out,int buff_size)
{
	//���� ��buff
	auto buff_ptr = SHARED_BUFF_PTR(buff_size);
	//��ʼ��ʱ
	auto timer = start_timer(time_out, boost::bind(recv_handler, handle_, nullptr, 0, TCP_ERROR_CODE_RECV_TIME_OUT));
	//�������ݺ�Ļص�����
	auto read_handler = \
		[this,buff_ptr, recv_handler, timer](boost::system::error_code ec, std::size_t s)
	{
		stop_timer(timer);

		if (ec)
		{
			LOG(LERROR, "�����ݴ���", ec.value(), ec.message());
			recv_handler(handle_, nullptr, 0, TCP_ERROR_CODE_RECV_ERROR);
			return;
		}
		//�������´�����������Ȼ�����޷����ݸ�after_async_read
		auto n_handle_ = handle_;
		auto n_buff_ptr_ = buff_ptr;
		auto n_size_ = s;
		//��������
		recv_handler(n_handle_, n_buff_ptr_, n_size_,TCP_ERROR_CODE_OK);
		return;
	};
	
	//���ýӿڣ����첽�����ȡ���ݣ�һ�����ݾͷ���
	sock_->async_read_some(boost::asio::buffer(buff_ptr.get(), buff_size), read_handler);
	
}

void net_service::tcp::TcpLink::async_send(SEND_HANDLER send_handler, std::shared_ptr<char> buff_ptr, size_t len, int time_out, size_t begin)
{
	//��ʼ��ʱ
	auto timer = start_timer(time_out, boost::bind(send_handler,handle_, TCP_ERROR_CODE_SEND_TIME_OUT));
	//д�����ݺ�Ļص�����
	auto write_handler = \
		[this,len, begin, buff_ptr, send_handler,time_out,timer](boost::system::error_code ec, std::size_t s)
	{
		stop_timer(timer);

		if (ec)
		{
			LOG(LERROR, "д���ݴ���", ec.value(), ec.message());
			send_handler(handle_,TCP_ERROR_CODE_SEND_ERROR);
			return;
		}
		//д����
		if (len <= s)
		{
			//�����첽д,ȡ��һ��buff
			send_handler(handle_,TCP_ERROR_CODE_OK);
			return;
		}
		//δд��
		//����ʣ�µ��ֽ�
		int now_len = len - s;
		//����д
		async_send(send_handler, buff_ptr, now_len, time_out, begin + s);
		return;
	};

	sock_->async_write_some(boost::asio::buffer(buff_ptr.get() + begin, len), write_handler);
}

std::shared_ptr<boost::asio::ip::tcp::socket> net_service::tcp::TcpLink::get_sock_ptr()
{
	return sock_;
}


std::shared_ptr<boost::asio::steady_timer> net_service::tcp::TcpLink::start_timer(int time_out, boost::function<void()> call)
{

	//��ʱ��
	auto timer = std::make_shared<boost::asio::steady_timer>(sock_->get_io_context());
	
	//���ó�ʱ
	auto time_out_handler = \
		[this, call](boost::system::error_code ec)
	{
		//LOG(LWARN, "Tcp Link timer ��ʱ,handle_=", handle_,ec.value(),ec.message());
		if (!ec)
		{
			LOG(LWARN, "Tcp Link timer ��ʱ,handle_=", handle_);
			call();
		}
	};
	
	timer->expires_from_now(std::chrono::seconds(time_out));
	timer->async_wait(time_out_handler);
	lock.lock();
	timers_.push_back(timer);
	lock.unlock();
	return timer;
}

int net_service::tcp::TcpLink::stop_timer(std::shared_ptr<boost::asio::steady_timer> timer)
{
	boost::system::error_code ec;
	timer->cancel(ec);
	lock.lock();
	auto p = timers_.begin();
	for (; p != timers_.end(); p++)
	{
		if (*p == timer)
		{
			timers_.erase(p);
			break;
		}
	}
	lock.unlock();
	return ec.value();
}
