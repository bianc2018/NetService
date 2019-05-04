#ifndef MUL_FROM_PARSER_H
#define MUL_FROM_PARSER_H

#include <string>

#include "json/json.h"
#include "http_service.h"

//传输的类型
const std::string TYPE("Content-Type");
//分割boundary
const std::string BOUNDARY_BEG("boundary=");
const std::string CRLF("\r\n");

class MultipartFormParser
{
public :
	static MultipartFormParser &instance();
	static int parser(HTTP_HANDLE handle);
	static int unparser(HTTP_HANDLE handle);
private:
	MultipartFormParser();
	std::string get_boundary(HTTP_HANDLE handle);
	int parser_param(const std::string&data, Json&out);
private:
	std::string form_cache_path_;
	size_t buff_size_;
};

#endif
