#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <vector>
#include <fstream>

#include "http_object.h"

namespace net_service
{
	namespace http
	{

		//http�����Ľ����������нṹ��
		class  HttpRequest:public HttpObject
		{
		public:
			HttpRequest();
			
			//����״̬ ��200 403 404
			void set_method(const std::string &value);
			std::string get_method();
			//����ԭ��
			void set_uri(const std::string &value);
			std::string get_uri();

			//����http�汾 http1.1 http1.0
			void set_version(const std::string &value);
			std::string  get_version();
		};
		typedef std::shared_ptr<net_service::http::HttpRequest> req_ptr;
		//typedef std::shared_ptr<net_service::http::HttpResponse> res_ptr;
	}
}

#endif //HTTP_REQUEST_H