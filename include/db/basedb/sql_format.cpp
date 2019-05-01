#include "sql_format.h"

std::string _class_attr(const std::string & var_name)
{
	auto pos = var_name.find_last_of(FIND_CLASS_ATTR);
	if (std::string::npos == pos)
		return var_name;

	return var_name.substr(pos + 1);
}

std::string replace(const std::string & src, const std::string & token, const std::string & rep)
{
	std::string dst;
	size_t pos = 0;
	do
	{
		auto token_pos = src.find(token, pos);
		dst += src.substr(pos, token_pos - pos);

		if (std::string::npos== token_pos)
			break;

		dst += rep;
		pos = token_pos + token.size();

	} while (pos<src.size());

	return dst;
}

std::string encode_json_value(const std::string & src)
{
	std::string tmp = src;

	tmp = replace(tmp, "\"", "&quot;");
	tmp = replace(tmp, "'", "&acute;");
	tmp = replace(tmp, "<", "&lt;");
	tmp = replace(tmp, ">", "&gt;");

	tmp = replace(tmp, "\\", "&bsol;");
	tmp = replace(tmp, "%", "&percnt;");

	return tmp;
}

std::string decode_json_value(const std::string & src)
{
	std::string tmp = src;

	tmp = replace(tmp, "&quot;", "\"");
	tmp = replace(tmp, "&acute;", "'");
	tmp = replace(tmp, "&lt;", "<");
	tmp = replace(tmp, "&gt;", ">");

	tmp = replace(tmp, " &amp;", "&");
	tmp = replace(tmp, "&bsol;", "\\");
	tmp = replace(tmp, "&percnt;", "%");

	return tmp;
}




std::string _sql_format(const std::string & fill)
{
	return std::string();
}

std::string time_to_string(time_t t, const std::string & fmt,const std::string &type)
{
	if (0 == t)
		t = ::time(0);

	char ch[1024];
	//本地数据
	if(type=="LOCAL")
		::strftime(ch, sizeof(ch), fmt.c_str(), ::localtime(&t)); 
	//格林时间
	else if(type=="GMT")
		::strftime(ch, sizeof(ch), fmt.c_str(), ::gmtime(&t));

	return std::string(ch);
}

