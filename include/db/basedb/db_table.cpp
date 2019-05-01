#include "db_table.h"
#include "../../log/log.hpp"

DBTable::DBTable()
{
	memset(this, 0, sizeof(DBTable));
}

DBTable::~DBTable()
{
	sqlite3_free_table(data_);
}

DBTable::DBTable(int num_data_, int num_key_, char ** data):\
num_data_(num_data_),num_key_(num_key_),data_(data)
{
	
}

string DBTable::get_cell(int row, int col)
{
	//看是否越界
	if (row >= 0 && row < num_data_ && col < num_key_&&col >= 0)
	{
		//计算
		char *it = data_[num_key_*(row + 1) + col];

		if (nullptr == it)
			return "";
		return string(it);
	}
	else
	{
		LOG(LERROR, "输入的行数不正确 ", "row (0,", num_data_, ")", "col(0,", num_key_, ")");
		return "";
	}
}

string DBTable::get_cell(int row, const string & col_name)
{
	for (int i = 0; i < num_key_; i++)
	{
		if (data_[i] == col_name)
		{
			return get_cell(row, i);
		}
	}
	LOG(LERROR, "键值", col_name, "不存在");
	return "";
}

int DBTable::get_cell_num(int row, const std::string & col_name)
{
	std::string cell = get_cell(row, col_name);
	if (cell == "")
		return 0;
	return std::stoi(cell);
}

int DBTable::get_cell_num(int row, int col)
{
	std::string cell = get_cell(row, col);
	if (cell == "")
		return 0;
	return std::stoi(cell);
}

std::map<int, string> DBTable::get_keys()
{
	std::map<int, string> keys;
	for (int i = 0; i < num_key_; i++)
	{
		keys.insert(std::make_pair(i, data_[i]));
	}
	return keys;
}
