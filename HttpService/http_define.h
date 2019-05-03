#ifndef HTTP_DEFINE_H
#define HTTP_DEFINE_H

#include <string>
#include <functional>

#define HTTP_SERVICE_API   __declspec( dllexport )
//����ָ�����
//typedef std::shared_ptr<net_service::http::HttpRequest> req_ptr;
//typedef std::shared_ptr<net_service::http::HttpResponse> res_ptr;
//���
typedef long long HTTP_HANDLE;
//�ص�
enum HTTP_ERROR_CODE
{
	HTTP_ERROR_CODE_OK = 0,
	//��Ч��http���
	HTTP_ERROR_CODE_INVALID_HANDLE = -100,
	//��ַ��������
	HTTP_ERROR_CODE_RESOLVER_ERROR = -101,
	//tcp���������ʧ��
	HTTP_ERROR_CODE_TCP_SERVER_ERROR = -102,
	//tcp�ͻ�������ʧ��
	HTTP_ERROR_CODE_TCP_CLIENT_ERROR = -103,
	//���󲻴��� exists
	HTTP_SET_REQ_NO_EXIST = -104,
	//�ظ������� exists
	HTTP_SET_RES_NO_EXIST = -105,
	//�ļ������� exists
	HTTP_SET_FIILE_NO_EXIST = -106,
	//ȡ�����������
	HTTP_SET_RANGE_ERROR = -107,
	//ȡ����
	HTTP_READ_FILE_ERROR = -108,
	HTTP_NOT_GET_FILE_LEN = -109,
};
//����״̬
enum PARSER_STATUS
{
	//������ͷ��
	PARSER_HEAD = 0,
	//����������content_
	PARSER_BODY,
	//�������
	PARSER_OVER,
	//�����쳣��ֹ
	PARSER_ERROR = -1,
};
//����ص�
typedef std::function< int(HTTP_HANDLE handle, int err_code)> SERVER_HANDLER;
//���ݷ���
typedef std::function< int(HTTP_HANDLE handle, int err_code)> RESPONSE_HANDLER;

typedef std::pair<std::string, int> HTTP_ADDRESSS;

const std::string BODY_LEN("Content-Length");
const std::string CONTENT_TYPE("Content-Type");
const std::string HTML_MIME("text/html");
const std::string UNKNOW_REASON("UnKnow Code");

//UnKnow Code
#define HTTP_SERVICE_VERSION "1.0"
#endif //!HTTP_DEFINE_H

