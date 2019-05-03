#include "../http_service.h"

#include "http_service_impl.h"
#include "log/log.hpp"


std::string get_ext(const std::string&path, const std::string&notfond = "html")
{
	auto  p = path.find_last_of('.');
	if (std::string::npos == p)
	{
		return notfond;
	}
	return path.substr(p + 1);
}


HTTP_SERVICE_API void net_service::http::async_request(HTTP_HANDLE handle, RESPONSE_HANDLER response_handler)
{
	
}

HTTP_SERVICE_API void net_service::http::run()
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.run();
}

HTTP_SERVICE_API void net_service::http::close_link(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.close_link(handle);
}

HTTP_SERVICE_API void net_service::http::close_server(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.close_server(handle);
}

HTTP_SERVICE_API HTTP_ADDRESSS net_service::http::resolver_query(const std::string & url)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto addrs = impl.resolver_query(url);
	if (0 == addrs.size())
		return HTTP_ADDRESSS("", 0);
	return addrs[0];
}

HTTP_SERVICE_API void net_service::http::set_recv_buff_size(size_t size)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.set_recv_buff_size(size);
}

HTTP_SERVICE_API void net_service::http::set_send_buff_size(size_t size)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.set_send_buff_size(size);
}

HTTP_SERVICE_API void net_service::http::set_log_path(const std::string & path)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.set_log_path(path);
}

HTTP_SERVICE_API void net_service::http::set_thread_num(size_t size)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.set_thread_num(size);
}

HTTP_SERVICE_API HTTP_ADDRESSS net_service::http::get_client_address(HTTP_HANDLE handle)
{
	return net_service::tcp::get_remote_address(handle);
}

HTTP_SERVICE_API void net_service::http::set_time_out(size_t time_out)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.set_time_out(time_out);
}

HTTP_SERVICE_API int net_service::http::set_mime(const std::string & ext, const std::string & type)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	return impl.set_mime(ext, type);
}

HTTP_SERVICE_API std::string net_service::http::get_mime(const std::string & ext, const std::string & notfond)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	return impl.get_mime(ext, notfond);
}

HTTP_SERVICE_API int net_service::http::set_reason(const std::string & code, const std::string & reason_phrase)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	return impl.set_reason(code, reason_phrase);
}

HTTP_SERVICE_API std::string net_service::http::get_reason(const std::string & code, const std::string & notfond)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	return impl.get_reason(code, notfond);
}

HTTP_SERVICE_API void net_service::http::new_request(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.new_req(handle);

	//LOG(LINFO, handle, " new a req");
}

HTTP_SERVICE_API int net_service::http::request_set_body_len(HTTP_HANDLE handle, size_t size)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;

	req->set_body_len(size);
	return 0;
}

HTTP_SERVICE_API size_t net_service::http::request_get_body_len(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return 0;
	return req->get_body_len();
}

HTTP_SERVICE_API int net_service::http::request_set_method(HTTP_HANDLE handle, const std::string & method)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;

	req->set_method(method);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::request_get_method(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return "";
	return req->get_method();
}

HTTP_SERVICE_API int net_service::http::request_set_uri(HTTP_HANDLE handle, const std::string & uri)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;
	req->set_uri(uri);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::request_get_uri(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return "";
	return req->get_uri();
}

HTTP_SERVICE_API int net_service::http::request_set_version(HTTP_HANDLE handle, const std::string & version)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;
	req->set_version(version);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::request_get_version(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return "";
	return req->get_version();
}

HTTP_SERVICE_API int net_service::http::request_set_header(HTTP_HANDLE handle, const std::string & key, const std::string & value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;

	req->set_head_value(key, value);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::request_get_header(HTTP_HANDLE handle, const std::string & key, const std::string & notfond)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return notfond;

	return req->get_head_value(key,notfond);
}

HTTP_SERVICE_API int net_service::http::request_set_body(HTTP_HANDLE handle, const std::string & value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;

	req->set_body(value);
	return 0;
}

HTTP_SERVICE_API int net_service::http::request_set_ext_data(HTTP_HANDLE handle, const std::string & key, void * data)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;
	req->set_ext_data(key, data);
	return 0;
}

HTTP_SERVICE_API void * net_service::http::request_get_ext_data(HTTP_HANDLE handle, const std::string & key)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return nullptr;
	
	return req->get_ext_data(key);
}

