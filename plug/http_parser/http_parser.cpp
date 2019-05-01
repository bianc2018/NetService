#include "http_parser.h"
#include "parser_define.h"
#include "http_service.h"

using namespace net_service::http;

HTTP_PARSER_API int http_parser_before(unsigned long long handle)
{
	auto ret = uri_parser_before(handle);
	if (ret != 0)
		return ret;

	ret = cookie_parser_before(handle);
	if (ret != 0)
		return ret;
	return ret;
}

HTTP_PARSER_API int http_parser_after(unsigned long long handle)
{
	uri_parser_after(handle);
	cookie_parser_after(handle);
	return 0;
}

HTTP_PARSER_API int uri_parser_before(unsigned long long handle)
{
	auto uri_map = new std::map<std::string, std::string>();
	auto ret = 0;
	auto uri = request_get_uri(handle);
	//paramse
	split_vector uri_params;
	split_vector params;
	split_vector key_value;

	uri_params = split(uri, SPLIT_URI_PARAMS);
	if (2 == uri_params.size())
	{
		params = split(uri_params[1], SPLIT_PARAMS);

		for (auto param : params)
		{
			key_value = split(param, SPLIT_KEY_VALUE);
			if (1 == key_value.size())
				uri_map->operator[](key_value[0]) = "";
			else if (2 == key_value.size())
				uri_map->operator[](key_value[0]) = key_value[1];
		}
	}
	request_set_uri(handle, uri_params[0]);

	ret = request_set_ext_data(handle, "uri_map", uri_map);
	if (ret != 0)
	{
		delete uri_map;
		return -1;
	}
	return 0;
}

HTTP_PARSER_API int uri_parser_after(unsigned long long handle)
{
	std::map<std::string, std::string> *uri_map = (std::map<std::string, std::string> *)request_get_ext_data(handle, "uri_map");
	if (uri_map != nullptr)
		delete uri_map;
	return 0;
}

HTTP_PARSER_API int cookie_parser_before(unsigned long long handle)
{
	split_vector cookie_vec;
	split_vector key_value;

	auto cookies = new std::map<std::string, std::string>();
	auto set_cookies = new std::map<std::string, std::string>();

	auto cookie = request_get_header(handle, "Cookie", "");
	cookie_vec = split(cookie, ": ");

	for (auto param : cookie_vec)
	{
		key_value = split(param, "=");
		if (1 == key_value.size())
			cookies->insert(std::make_pair(key_value[0], ""));
		else if (2 == key_value.size())
			cookies->insert(std::make_pair(key_value[0], key_value[1]));
	}

	auto ret = response_set_ext_data(handle, "set_cookie_map", cookies);
	if (ret != 0)
	{
		delete cookies;
		delete set_cookies;
		return -1;
	}

	ret = request_set_ext_data(handle, "cookie_map", set_cookies);
	if (ret != 0)
	{
		delete cookies;
		delete set_cookies;
		return -2;
	}

	return 0;
}

HTTP_PARSER_API int cookie_parser_after(unsigned long long handle)
{
	std::map<std::string, std::string> *cookie_map = (std::map<std::string, std::string> *)request_get_ext_data(handle, "cookie_map");
	std::map<std::string, std::string> *set_cookie_map = (std::map<std::string, std::string> *)request_get_ext_data(handle, "set_cookie_map");
	if (nullptr != set_cookie_map)
	{
		//Set-Cookie
		std::string cookie_cache = "";
		bool is_first = true;
		for (auto cookie : *set_cookie_map)
		{
			if (is_first)
			{
				cookie_cache = cookie.first + "=" + cookie.second;
				is_first = false;
			}
			else
			{
				cookie_cache += SET_COOKIE + ":" + cookie.first + "=" + cookie.second;
			}
			cookie_cache += "; Expires=" + time_to_string(LOCAL_TIME + 86000, "%a,%d %b %Y %H:%M:%S GMT", "GMT");
			cookie_cache += "; Path=/\r\n";
		}

		response_set_header(handle, SET_COOKIE, cookie_cache);
	}

	if (nullptr != cookie_map)
		delete cookie_map;
	if (nullptr != set_cookie_map)
		delete set_cookie_map;
	return 0;
}
