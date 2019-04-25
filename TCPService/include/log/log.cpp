#include "log.hpp"


//初始化静态变量
string GLOG::log_path_=".";
int GLOG::out_put_lv_=LDEBUG;
string GLOG::fill_ = SPACE_ELE;
std::shared_ptr<GLOG> GLOG::p_glog_ = nullptr;

GLOG::GLOG(string name):block_size_(1024*1024*1)
{
	//std::cout << "GLOG 初始化\n";
	log_lv_[LFATAL] = "FATAL";
	log_lv_[LERROR] = "ERROR";
	log_lv_[LWARN] = "WARN";
	log_lv_[LINFO] = "INFO";
	log_lv_[LDEBUG] = "DEBUG";
	log_lv_[ALL] = "ALL";
	init_log_file(name);
	
}





void GLOG::set_log_path(const string & path)
{
	log_path_ = path;
}

void GLOG::set_log_lv(int lv)
{
	out_put_lv_ = lv;
}

void GLOG::set_fill(const string & _fill)
{
	fill_ = _fill;
}

string GLOG::get_file_name(const string & path)
{
	auto pos = path.find_last_of(SPLIT + WIN_SPLIT);
	if (string::npos == pos)
		return path;
	else
		return path.substr(pos+1);
}


int GLOG::init_log_file(const std::string & name, int lv)
{
	if (lv < 0 || lv > LV_SIZE)
	{
		for (int i = 0; i <=LV_SIZE; i++)
			open(name, i);
	}
	else
		open(name, lv);
	return 0;
}

void GLOG::set_block_size(size_t k)
{
	block_size_ = k * 1024;
}

#ifdef _WIN32
#include "Windows.h"
#include "DbgHelp.h"
#include "tchar.h"
#endif

int GLOG::dump(size_t size)
{
#ifdef _WIN32
	// 定义函数指针
	typedef BOOL(WINAPI * MiniDumpWriteDumpT)(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);
	// 从 "DbgHelp.dll" 库中获取 "MiniDumpWriteDump" 函数
	MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
	HMODULE hDbgHelp = LoadLibraryA("DbgHelp.dll");
	if (NULL == hDbgHelp)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

	if (NULL == pfnMiniDumpWriteDump)
	{
		FreeLibrary(hDbgHelp);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	// 创建 dmp 文件件
	TCHAR szFileName[MAX_PATH] = { 0 };
	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);
	wsprintf(szFileName, L"%04d%02d%02d-%02d%02d%02d.dmp",
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
	HANDLE hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if (INVALID_HANDLE_VALUE == hDumpFile)
	{
		FreeLibrary(hDbgHelp);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	// 写入 dmp 文件
	MINIDUMP_EXCEPTION_INFORMATION expParam;
	expParam.ThreadId = GetCurrentThreadId();
	expParam.ExceptionPointers = nullptr;
	expParam.ClientPointers = FALSE;
	pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
		hDumpFile, MiniDumpWithDataSegs, NULL, NULL, NULL);
	// 释放文件
	CloseHandle(hDumpFile);
	FreeLibrary(hDbgHelp);
	return EXCEPTION_EXECUTE_HANDLER;
#endif
}

GLOG::~GLOG()
{
	for(int i=0;i<=LV_SIZE;i++)
		log_file_[i].close();
}

string GLOG::get_now_time()
{
	time_t t = ::time(0);
	char ch[64];
	::strftime(ch, sizeof(ch), "%Y%m%d%H%M%S", ::localtime(&t)); //年-月-日 时-分-秒
	return string(ch);
}

string GLOG::get_now_time(const string & fmt)
{
	time_t t = ::time(0);
	char ch[1024];
	::strftime(ch, sizeof(ch), fmt.c_str(), ::localtime(&t)); //年-月-日 时-分-秒
	return string(ch);
}

int GLOG::open(const std::string & name, int lv)
{
	if (lv < 0 || lv > LV_SIZE)
	{
		return -2;
	}
	std::string data = get_now_time("%Y_%m_%d");
	std::string time = get_now_time("%H_%M_%S");
	std::string file_name = log_path_ + SPLIT+ log_lv_[lv] +SPLIT+ data + SPLIT;

	if (!boost::filesystem::exists(file_name))
	{
		boost::filesystem::create_directories(file_name);
	}

	file_name += name + "_" + time+ "." + log_lv_[lv];

	lock_.lock();
	//关闭旧文件
	if (log_file_[lv].is_open())
		log_file_[lv].close();

	log_file_[lv].open(file_name, std::ios::out);
	file_path_[lv] = file_name;
	lock_.unlock();
	if (!log_file_[lv])
	{
		print(GLOBAL, LERROR, "无法打开文件:", file_name.c_str());
		return -1;
	}
	return 0;
}

string GLOG::data_to_string(const int in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const size_t in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const long in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const long long in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const unsigned long in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const string in)
{
	return in;
}

string GLOG::data_to_string(const double in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const char * in)
{
	std::shared_ptr<char> buff(new char[1000]);
	sprintf(buff.get(), "%s", in);
	string str(buff.get());
	return str;
}

string GLOG::data_to_string(const float in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const std::thread::id & in)
{
	std::ostringstream os;
	os << in;
	return  os.str();
}

string GLOG::data_to_string(const unsigned short & in)
{
	return std::to_string(in);
}

string GLOG::data_to_string(const bool & in)
{
	if (true == in)
		return "true";
	return "false";
}
