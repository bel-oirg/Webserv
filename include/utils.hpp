
#include "webserv.hpp"


namespace wbs
{
    string                      to_string(int value);
    std::string                 trim_line(const std::string &line);
	std::vector<std::string> split(const std::string &str, const std::string delimiters);
}