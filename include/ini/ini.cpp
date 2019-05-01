#include "ini.h"

#include <iostream>

int Ini::patser(const std::string file_path)
{
	//boost::property_tree::ptree
	try
	{
		boost::property_tree::read_ini(file_path, root_);
	}
	catch (const boost::property_tree::ini_parser_error& e)
	{
		std::cout << "boost::property_tree::ini_parser_error," << e.what() << std::endl;
		return -1;
	}
	
	return 0;
}

int Ini::get_config_int(const std::string & tag, const std::string & key, int nofond)
{
	try
	{
		auto tag_setting = root_.get_child(tag);

		return tag_setting.get<int>(key, nofond);
	}
	catch (const boost::property_tree::ptree_error& e)
	{
		std::cout << "boost::property_tree::ini_parser_error," << e.what() << std::endl;
		return nofond;
	}
	
}

std::string Ini::get_config_string(const std::string & tag, const std::string & key, std::string nofond)
{
	try
	{
		auto tag_setting = root_.get_child(tag);

		return tag_setting.get<std::string>(key, nofond);
	}
	catch (const boost::property_tree::ptree_error& e)
	{
		std::cout << "boost::property_tree::ini_parser_error," << e.what() << std::endl;
		return nofond;
	}
	
}

double Ini::get_config_double(const std::string & tag, const std::string & key, double nofond)
{
	try
	{
		auto tag_setting = root_.get_child(tag);

		return tag_setting.get<double>(key, nofond);
	}
	catch (const boost::property_tree::ptree_error& e)
	{
		std::cout << "boost::property_tree::ini_parser_error," << e.what() << std::endl;
		return nofond;
	}
	
}
