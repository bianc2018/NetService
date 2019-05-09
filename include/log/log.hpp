#ifndef BASE_MOD_LOG
#define BASE_MOD_LOG


#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <mutex>
#include <sstream>
#include <memory>
#include <ctime>
#include <stdarg.h>

#include <boost/filesystem.hpp>
//��־ϵͳ
#define LFATAL 0
#define LERROR 1
#define LWARN  2
#define LINFO  3
#define LDEBUG 4
#define ALL 5
#define LV_SIZE 5
#define GLOBAL ""

using std::string;
const std::string SPLIT("/");
const std::string SPACE_ELE(" ");
const std::string WIN_SPLIT("\\");

#define _CRT_SECURE_NO_WARNINGS 1

class GLOG
{
public:
	GLOG(string name);
	//��ӡ��־
	template<typename ...Types>
	void print(const std::string &_class,int lv, Types && ... args)
	{
		
		//�жϵȼ��Ƿ����
		if (lv <= out_put_lv_&&lv>=LFATAL&&lv<=LDEBUG)
		{
			lock_.lock();
			//��ȡ�߳�id
			std::thread::id thread_id = std::this_thread::get_id();
			//��ʽ�����
			string message = format("[", thread_id, "][", get_now_time("%H:%M:%S"), "][", log_lv_[lv], "] ", format(std::forward<Types>(args)...));
			//��ӡ
			printf("%s\n", message.c_str());
			log_file_[lv] << message << std::endl;
			log_file_[ALL] << message << std::endl;
			lock_.unlock();
		}
		if (LFATAL == lv)
		{
			abort();
		}
	}
	//��ȡ��־ʵ��
	static std::shared_ptr<GLOG> get_log()
	{
		//����ģʽ
		if (nullptr == p_glog_)
			p_glog_.reset(new GLOG("glog"));
		boost::system::error_code ec;
		for (int i = 0; i <= LV_SIZE; i++)
		{
			auto log_size = boost::filesystem::file_size(p_glog_->file_path_[i],ec);
			if (ec)
			{
				printf("��ȡ�ļ���Сʧ�� %d,%s\n", ec.value(), ec.message().c_str());
				continue;
			}
			if (log_size >= p_glog_->block_size_)
			{
				p_glog_->init_log_file("glog",i);
			}
		}
		return p_glog_;
	}
	//����·��
	static void set_log_path(const string &path);
	//��������ȼ� LDEBUG ��ʾȫ�����
	static void set_log_lv(int lv);
	//���������ַ�
	static void set_fill(const string &_fill);
	//�����ļ�·����ȡ�ļ���
	static string get_file_name(const string &path);
	//��ʽ���ַ���
	template<typename T, typename ...Types>
	string format(T first, Types&& ... args)
	{
		//std::cout << "first " << first << std::endl;
		return data_to_string(first) + fill_ + format(std::forward<Types>(args)...);
		//std::cout << "first~ " << first << std::endl;
	}
	//��ȡ��ǰʱ��
	static string get_now_time();
	//��ȡ��ǰʱ�� ���� fmt ��ʽ
	static string get_now_time(const string &fmt);

	int init_log_file(const std::string &name,int lv=-1);
	
	void set_block_size(size_t k); //��λkb

	int dump(size_t size=1024*1024*1024);
	~GLOG();
private:
	std::fstream log_file_[LV_SIZE+1];
	string log_lv_[LV_SIZE + 1];
	string file_path_[LV_SIZE + 1];

	//int isdebug_;
	std::mutex lock_;
	static std::shared_ptr<GLOG> p_glog_;
	size_t block_size_; //log�ļ������size
	static string log_path_;
	
	static int out_put_lv_;
	static string fill_;

private:
	int open(const std::string &file_name, int lv);
	//�ַ�ת��
	string data_to_string(const int in);
	string data_to_string(const size_t in);
	string data_to_string(const long in);
	string data_to_string(const long long in);
	string data_to_string(const unsigned long in);
//	string data_to_string(const unsigned long long in);
	string data_to_string(const string in);
	string data_to_string(const double in);
	string data_to_string(const char* in);
	string data_to_string(const float in);
	string data_to_string(const std::thread::id&in);
	string data_to_string(const unsigned short&in);
	string data_to_string(const bool&in);
	
	
	
	string format()
	{
		return "";
	}
};
//��ӡ��־
#define LOG(LV,...)\
	LV<=LERROR ?\
	GLOG::get_log()->print(GLOBAL,LV,"[",GLOG::get_file_name(__FILE__),__LINE__,__FUNCTION__,"]",##__VA_ARGS__):\
	GLOG::get_log()->print(GLOBAL,LV,"[",__FUNCTION__,"]",##__VA_ARGS__)

#define SET_LOG_PATH(path) GLOG::set_log_path(path)
#define SET_OUTPUT_LV(lv) GLOG::get_log()->set_log_lv(lv)
#define SET_FILL(fill) GLOG::get_log()->set_fill(fill)
#define SET_BLOCK_SIZE(size) GLOG::get_log()->set_block_size(size)
#define DUMP() GLOG::get_log()->dump();
#endif // !BASE_MOD_LOG



