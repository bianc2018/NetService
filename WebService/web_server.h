#ifndef WEB_SEVICE_H
#define WEB_SEVICE_H

#define WIN 1

#ifdef  WIN
#include <Windows.h>
#endif //  WIN32

#include <map>
#include <functional>
#include <vector>

#include "http_service.h"
#include "web_service.h"
#define SHARED_BUFF_PTR(size) std::shared_ptr<char>(new char[size], std::default_delete<char[]>())
typedef int(*P_WEN_HANDLER)(HTTP_HANDLE handle, int err);

namespace net_service
{
	namespace web
	{
		class WebServer
		{
			//����ָ�����
			typedef std::function<int(HTTP_HANDLE handle, int err)> WEB_HANDLER;
		public:
			//�ײ���ã����ڴ���һ������
			int http_handler(HTTP_HANDLE handle, int err_code);
			
			//��ʼ��
			WebServer(WebSeviceConfigData config);
			//�ر�
			~WebServer();

			//����������ص���uri��������ӣ�call_back �ص�������user_data �û����ݣ�����Ȩ��perm Ȩ�� perm ��� 0 ���ڲ��ɷ���
			int set_deal_with(const std::string&uri, WEB_HANDLER web_handler);

			//���ù��˺��� �������õ�˳�����
			//����֮ǰ����
			int set_before_filter(WEB_HANDLER call_back);
			//����֮�����
			int set_after_filter(WEB_HANDLER call_back);
			//����handler
			int load_handler(const std::string config_path = "./handler.json");
		public:

			//����
			WebSeviceConfigData config_data_;

		private:
			//Ĭ�ϻص�����
			int deal_default(HTTP_HANDLE handle,int err);

			//��������uri �ַ�����
			int call(const std::string& uri, HTTP_HANDLE handle, int err);

			void* get_func(const std::string &path,const std::string &name);

			//int set_res_mime(HTTP_HANDLE handle,const std::string& uri);
			//�����淵�� ""
			const std::string & try_page_cache(const std::string uri);
		private:
			//����ص�
			std::map<const std::string, WEB_HANDLER> func_map_;

			//������ //���˺��� ����0 ͨ������ 
			std::vector<WEB_HANDLER> before_filter_vec_;
			std::vector<WEB_HANDLER> after_filter_vec_;

			std::map<std::string, std::string> page_cache_map_;
#ifdef  WIN
			std::map<std::string, HINSTANCE> dll_map_;
#endif
			HTTP_HANDLE http_handle_;
		};
	}
}
#endif