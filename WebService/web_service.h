#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <string>
#define WEB_SERVICE_API  __declspec( dllexport )
typedef void* WEB_HANDLE;
namespace net_service
{
	namespace web
	{
		//web��������
		struct WebSeviceConfigData
		{
			std::string ip;
			int port=8000;
			int accept_num;
			int timeout;
			size_t recv_buff_size;
			size_t send_buff_size;

			std::string mime_path;
			std::string reason_path;

			std::string log_path;
			int log_output = 1;
			std::string web_root;

		};
		WEB_SERVICE_API WEB_HANDLE start_server(WebSeviceConfigData config);

		/*
		{
			"http_service_version":"http��İ汾"��
			"handlers":
			[
				{
					"type":0-2 0 request ������ 1 �ص�����  2 ����
					"uri":"�����uri " type 0 2 ����Ҫ
					"func":"��������"
					"dll":"dll ·��"
				}
				//
			]
		}
		*/
		WEB_SERVICE_API int load_server_handler(WEB_HANDLE handle,const std::string&path="./handler.json");

		WEB_SERVICE_API void close_server(WEB_HANDLE handle);
	}
}

#endif // !TCP_SERVICE_H


