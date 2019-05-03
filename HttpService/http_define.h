#ifndef HTTP_DEFINE_H
#define HTTP_DEFINE_H

#include <string>
#include <functional>

#define HTTP_SERVICE_API   __declspec( dllexport )
//共享指针参数
//typedef std::shared_ptr<net_service::http::HttpRequest> req_ptr;
//typedef std::shared_ptr<net_service::http::HttpResponse> res_ptr;
//句柄
typedef long long HTTP_HANDLE;
//回调
enum HTTP_ERROR_CODE
{
	HTTP_ERROR_CODE_OK = 0,
	//无效的http句柄
	HTTP_ERROR_CODE_INVALID_HANDLE = -100,
	//地址解析出错
	HTTP_ERROR_CODE_RESOLVER_ERROR = -101,
	//tcp服务端启动失败
	HTTP_ERROR_CODE_TCP_SERVER_ERROR = -102,
	//tcp客户端启动失败
	HTTP_ERROR_CODE_TCP_CLIENT_ERROR = -103,
	//请求不存在 exists
	HTTP_SET_REQ_NO_EXIST = -104,
	//回复不存在 exists
	HTTP_SET_RES_NO_EXIST = -105,
	//文件不存在 exists
	HTTP_SET_FIILE_NO_EXIST = -106,
	//取数据区间错误
	HTTP_SET_RANGE_ERROR = -107,
	//取数据
	HTTP_READ_FILE_ERROR = -108,
	HTTP_NOT_GET_FILE_LEN = -109,
};
//解析状态
enum PARSER_STATUS
{
	//解析到头部
	PARSER_HEAD = 0,
	//解析到内容content_
	PARSER_BODY,
	//解析完毕
	PARSER_OVER,
	//解析异常终止
	PARSER_ERROR = -1,
};
//服务回调
typedef std::function< int(HTTP_HANDLE handle, int err_code)> SERVER_HANDLER;
//数据返回
typedef std::function< int(HTTP_HANDLE handle, int err_code)> RESPONSE_HANDLER;

typedef std::pair<std::string, int> HTTP_ADDRESSS;

const std::string BODY_LEN("Content-Length");
const std::string CONTENT_TYPE("Content-Type");
const std::string HTML_MIME("text/html");
const std::string UNKNOW_REASON("UnKnow Code");

//UnKnow Code
#define HTTP_SERVICE_VERSION "1.0"
#endif //!HTTP_DEFINE_H

