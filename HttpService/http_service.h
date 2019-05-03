#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include <string>

#include "http_define.h"
namespace net_service
{
	namespace http
	{
		//HTTP_REQ_HANDLE
		//����һ��������
		HTTP_SERVICE_API HTTP_HANDLE start_server(const std::string &ip, int port, SERVER_HANDLER http_handler, int accept_num = 4);

		//������������
		HTTP_SERVICE_API void async_request(HTTP_HANDLE handle,RESPONSE_HANDLER response_handler);

		HTTP_SERVICE_API void run();
		//�ر�һ������ ������
		HTTP_SERVICE_API void close_link(HTTP_HANDLE handle);
		HTTP_SERVICE_API void close_server(HTTP_HANDLE handle);
		//resolver_query ����
		HTTP_SERVICE_API HTTP_ADDRESSS resolver_query(const std::string &url);
		//���ò���
		HTTP_SERVICE_API void set_recv_buff_size(size_t size);
		HTTP_SERVICE_API void set_send_buff_size(size_t size);
		HTTP_SERVICE_API void set_log_path(const std::string &path);
		HTTP_SERVICE_API void set_thread_num(size_t size);
		//��ȡip port
		HTTP_SERVICE_API HTTP_ADDRESSS get_client_address(HTTP_HANDLE handle);
		//s
		HTTP_SERVICE_API void set_time_out(size_t time_out);

		HTTP_SERVICE_API int set_mime(const std::string&ext,const std::string&type);
		HTTP_SERVICE_API std::string get_mime(const std::string&ext, const std::string&notfond=HTML_MIME);
		HTTP_SERVICE_API int set_reason(const std::string&code, const std::string&reason_phrase);
		HTTP_SERVICE_API std::string get_reason(const std::string&code, const std::string&notfond = UNKNOW_REASON);

		//����
		HTTP_SERVICE_API void new_request(HTTP_HANDLE handle);

		HTTP_SERVICE_API int request_set_body_len(HTTP_HANDLE handle,size_t size);
		HTTP_SERVICE_API size_t request_get_body_len(HTTP_HANDLE handle);

		HTTP_SERVICE_API int request_set_method(HTTP_HANDLE handle,const std::string &value);
		HTTP_SERVICE_API std::string request_get_method(HTTP_HANDLE handle);

		HTTP_SERVICE_API int request_set_uri(HTTP_HANDLE handle, const std::string &value);
		HTTP_SERVICE_API std::string request_get_uri(HTTP_HANDLE handle);

		HTTP_SERVICE_API int request_set_version(HTTP_HANDLE handle, const std::string &value);
		HTTP_SERVICE_API std::string request_get_version(HTTP_HANDLE handle);

		HTTP_SERVICE_API int request_set_header(HTTP_HANDLE handle, const std::string &key, const std::string &value);
		HTTP_SERVICE_API std::string request_get_header(HTTP_HANDLE handle, const std::string &key, const std::string &notfond = "");

		HTTP_SERVICE_API int request_set_body(HTTP_HANDLE handle, const std::string &value);
		
		//������չ����
		HTTP_SERVICE_API int request_set_ext_data(HTTP_HANDLE handle,const std::string &key, void * data);
		HTTP_SERVICE_API void* request_get_ext_data(HTTP_HANDLE handle, const std::string &key);

		//�����ļ�Ϊ����
		HTTP_SERVICE_API int request_set_body_by_file(HTTP_HANDLE handle, const std::string &path, unsigned long long beg = 0, long long end = -1);
		HTTP_SERVICE_API int request_get_body(HTTP_HANDLE handle, char* buff, unsigned long long beg, unsigned long long len);
		//���ý�������ʱ���󳬹�����ʱ,������body��������
		HTTP_SERVICE_API int request_set_body_cache(HTTP_HANDLE handle,size_t max_size, const std::string &cache_path="./http_cache/request");
		
		//�ظ�
		HTTP_SERVICE_API void new_response(HTTP_HANDLE handle);

		HTTP_SERVICE_API int response_set_body_len(HTTP_HANDLE handle, size_t size);
		HTTP_SERVICE_API size_t response_get_body_len(HTTP_HANDLE handle);

		HTTP_SERVICE_API int response_set_version(HTTP_HANDLE handle, const std::string &value);
		HTTP_SERVICE_API std::string response_get_version(HTTP_HANDLE handle);

		HTTP_SERVICE_API int response_set_status_code(HTTP_HANDLE handle, const std::string &value);
		HTTP_SERVICE_API std::string response_get_status_code(HTTP_HANDLE handle);

		HTTP_SERVICE_API int response_set_reason_phrase(HTTP_HANDLE handle, const std::string &value);
		HTTP_SERVICE_API std::string request_get_reason_phrase(HTTP_HANDLE handle);

		HTTP_SERVICE_API int response_set_header(HTTP_HANDLE handle, const std::string &key, const std::string &value);
		HTTP_SERVICE_API std::string response_get_header(HTTP_HANDLE handle, const std::string &key, const std::string &notfond = "");

		HTTP_SERVICE_API int response_set_body(HTTP_HANDLE handle, const std::string &value);
		HTTP_SERVICE_API int response_get_body(HTTP_HANDLE handle, char* buff, unsigned long long beg , unsigned long long len);
		
		HTTP_SERVICE_API int response_set_ext_data(HTTP_HANDLE handle, const std::string &key, void * data);
		HTTP_SERVICE_API void* response_get_ext_data(HTTP_HANDLE handle, const std::string &key);

		//�����ļ�Ϊ����[]
		HTTP_SERVICE_API int response_set_body_by_file(HTTP_HANDLE handle, const std::string &path, unsigned long long beg = 0, long long end = -1);
		//���ý�������ʱ���󳬹�����ʱ,������body��������
		HTTP_SERVICE_API int response_set_body_cache(HTTP_HANDLE handle, size_t max_size, const std::string &cache_path = "./http_cache/response");
	}
}
#endif // !HTTP_SERVICE_H


