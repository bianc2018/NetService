#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

#include <string>
#include <vector>
namespace net_service
{
	namespace http
	{
		const size_t BODY_MAX_SIZE(1024 * 1024 * 10);
		const std::string BODY_CACHE_PATH("./cache/http_body_cache");
		//request 行头
		const int REQ_METHOD(0);
		const int REQ_URI(1);
		const int REQ_VERSION(2);
		//response 行头
		const int RES_VERSION(0);
		const int RES_STATUS_CODE(1);
		const int RES_REASON_PHRASE(2);//reason-phrase
		//换行
		const std::string CRLF("\r\n");
		//空格
		const std::string SPACE(" ");
		//
		const std::string REQUEST_SPLIT(": ");
		const std::string REQUEST_HEAD(" ");
		
		const std::string HTTP_VERSION("HTTP/1.1");
		const std::string BODY_LEN("Content-Length");
		

		//字符分割
		//字符分割结果
		typedef std::vector<std::string> split_vector;
		//字符串分割函数 考虑单独起来
		split_vector split(const std::string& s, const std::string&sub_split);
	}
}
#endif //!HTTP_UTIL_H

