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