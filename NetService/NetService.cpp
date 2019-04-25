#include "include/tcp_service.h"
//回调函数

//接收完数据 返回
void recv(TCP_HANDLE handle, std::shared_ptr<char> recv_data, int recv_len)
{
	std::string buff(recv_data.get(), recv_len);
	printf("recv handle data=%s\n", buff.c_str());
}
//发送完数据
void send(TCP_HANDLE handle)
{
	printf("send handle %d ", handle);
}

//接收到连接
void accept(TCP_HANDLE handle)
{
	printf("accept handle %d\n", handle);
	TCP_SERVICE_async_recv(handle, recv, 5);
}

int main()
{
	TCP_SERVICE_init(16);

	TCP_HANDLE server = TCP_SERVICE_start_server("127.0.0.1", 8000, accept);

	TCP_HANDLE client = TCP_SERVICE_start_client("127.0.0.1", 8000,5);

	auto buff = SHARED_BUFF_PTR(200);
	std::string len = "hello world!";
	memcpy(buff.get(), len.c_str(), 200);
	TCP_SERVICE_async_send(client, buff, len.size(), send, 5);
	TCP_SERVICE_uninit();
}