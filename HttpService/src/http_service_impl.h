#ifndef HTTP_SERVICE_IMPL_H
#define HTTP_SERVICE_IMPL_H

#include <map>
#include <string>

#include "../http_define.h"
#include "http_request.h"
#include "http_response.h"

#include "tcp_service.h"

namespace net_service
{
	namespace http
	{
		
		class HttpServiceImpl
		{
		public:
			static HttpServiceImpl &instance();

			HTTP_HANDLE start_server(const std::string &ip,int port, SERVER_HANDLER server_handler, int accept_num = 4);

			HTTP_HANDLE start_client(const std::string &ip, int port);
			int do_request(HTTP_HANDLE handle, RESPONSE_HANDLER response_handler);
			
			void close_server(HTTP_HANDLE);
			void close_link(HTTP_HANDLE);
			//设置
			void set_time_out(size_t time_out);
			void set_recv_buff_size(size_t recv_buff_size);
			void set_send_buff_size(size_t send_buff_size);
			void set_log_path(const std::string& log_path);
			void set_thread_num(size_t thread_num);
			void run();
			
			//get
			req_ptr new_req(HTTP_HANDLE handle);
			res_ptr new_res(HTTP_HANDLE handle);

			//void delete_req(HTTP_HANDLE handle);
			//void delete_res(HTTP_HANDLE handle);

			req_ptr get_req(HTTP_HANDLE handle);
			res_ptr get_res(HTTP_HANDLE handle);

			std::vector<std::pair<std::string, int>> resolver_query(const std::string &url);

			~HttpServiceImpl();
		private:
			HttpServiceImpl();
			//server
			void accept_handler(TCP_HANDLE server_handle, TCP_HANDLE link_handle,int err, SERVER_HANDLER server_handler);//接收到一个连接
			
			void recv_request_handler(TCP_HANDLE handle, std::shared_ptr<char> recv_data, int recv_len, int err, SERVER_HANDLER server_handler,req_ptr request);

			void send_response(TCP_HANDLE handle,res_ptr response,SERVER_HANDLER server_handler,size_t pos);
			
			void send_response_handler(TCP_HANDLE handle, int err, SERVER_HANDLER server_handler, res_ptr response,size_t pos);
			
			//req
			void recv_response_handler(TCP_HANDLE handle, std::shared_ptr<char> recv_data, int recv_len, int err, RESPONSE_HANDLER response_handler, res_ptr response);

			void send_request(TCP_HANDLE handle, RESPONSE_HANDLER response_handler, req_ptr request);

			void send_request_handler(TCP_HANDLE handle, int err, RESPONSE_HANDLER response_handler, req_ptr request);
			
		private:
			
			//time_out s 接收发送超时
			size_t time_out_;
			size_t send_buff_size_;
			//队列
			std::map<HTTP_HANDLE, req_ptr> req_map_;
			std::map<HTTP_HANDLE, res_ptr> res_map_;
		};
	}
}
#endif // !HTTP_SERVICE_IMPL_H    
