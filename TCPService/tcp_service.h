#ifndef TCP_SERVICE_H
#define TCP_SERVICE_H

#include <string>

#include "tcp_define.h"

TCP_SERVICE_API void TCP_SERVICE_init(std::string log_path="./log", int thread_num=16);
TCP_SERVICE_API void TCP_SERVICE_uninit();

//����һ��������
TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_server(const std::string &ip, int port, ACCEPT_HANDLER accept_handler, int accept_num = 4);

//���� �������Ӿ�� �ͻ���
TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_start_client(const std::string &ip,int port);

//��������
TCP_SERVICE_API void TCP_SERVICE_async_recv(TCP_HANDLE handle, RECV_HANDLER recv_handler, int time_out);

//��������
TCP_SERVICE_API void TCP_SERVICE_async_send(TCP_HANDLE handle, std::shared_ptr<char> data, int len, SEND_HANDLER send_handler, int time_out);

TCP_SERVICE_API void TCP_SERVICE_run();
//��ȡԶ���û���ip��port
TCP_SERVICE_API void TCP_SERVICE_get_remote_address(TCP_HANDLE handle,std::string&ip,int&port);
//��ȡ���ض˵�ip��port
TCP_SERVICE_API void TCP_SERVICE_get_local_address(TCP_HANDLE handle,std::string&ip, int&port);

//���ݷ��������server_handle ��ȡ���µ����� p_handle p_handle_lenҪ��ȡ����Ŀ all ȫ����������Ŀ ���� ʱ���ȡ����Ŀ
TCP_SERVICE_API int TCP_SERVICE_get_get_links_handle(TCP_HANDLE server_handle, TCP_HANDLE *p_handle,int p_handle_len,int* all=nullptr);
//�������Ӿ�� ��ȡ�����ķ�����
TCP_SERVICE_API TCP_HANDLE TCP_SERVICE_get_get_server_handle(TCP_HANDLE link_handle);

//�ر�һ������ ������
TCP_SERVICE_API void TCP_SERVICE_close_link(TCP_HANDLE handle);
TCP_SERVICE_API void TCP_SERVICE_close_server(TCP_HANDLE handle);

//���ò���
TCP_SERVICE_API int TCP_SERVICE_set_recv_buff(size_t value);

#endif // !TCP_SERVICE_H


