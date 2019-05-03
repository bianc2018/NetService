#include "http_object.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/filesystem.hpp>

#include "log/log.hpp"

net_service::http::HttpObject::HttpObject():\
is_cache_(false),max_size_(BODY_MAX_SIZE),cache_path_(BODY_CACHE_PATH),range_(-1,-1), parser_body_len_(0),\
status_(PARSER_HEAD), is_clear_(true)
{
	
}

net_service::http::HttpObject::~HttpObject()
{
	
	if (is_clear_)
	{
		boost::system::error_code ec;
		if (boost::filesystem::exists(cache_file_path_, ec))
		{
			boost::filesystem::remove(cache_file_path_, ec);
			if (ec)
			{
				LOG(LERROR, "清空数据失败,path=", cache_file_path_, ",ec(", ec.value(), ",", ec.message(), ")");

			}
		}
	}
	
}

int net_service::http::HttpObject::parser(char * buff, POS size)
{
	//没有数据进来，解析结束了
	if (size <= 0)
	{
		status_ = PARSER_OVER;
	}
	else
	{
		//缓存起来
		parser_cache_ += std::string(buff, size);
		//LOG(LDEBUG, "parser:\n", m_cache_);
		if (PARSER_HEAD == status_)
		{
			//查找两个换行符，报文头与内容的分割符
			auto pos = parser_cache_.find(CRLF + CRLF);

			if (std::string::npos == pos)
			{
				//找不到 继续
				status_ = PARSER_HEAD;
			}
			else
			{
				//找到了，解析
				std::string str_head = parser_cache_.substr(0, pos);
				parser_cache_ = parser_cache_.substr(pos + 2 * CRLF.size());

				if (parser_head(str_head) < 0)
				{
					status_ = PARSER_ERROR;
				}
				else
				{
					auto content_lenght = std::stoll(get_head_value(BODY_LEN, "0"));
					if (content_lenght != 0)
					{
						parser_body_len_ = content_lenght;
						if (content_lenght > max_size_)
						{
							//文件缓存
							is_cache_ = true;
							range_ = std::make_pair(0, content_lenght);
							//文件路径
							boost::system::error_code ec;
							if (!boost::filesystem::exists(cache_path_, ec))
							{
								//缓存路径不存在
								boost::filesystem::create_directories(cache_path_, ec);
								if (ec)
								{
									LOG(LERROR, "创建文件路径失败,path=", cache_path_, ",ec(", ec.value(), ",", ec.message(), ")");
									status_ = PARSER_OVER;
									return status_;
								}
							}
							// 这里是两个() ，因为这里是调用的 () 的运算符重载 uuid
							boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
							const std::string tmp_uuid = boost::uuids::to_string(a_uuid);
							//缓存文件
							cache_file_path_ = cache_path_ + "/" + tmp_uuid + ".http_service_cache";
							//如果存在 清空数据
							if (boost::filesystem::exists(cache_file_path_, ec))
							{
								boost::filesystem::remove(cache_file_path_, ec);
								if (ec)
								{
									LOG(LERROR, "清空数据失败,path=", cache_file_path_, ",ec(", ec.value(), ",", ec.message(), ")");
									status_ = PARSER_OVER;
									return status_;
								}
							}

						}
						//LOG(LINFO,"BODY",m_cache);
						std::string data = parser_cache_;
						parser_cache_ = "";
						status_ = parser_body(data);
					}
					else
					{
						status_ = PARSER_OVER;
					}
				}
			}
		}
		else if (PARSER_BODY == status_)
		{
			//LOG(LINFO,"BODY",m_cache);
			std::string data = parser_cache_;
			parser_cache_ = "";
			status_ = parser_body(data);
			//LOG(LINFO, data);
		}
	}
	//LOG(LDEBUG, "parser=", status_);
	return status_;
}

size_t net_service::http::HttpObject::get_body_len()
{
	auto str = get_head_value(BODY_LEN, "0");
	return std::stoull(str);
}

void net_service::http::HttpObject::set_body_len(size_t body_len)
{
	auto str = std::to_string(body_len);
	set_head_value(BODY_LEN, str);
}

void net_service::http::HttpObject::set_beg_line(int index, const std::string & value)
{
	if (index >= 0 && index <= 2)
		beg_line_[index] = value;
}

std::string net_service::http::HttpObject::get_beg_line(int index)
{
	if (index >= 0 && index <= 2)
		return beg_line_[index];

	return "";
}

void net_service::http::HttpObject::set_head_value(const std::string & key, const std::string & value)
{
	header_[key] = value;
}

std::string net_service::http::HttpObject::get_head_value(const std::string & key, const std::string & notfond)
{
	auto pos = header_.find(key);
	if (header_.end() == pos)
		return notfond;
	return pos->second;
}

int net_service::http::HttpObject::set_body_from_file(const std::string & path, POS beg, long long end)
{
	if (!boost::filesystem::exists(path))
		return HTTP_SET_FIILE_NO_EXIST;
	boost::system::error_code ec;
	//获取文件长度
	boost::uintmax_t content_length = boost::filesystem::file_size(path,ec);
	if (ec)
	{
		LOG(LERROR, "获取文件长度出错.ec=", ec.value(), ec.message());
		return HTTP_NOT_GET_FILE_LEN;
	}
	//检查 range
	if (end < 0)
		end = content_length - 1;
	if (beg > end)
		return HTTP_SET_RANGE_ERROR;

	//设置
	is_cache_ = true;
	is_clear_ = false;
	range_ = std::make_pair(beg, end);
	cache_file_path_ = path;
	set_head_value(BODY_LEN, std::to_string(end - beg + 1));

	return 0;
}

