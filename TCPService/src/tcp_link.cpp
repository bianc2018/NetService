#include "tcp_link.h"

#include <mutex>

#include "../include/log/log.hpp"

std::mutex net_service::tcp::TcpLink::lock;

net_service::tcp::TcpLink::TcpLink(std::shared_ptr<boost::asio::ip::tcp::socket> sock):sock_(sock)
{
	static TCP_HANDLE record = 0;
	lock.lock();
	record++;
	if (record < 0)
		record = 0;
	handle_ = record;
	lock.unlock();
}

net_service::tcp::TcpLink::~TcpLink()
{
	for (auto timer : timers_)
	{
		boost::system::error_code ec;
		timer->cancel(ec);
	}
	timers_.clear();
}

void net_service::tcp::TcpLink::async_recv(RECV_HANDLER recv_handler, int time_out,int buff_size)
{
	//申请 读buff
	auto buff_ptr = SHARED_BUFF_PTR(buff_size);
	//开始计时
	auto timer = start_timer(time_out, boost::bind(recv_handler, TCP_ERROR_CODE_RECV_TIME_OUT, nullptr, 0));
	//读到数据后的回调函数
	auto read_handler = \
		[this,buff_ptr, recv_handler, timer](boost::system::error_code ec, std::size_t s)
	{
		stop_timer(timer);

		if (ec)
		{
			LOG(LERROR, "读数据错误", ec.value(), ec.message());
			recv_handler(TCP_ERROR_CODE_RECV_ERROR, nullptr, 0);
			return;
		}
		//必须重新创建变量，不然数据无法传递给after_async_read
		auto n_handle_ = handle_;
		auto n_buff_ptr_ = buff_ptr;
		auto n_size_ = s;
		//处理数据
		recv_handler(n_handle_, n_buff_ptr_, n_size_);
		return;
	};
	
	//调用接口，从异步网络读取数据，一有数据就返回
	sock_->async_read_some(boost::asio::buffer(buff_ptr.get(), buff_size), read_handler);
	
}

void net_service::tcp::TcpLink::async_send(SEND_HANDLER send_handler, std::shared_ptr<char> buff_ptr, size_t len, int time_out, size_t begin)
{
	//开始计时
	auto timer = start_timer(time_out, boost::bind(send_handler, TCP_ERROR_CODE_SEND_TIME_OUT));
	//写完数据后的回调函数
	auto write_handler = \
		[this,len, begin, buff_ptr, send_handler,time_out,timer](boost::system::error_code ec, std::size_t s)
	{
		stop_timer(timer);

		if (ec)
		{
			LOG(LERROR, "写数据错误", ec.value(), ec.message());
			send_handler(TCP_ERROR_CODE_SEND_ERROR);
			return;
		}
		//写完了
		if (len <= s)
		{
			//返回异步写,取下一个buff
			send_handler(handle_);
			return;
		}
		//未写完
		//计算剩下的字节
		int now_len = len - s;
		//继续写
		async_send(send_handler, buff_ptr, now_len, time_out, begin + s);
		return;
	};

	sock_->async_write_some(boost::asio::buffer(buff_ptr.get() + begin, len), write_handler);
}


std::shared_ptr<boost::asio::steady_timer> net_service::tcp::TcpLink::start_timer(int time_out, boost::function<void()> call)
{

	//定时器
	auto timer = std::make_shared<boost::asio::steady_timer>(sock_->get_io_context());
	
	//设置超时
	auto time_out_handler = \
		[this, call](boost::system::error_code ec)
	{
		if (!ec)
		{
			LOG(LWARN, "Tcp Link timer 超时,handle_=", handle_);
			call();
		}
	};
	
	timer->expires_from_now(std::chrono::seconds(time_out));
	timer->async_wait(time_out_handler);
	timers_.push_back(timer);
	return timer;
}

int net_service::tcp::TcpLink::stop_timer(std::shared_ptr<boost::asio::steady_timer> timer)
{
	boost::system::error_code ec;
	timer->cancel(ec);

	auto p = timers_.begin();
	for (; p != timers_.end(); p++)
	{
		if (*p == timer)
		{
			timers_.erase(p);
			break;
		}
	}
	
	return ec.value();
}
