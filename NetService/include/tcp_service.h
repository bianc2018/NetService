#ifndef TCP_SERVICE_H
#define TCP_SERVICE_H

#include <string>

#include "tcp_define.h"

TCP_SERVICE_API void TCP_SERVICE_init(int thread_num=16);
TCP_SERVICE_API void TCP_SERVICE_uninit();

//����һ��������
TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_server(const std::string &ip, int port, ACCEPT_HANDLER accept_handler);

//���� �������Ӿ�� �ͻ���
TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_client(const std::string &ip,int port,int connect_time_out);

//��������
TCP_SERVICE_API void TCP_SERVICE_async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out);

//��������
TCP_SERVICE_API void TCP_SERVICE_async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out);

//�ر�һ������ ������
TCP_SERVICE_API void TCP_SERVICE_close_link(TCP_HANDLE handle);
TCP_SERVICE_API void TCP_SERVICE_close_server(TCP_HANDLE handle);

//���ò��� ֧�� key buff_size
TCP_SERVICE_API int TCP_SERVICE_set(const std::string key,int value);

#endif // !TCP_SERVICE_H


