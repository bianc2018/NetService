//������صĶ���
#ifndef TCP_DEFINE_H
#define TCP_DEFINE_H

#include <iostream>
#include <memory>
const int FRAME_SIZE(4096);

#define TCP_SERVICE_API  extern   "C"     __declspec( dllexport )
//���
typedef long long TCP_HANDLE;

//�ص�����
//���յ�����
typedef void(*ACCEPT_HANDLER)(TCP_HANDLE handle);
//���������� ����
typedef void(*RECV_HANDLER)(TCP_HANDLE handle, std::shared_ptr<char> recv_data, int recv_len);
//����������
typedef void(*SEND_HANDLER)(TCP_HANDLE handle);

//��̬����ָ��
#define SHARED_BUFF_PTR(size) std::shared_ptr<char>(new char[size], std::default_delete<char[]>())
#define SHARED_ANY_PTR(type,size) std::shared_ptr<type>(new type[size], std::default_delete<type[]>())

enum TCP_ERROR_CODE
{
	TCP_ERROR_CODE_OK=0,

	TCP_ERROR_CODE_RECV_ERROR = -1,
	TCP_ERROR_CODE_RECV_TIME_OUT = -2,

	TCP_ERROR_CODE_SEND_ERROR = -3,
	TCP_ERROR_CODE_SEND_TIME_OUT = -4,

	//�ͻ��� ����ʧ��
	TCP_ERROR_CLIENT_CONNECT_ERROR = -5,

	//����� ����ʧ��
	TCP_ERROR_SERVER_OPEN_ERROR = -6,
	//����� set_option ���ò�������
	TCP_ERROR_SERVER_SET_ERROR = -7,
	//����� �󶨵�ַʧ��
	TCP_ERROR_SERVER_BIND_ERROR = -8,

	//���Ӿ����Ч
	TCP_ERROR_LINK_HANDLE_ERROR = -9,
	//��������Ч
	TCP_ERROR_SERVER_HANDLE_ERROR = -9,
};
#endif 