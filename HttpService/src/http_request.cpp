#include "http_request.h"

net_service::http::HttpRequest::HttpRequest()
{
	set_method("GET");
	set_uri("/");
	set_version(HTTP_VERSION);
}

void net_service::http::HttpRequest::set_method(const std::string & value)
{
	set_beg_line(REQ_METHOD, value);
}

std::string net_service::http::HttpRequest::get_method()
{
	return get_beg_line(REQ_METHOD);
}

void net_service::http::HttpRequest::set_uri(const std::string & value)
{
	set_beg_line(REQ_URI, value);
}

std::string net_service::http::HttpRequest::get_uri()
{
	return get_beg_line(REQ_URI);
}

void net_service::http::HttpRequest::set_version(const std::string & value)
{
	set_beg_line(REQ_VERSION, value);
}

std::string net_service::http::HttpRequest::get_version()
{
	return get_beg_line(REQ_VERSION);
}
