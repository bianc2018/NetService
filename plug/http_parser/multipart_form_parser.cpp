#include "multipart_form_parser.h"

#include <boost/crc.hpp>
#include <boost/filesystem.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "log/log.hpp"
#include "ini/ini.h"
#include "parser_define.h"

#define SHARED_BUFF_PTR(size) std::shared_ptr<char>(new char[size], std::default_delete<char[]>())

MultipartFormParser & MultipartFormParser::instance()
{
	static MultipartFormParser impl;
	return impl;
	// TODO: 在此处插入 return 语句
}

int MultipartFormParser::parser(HTTP_HANDLE handle)
{
	MultipartFormParser &impl = MultipartFormParser::instance();
	//获取分隔符boundary
	std::string boundary = impl.get_boundary(handle);

	if ("" == boundary)
	{
		//LOG(LERROR, "找不到 boundary");
		return 0;
	}
	LOG(LDEBUG, "form parser begin",handle);
	std::string cache;
	
	size_t read_pos = 0;
	size_t body_len = net_service::http::request_get_body_len(handle);
	auto read_buff = SHARED_BUFF_PTR(impl.buff_size_+1);
	std::string b_boundary = "--" + boundary + CRLF;
	std::string e_boundary = "--" + boundary + "--";
	std::string param_boundary = CRLF+ CRLF;
	Json form_data;
	form_data.init_array();
	
	while (true)
	{
		//读取数据
		int read_len = net_service::http::request_get_body(handle, read_buff.get(), read_pos, impl.buff_size_);
		if (read_len < 0)
		{
			return PARSER_ERROR;
		}
		read_pos += read_len;
		cache += std::string(read_buff.get(), read_len);

		//开头
		auto b_pos = cache.find(b_boundary);

		//找到
		if (std::string::npos != b_pos)
		{
			LOG(LDEBUG, "find begin", handle);
			//找到标签
			Json form_it;
			cache = cache.substr(b_pos + b_boundary.size());
			
			do
			{
				//查找参数符
				auto pos = cache.find(param_boundary);
				if (pos != std::string::npos)
				{
					impl.parser_param(cache.substr(0,pos),form_it);
					cache = cache.substr(pos + param_boundary.size());
					LOG(LDEBUG, "find param_boundary", handle,"\n", form_it.to_string());
					break;
				}
				else//没找到而且已读完
				{
					if (body_len <= read_pos)
					{
						return PARSER_ERROR;
					}
				}

				//读取数据
				int read_len = net_service::http::request_get_body(handle, read_buff.get(), read_pos, impl.buff_size_);
				if (read_len < 0)
				{
					return PARSER_ERROR;
				}
				read_pos += read_len;
				cache += std::string(read_buff.get(), read_len);

			} while (true);

			//缓存本体
			auto file_name = form_it.get_string_value("Content-Disposition.filename", "");
			if ("" != file_name)
			{
				boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
				const string tmp_uuid = boost::uuids::to_string(a_uuid);
				const std::string cache_path = impl.form_cache_path_ + "/" + tmp_uuid + ".form_cache";
				std::fstream cache_file(cache_path,std::ios::out|std::ios::binary);
				if (!cache_file)
				{
					LOG(LERROR, "file don't open path=", cache_path);
					return PARSER_ERROR;
				}
				//开始
				do
				{
					auto pos = cache.find(b_boundary);
					if (std::string::npos == pos)
					{
						//结尾
						auto pos = cache.find(e_boundary);
						if (std::string::npos != pos)
						{
							std::string data = cache.substr(0, pos-CRLF.size());
							cache_file.write(data.c_str(), data.size());
							cache_file.close();

							form_it.add_object_value("is_cache", 1);
							form_it.add_object_value("path", cache_path);
							form_it.add_object_value("uuid", tmp_uuid);
							//tmp_uuid
							form_data.add_array_value(form_it);
							LOG(LDEBUG, "parser over", handle, "\n", form_data.to_string());
							std::string *str = new std::string(form_data.to_string());
							net_service::http::request_set_ext_data(handle, "form_json", str);

							return 0;
						}
						//两个都找不到
						else
						{
							cache_file.write(cache.c_str(), cache.size());
							cache = "";
						}
					}
					else
					{
						std::string data = cache.substr(0, pos- CRLF.size());
						cache_file.write(data.c_str(), data.size());
						cache = cache.substr(pos);
						cache_file.close();

						form_it.add_object_value("is_cache", 1);
						form_it.add_object_value("path", cache_path);
						form_it.add_object_value("uuid", tmp_uuid);
						form_data.add_array_value(form_it);
						LOG(LDEBUG, "parser ", handle, "\n", form_data.to_string());
						break;
					}

					if (body_len == read_len)
					{
						cache_file.close();
						boost::filesystem::remove(cache_path);
						LOG(LERROR, "找不到结尾");
						return PARSER_ERROR;
					}
					//读取数据
					int read_len = net_service::http::request_get_body(handle, read_buff.get(), read_pos, impl.buff_size_);
					if (read_len < 0)
					{
						return PARSER_ERROR;
					}
					read_pos += read_len;
					cache += std::string(read_buff.get(), read_len);
				} while (true);
			}
			else
			{
				std::string value;
				do
				{
					auto pos = cache.find(b_boundary);
					if (std::string::npos == pos)
					{
						//结尾
						auto pos = cache.find(e_boundary);
						if (std::string::npos != pos)
						{
							value += cache.substr(0, pos- CRLF.size());

							form_it.add_object_value("is_cache", 0);
							form_it.add_object_value("value", value);
							form_data.add_array_value(form_it);

							LOG(LDEBUG, "parser over", handle, "\n", form_data.to_string());

							std::string *str = new std::string(form_data.to_string());
							net_service::http::request_set_ext_data(handle, "form_json", str);

							return 0;
						}
						//两个都找不到
						else
						{
							value += cache;
							cache = "";
						}
					}
					else
					{
						value += cache.substr(0, pos- CRLF.size());

						form_it.add_object_value("is_cache", 0);
						form_it.add_object_value("value", value);
						form_data.add_array_value(form_it);

						LOG(LDEBUG, "parser ", handle, "\n", form_data.to_string());
						break;
					}

					if (body_len == read_len)
					{
						LOG(LERROR, "找不到结尾");
						return PARSER_ERROR;
					}
					//读取数据
					int read_len = net_service::http::request_get_body(handle, read_buff.get(), read_pos, impl.buff_size_);
					if (read_len < 0)
					{
						return PARSER_ERROR;
					}
					read_pos += read_len;
					cache += std::string(read_buff.get(), read_len);
				} while (true);
			}
		}
		else
		{
			if (body_len <= read_pos)
			{
				return PARSER_ERROR;
			}
		}
	}
}

