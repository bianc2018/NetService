#ifndef DB_TABLE_H
#define DB_TABLE_H

extern "C"
{
#include "../../sql/sqlite3.h"
};

#include <string>
#include <map>

class DBTable
{
public:
	DBTable();
	~DBTable();
	DBTable(int num_data,int num_key,char**m_data);
	std::string get_cell(int row,int col);
	std::string get_cell(int row, const std::string &col_name);

	int get_cell_num(int row, const std::string &col_name);
	int get_cell_num(int row, int col);
	//�� ����
	std::map<int,std::string> get_keys();

	int get_rows_num()
	{
		return num_data_;
	}
	int get_cols_num()
	{
		return num_key_;
	}
private:
	//����
	int num_data_;
	//����
	int num_key_;
	//����
	/*
	�б��
	a b c
	1 2 3
	4 5 6
	data_�������� mcol 3 row=2 num_key_*��row+1��+col+1
	a\0b\0c\01\02\03\04\05\06\0
	*/
	char**data_;

};

//�ӱ���л�ȡ�ַ����� ���tb ����name ����rows Ĭ��0
#define GET_CELL_0(tb,name) name = tb.get_cell(0, _class_attr(VAR(name)))
#define GET_CELL(tb,name,rows) name = tb.get_cell(rows, _class_attr(VAR(name)))
//�ӱ���л�ȡ�������� ���tb ����name ����rows Ĭ��0
#define GET_CELL_INT0(tb,name) name = tb.get_cell_num(0, _class_attr(VAR(name)))
#define GET_CELL_INT(tb,name,rows) name = tb.get_cell_num(rows, _class_attr(VAR(name)))
#endif //!BASE_DB_H