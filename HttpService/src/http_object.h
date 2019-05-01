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

			//����http�汾����
			void set_beg_line(int index ,const std::string &value);
			std::string  get_beg_line(int index);
			
			//���ûظ����ĵı���ͷ
			void set_head_value(const std::string &key, const std::string &value);
			std::string get_head_value(const std::string &key, const std::string &notfond = "");

			//�����ļ�����Ϊ��������
			int set_body_from_file(const std::string & path, POS beg, long long end);

			//ֱ��д�ֽڵ�����
			void set_body(const std::string& data);
			int get_body(char *buff, POS beg, POS len);

			int get_status();

			//��ȡ��������
			int get_content(char *buff, POS beg, POS len);

			void set_body_cache(size_t max_size, const std::string &cache_path = "./http_cache");

			// ��չ����
			void set_ext_data(const std::string &key, void*data);
			void* get_ext_data(const std::string &key);
		private:
			//��ȡ����httpЭ�鹹���ı���ͷ
			std::string get_head();
			int parser_head(const std::string &str_head);
			int parser_body(std::string &data);
		private:
			int status_;
			std::string parser_cache_;//��������
			long long parser_body_len_;
			//�����ײ� version_ + SPACE + status_code_ + SPACE + reason_ + CRLF;
			std::string beg_line_[3];

			//�ṹ���ı���ͷ key:value
			std::map<std::string, std::string> header_;

			//��������
			bool is_cache_;//false
			std::string body_;
			//����·��
			std::string cache_file_path_;
			//��Χ���ļ���ȡ
			std::pair<long long, long long> range_;

			//����
			std::string cache_path_;
			size_t max_size_;

			//������չ������ �����뱨�ĵ��齨
			std::map<const std::string, void *>ext_data_map_;
		};
	}
}
#endif //HTTP_OBJECT_H
