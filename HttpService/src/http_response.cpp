#include "http_response.h"

#include <boost/filesystem.hpp>

#include "log/log.hpp"

net_service::http::HttpResponse::HttpResponse()
{
	set_version(HTTP_VERSION);
	set_reason_phrase("OK");
	set_status_code("200");
	set_head_value("Content-Length", "0");
	set_head_value("Connection", "close");

	//LOG(LDEBUG, "new a res ");
}

void net_service::http::HttpResponse::set_status_code(const std::string & value)
{
	set_beg_line(RES_STATUS_CODE, value);
}

std::string net_service::http::HttpResponse::get_status_code()
{
	return get_beg_line(RES_STATUS_CODE);
}

void net_service::http::HttpResponse::set_reason_phrase(const std::string & value)
{
	set_beg_line(RES_REASON_PHRASE, value);
}

std::string net_service::http::HttpResponse::get_reason_phrase()
{
	return get_beg_line(RES_REASON_PHRASE);
}

void net_service::http::HttpResponse::set_version(const std::string & value)
{
	set_beg_line(RES_VERSION, value);
}

std::string net_service::http::HttpResponse::get_version()
{
	return get_beg_line(RES_VERSION);
}