void net_service::http::HttpObject::set_body(const std::string & data)
{
	is_cache_ = false;
	range_ = std::make_pair(-1, -1);
	cache_file_path_ = "";
	set_head_value(BODY_LEN, std::to_string(data.size()));

	body_ = data;
}

int net_service::http::HttpObject::get_body(char * buff, POS beg, POS len)
{
	std::string data;
	if (is_cache_)
	{
		std::fstream file(cache_file_path_, std::ios::binary | std::ios::in);
		if (!file)
		{
			LOG(LERROR, "file not open! path=", cache_path_, ",code=", HTTP_SET_FIILE_NO_EXIST);
			return HTTP_SET_FIILE_NO_EXIST;
		}
		//范围检查
		boost::uintmax_t  file_length = boost::filesystem::file_size(cache_file_path_);
		//range_.first beg end range_.second
		
		
		if (beg + range_.first > file_length)
		{
			file.close();
			LOG(LERROR, "beg is out of range,beg=", beg, ",body_len=", get_head_value(BODY_LEN, "-1"));
			return HTTP_SET_RANGE_ERROR;
		}
		if (beg + len + range_.first > file_length)
		{
			//file.close();
			//LOG(LERROR, "end is out of range,beg=", beg, ",body_len=", get_head_value(BODY_LEN, "-1"));
			//return HTTP_SET_RANGE_ERROR;
			len = file_length - beg- range_.first;
		}
		//读数据
		file.seekg(beg + range_.first, std::ios::beg);
		file.read(buff, len);
		auto lenght = file.gcount();
		file.close();
		if (lenght < 0)
		{
			LOG(LERROR, "read file err ,path=", cache_file_path_, ",ret=", lenght);
			return HTTP_READ_FILE_ERROR;
		}
		return lenght;
	}
	else
	{
		if (beg > body_.size())
		{
			LOG(LERROR, "beg is out of range,beg=", beg, ",body_len=", get_head_value(BODY_LEN, "-1"));
			return HTTP_SET_RANGE_ERROR;
		}
		//截取数据
		auto data = body_.substr(beg);
		if (len > data.size())
			len = data.size();
		if (0 == len)
			return 0;
		memcpy(buff, data.c_str(), len);
		return len;
	}
}

int net_service::http::HttpObject::get_status()
{
	return status_;
}

int net_service::http::HttpObject::get_content(char * buff, POS beg, POS len)
{
	std::string head = get_head();
	//LOG(LINFO, "head\n", head);
	auto ptr = buff;
	int ret_head_len = 0;
	int  ret_get_body = 0;

	if (beg <= head.size())
	{
		if (beg + len <= head.size())
		{
			//只读取头部分
			std::string data = head.substr(beg, len);
			memcpy(buff, data.c_str(), data.size());
			return data.size();
		}

		//全部读取
		std::string data = head.substr(beg);
		ret_head_len = data.size();
		memcpy(buff, data.c_str(), ret_head_len);
		//重新定位
		ptr += ret_head_len;
		len -= ret_head_len;
		beg = 0;
	}
	else
		beg = beg - head.size();
	ret_get_body = get_body(ptr, beg, len);
	if (ret_get_body != 0)
		return ret_get_body;
	return ret_head_len + ret_get_body;
}

void net_service::http::HttpObject::set_body_cache(size_t max_size, const std::string & cache_path)
{
	max_size_ = max_size;
	cache_path_ = cache_path;
}

void net_service::http::HttpObject::set_ext_data(const std::string & key, void * data)
{
	ext_data_map_[key] = data;
}

void * net_service::http::HttpObject::get_ext_data(const std::string & key)
{
	auto pos = ext_data_map_.find(key);
	if (ext_data_map_.end() == pos)
		return nullptr;
	return pos->second;
}

std::string net_service::http::HttpObject::get_head()
{
	//组装报文头
	//添加头
	std::string head = beg_line_[0] + SPACE + beg_line_[1] + SPACE + beg_line_[2] + CRLF;

	//添加参数
	for (const auto& it : header_)
	{
		head += it.first + REQUEST_SPLIT + it.second + CRLF;
	}
	
	//换行
	head += CRLF;
	return head;
}

int net_service::http::HttpObject::parser_head(const std::string & str_head)
{
	//解析报文头 换行为分隔符
	split_vector line_ = split(str_head, CRLF);
	int line_size = line_.size();
	if (line_size <= 0)
		return PARSER_ERROR;

	//第一行为请求头，获取请求头
	split_vector head = split(line_[0], SPACE);
	if (head.size() < 3)
		return PARSER_ERROR;
	//方法，uri version
	set_beg_line(0, head[0]);
	//解析头
	set_beg_line(1, head[1]);
	set_beg_line(2, head[2]);

	//获取请求体，每一行为头数据
	for (int i = 1; i < line_size; i++)
	{
		split_vector params = split(line_[i], REQUEST_SPLIT);
		if (params.size() != 2)
			continue;
		set_head_value(params[0], params[1]);
	}
	return PARSER_BODY;
}

int net_service::http::HttpObject::parser_body(std::string & data)
{
	int status = PARSER_BODY;
	if (parser_body_len_ <= data.size())
	{
		data = data.substr(0, parser_body_len_);
		parser_body_len_ = 0;
		status = PARSER_OVER;
	}
	else
	{
		parser_body_len_ -= data.size();
	}

	if (is_cache_)
	{
		std::fstream file(cache_file_path_, std::ios::binary | std::ios::out | std::ios::app);
		if (!file)
		{
			LOG(LERROR, "not open this file,path=", cache_file_path_);
			return PARSER_ERROR;
		}
		file.write(data.c_str(), data.size());
		file.close();
	}
	else
	{
		body_ += data;
	}
	
	return status;
}
