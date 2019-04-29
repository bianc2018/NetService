#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <map>
#include <vector>
#include <fstream>

#include "../http_define.h"
#include "http_object.h"

namespace net_service
{
	namespace http
	{
		class HttpResponse:public HttpObject
		{
		public:
			HttpResponse();
			//����http�汾 http1.1 http1.0
			void set_version(const std::string &value);
			std::string  get_version();
			//����״̬ ��200 403 404
			void set_status_code(const std::string &value);
			std::string get_status_code();
			//����ԭ��
			void set_reason_phrase(const std::string &value);
			std::string get_reason_phrase();

		};
		typedef std::shared_ptr<HttpResponse> res_ptr;
	}
}
#endif //HTTP_RESPONSE_H
