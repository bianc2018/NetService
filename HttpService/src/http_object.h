#ifndef HTTP_OBJECT_H
#define HTTP_OBJECT_H

#include <map>
#include <vector>
#include <fstream>

#include "../http_define.h"
#include "tcp_define.h"
#include "json/json.h"
#include "util.h"
namespace net_service
{
	namespace http
	{
		typedef unsigned long long POS;
		class HttpObject
		{
		public:
			HttpObject();
			~HttpObject();

			int parser(char *buff, POS size);

			//设置http版本行首
			void set_beg_line(int index ,const std::string &value);
			std::string  get_beg_line(int index);
			
			//设置回复报文的报文头
			void set_head_value(const std::string &key, const std::string &value);
			std::string get_head_value(const std::string &key, const std::string &notfond = "");

			//设置文件内容为报文内容
			int set_body_from_file(const std::string & path, POS beg, long long end);

			//直接写字节到报文
			void set_body(const std::string& data);
			int get_body(char *buff, POS beg, POS len);

			int get_status();

			//获取报文内容
			int get_content(char *buff, POS beg, POS len);

			void set_body_cache(size_t max_size, const std::string &cache_path = "./http_cache");

			// 拓展数据
			void set_ext_data(const std::string &key, void*data);
			void* get_ext_data(const std::string &key);
		private:
			//获取根据http协议构建的报文头
			std::string get_head();
			int parser_head(const std::string &str_head);
			int parser_body(std::string &data);
		private:
			int status_;
			std::string parser_cache_;//解析缓存
			long long parser_body_len_;
			//报文首部 version_ + SPACE + status_code_ + SPACE + reason_ + CRLF;
			std::string beg_line_[3];

			//结构化的报文头 key:value
			std::map<std::string, std::string> header_;

			//报文内容
			bool is_cache_;//false
			std::string body_;
			//缓存路径
			std::string cache_file_path_;
			//范围从文件读取
			std::pair<long long, long long> range_;

			//常数
			std::string cache_path_;
			size_t max_size_;

			//额外拓展的数据 不参与报文的组建
			std::map<const std::string, void *>ext_data_map_;
		};
	}
}
#endif //HTTP_OBJECT_H