int MultipartFormParser::unparser(HTTP_HANDLE handle)
{
	//std::string *str = new std::string(form_data.to_string());
	std::string *str = (std::string*)net_service::http::request_get_ext_data(handle, "form_json");
	if (nullptr != str)
	{
		Json form_data;
		form_data.from_string(*str);
		auto form = form_data.get_array();
		for (auto it : form)
		{
			if (1 == it.get_int_value("is_cache"))
			{
				//清除缓存文件
				auto filepath = it.get_string_value("path");
				if (boost::filesystem::exists(filepath))
				{
					boost::system::error_code ec;
					boost::filesystem::remove(filepath,ec);
					if (ec)
					{
						LOG(LERROR, "删除缓存失败", ec.value(), ec.message());
					}
				}
			}
		}
		delete str;
	}
	net_service::http::request_set_ext_data(handle, "form_json", nullptr);
	return 0;
}

MultipartFormParser::MultipartFormParser():form_cache_path_("./cache"),buff_size_(1024*1024)
{
	Ini ini;
	ini.parser("./config.ini");
	form_cache_path_ = ini.get_config_string("MultipartFormParser", "form_cache_path", "./cache");
	buff_size_ = ini.get_config_int("MultipartFormParser", "buff_size", 1024 * 1024);
	if (!boost::filesystem::exists(form_cache_path_))
	{
		boost::filesystem::create_directories(form_cache_path_);
	}
}

std::string MultipartFormParser::get_boundary(HTTP_HANDLE handle)
{
	auto type = net_service::http::request_get_header(handle, TYPE);
	auto pos = type.find(BOUNDARY_BEG);

	if (std::string::npos == pos)
		return "";
	return type.substr(pos + BOUNDARY_BEG.size());
}

int MultipartFormParser::parser_param(const std::string & data, Json & out)
{
	//Content - Disposition: form - data; name = "images"; filename = "/storage/emulated/0/Camera/jdimage/1xh0e3yyfmpr2e35tdowbavrx.jpg"
	//Content - Type: application / octet - stream
	//Content - Transfer - Encoding : binary
	LOG(LDEBUG, "params:\n", data);
	std::string param_boundary = CRLF;
	auto crlf_ver = split(data, CRLF);
	for (auto cf : crlf_ver)
	{
		auto kv = split(cf, ": ");

		if (kv.size() == 1)
			out.add_object_value(kv[0], "");
		else if (kv.size() == 2)
		{
			if ("Content-Disposition" == kv[0])
			{
				Json disposition;
				auto disposition_ver = split(kv[1], "; ");
				for (auto d : disposition_ver)
				{
					auto disposition_kv = split(d, "=");
					if(disposition_kv.size()==1)
						disposition.add_object_value(disposition_kv[0], "");
					if (disposition_kv.size() == 2)
					{
						//去掉两边的 “”
						disposition.add_object_value(disposition_kv[0], disposition_kv[1].substr(1, disposition_kv[1].size()-2));
					}
				}
				out.add_object_value(kv[0], disposition);
			}
			else
				out.add_object_value(kv[0], kv[1]);
		}
	}

	return 0;
}



