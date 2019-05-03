#ifndef INI_CONFIG_H
#define INI_CONFIG_H
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp>
class Ini
{
public:
	int parser(const std::string file_path);
	
	int get_config_int(const std::string&tag, const std::string&key, int nofond=0);
	std::string get_config_string(const std::string&tag, const std::string&key, std::string nofond = "");
	double get_config_double(const std::string&tag, const std::string&key, double nofond = 0.0);
private:
	boost::property_tree::ptree root_;
};
#endif
