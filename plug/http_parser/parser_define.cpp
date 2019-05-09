#include "parser_define.h"


split_vector split(const std::string & s, const std::string & sub_split)
{
	split_vector ret;
	size_t pos = 0;
	//��֤������һ���ָ���
	std::string str = s + sub_split;
	while (true)
	{
		auto split_pos = str.find(sub_split, pos);
		if (std::string::npos == split_pos)
			break;
		//�ָ�
		ret.push_back(str.substr(pos, split_pos - pos));
		pos = split_pos + sub_split.size();
	}
	return ret;
}

std::string time_to_string(time_t t, const std::string & fmt, const std::string &type)
{
	if (0 == t)
		t = ::time(0);

	char ch[1024] = {0};
	//��������
	if (type == "LOCAL")
		::strftime(ch, sizeof(ch), fmt.c_str(), ::localtime(&t));
	//����ʱ��
	else if (type == "GMT")
		::strftime(ch, sizeof(ch), fmt.c_str(), ::gmtime(&t));

	return std::string(ch);
}