HTTP_SERVICE_API int net_service::http::request_set_body_by_file(HTTP_HANDLE handle, const std::string & path, unsigned long long beg, long long end)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;
	
	req->set_head_value(CONTENT_TYPE, get_mime(get_ext(path)));
	return req->set_body_from_file(path,beg,end);
}

HTTP_SERVICE_API int net_service::http::request_get_body(HTTP_HANDLE handle,char* buff, unsigned long long beg, unsigned long long len)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;
	return req->get_body(buff,beg,len);
}

HTTP_SERVICE_API int net_service::http::request_set_body_cache(HTTP_HANDLE handle, size_t max_size, const std::string & cache_path)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto req = impl.get_req(handle);
	if (nullptr == req)
		return HTTP_SET_REQ_NO_EXIST;

	req->set_body_cache(max_size,cache_path);
	return 0;
}

HTTP_SERVICE_API void net_service::http::new_response(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	impl.new_res(handle);
	LOG(LINFO, handle, " new a res");
}

HTTP_SERVICE_API int net_service::http::response_set_body_len(HTTP_HANDLE handle, size_t size)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;
	res->set_body_len(size);
	return 0;
}

HTTP_SERVICE_API size_t net_service::http::response_get_body_len(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return 0;
	return res->get_body_len();
}

HTTP_SERVICE_API int net_service::http::response_set_version(HTTP_HANDLE handle, const std::string & value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;
	res->set_version(value);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::response_get_version(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return "";
	return res->get_version();
}

HTTP_SERVICE_API int net_service::http::response_set_status_code(HTTP_HANDLE handle, const std::string & value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;
	res->set_reason_phrase(get_reason(value));
	res->set_status_code(value);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::response_get_status_code(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return "";
	return res->get_status_code();
}

HTTP_SERVICE_API int net_service::http::response_set_reason_phrase(HTTP_HANDLE handle,const std::string &value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;

	res->set_reason_phrase(value);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::request_get_reason_phrase(HTTP_HANDLE handle)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return "";
	return res->get_reason_phrase();
}

HTTP_SERVICE_API int net_service::http::response_set_header(HTTP_HANDLE handle, const std::string & key, const std::string & value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;

	res->set_head_value(key, value);
	return 0;
}

HTTP_SERVICE_API std::string net_service::http::response_get_header(HTTP_HANDLE handle, const std::string & key, const std::string & notfond)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return notfond;
	return res->get_head_value(key,notfond);
}

HTTP_SERVICE_API int net_service::http::response_set_body(HTTP_HANDLE handle, const std::string & value)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;

	res->set_body(value);
	return 0;
}

HTTP_SERVICE_API int net_service::http::response_get_body(HTTP_HANDLE handle, char * buff, unsigned long long beg, unsigned long long len)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;
	return res->get_body(buff, beg, len);
}

HTTP_SERVICE_API int net_service::http::response_set_ext_data(HTTP_HANDLE handle, const std::string & key, void * data)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;

	res->set_ext_data(key, data);
	return 0;
}

HTTP_SERVICE_API void * net_service::http::response_get_ext_data(HTTP_HANDLE handle, const std::string & key)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return nullptr;
	return res->get_ext_data(key);
}


HTTP_SERVICE_API int net_service::http::response_set_body_by_file(HTTP_HANDLE handle, const std::string & path, unsigned long long beg , long long end )
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;
	
	res->set_head_value(CONTENT_TYPE, get_mime(get_ext(path)));
	return res->set_body_from_file(path, beg, end);
}

HTTP_SERVICE_API int net_service::http::response_set_body_cache(HTTP_HANDLE handle, size_t max_size, const std::string & cache_path)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	auto res = impl.get_res(handle);
	if (nullptr == res)
		return HTTP_SET_RES_NO_EXIST;

	res->set_body_cache(max_size, cache_path);
	return 0;
}

HTTP_SERVICE_API HTTP_HANDLE net_service::http::start_server(const std::string & ip, int port, SERVER_HANDLER http_handler, int accept_num)
{
	net_service::http::HttpServiceImpl &impl = net_service::http::HttpServiceImpl::instance();
	return impl.start_server(ip, port, http_handler, accept_num);
}
