
#include "webserv.hpp"
#include <stdlib.h>

namespace wbs
{
    string to_string(int value)
    {
        ostringstream oss;
        oss << value;
        return oss.str();
    }

    void      trim_line(std::string &line)
    {
        size_t first = line.find_first_not_of(" \t\177");
        size_t last = line.find_last_not_of(" \t\177");

        if (first == std::string::npos)
        {
            return ;
            line = "";
        }
        
        line = line.substr(first, last - first + 1);
    }

    std::string get_trimed(const std::string &line)
    {
        size_t first = line.find_first_not_of(" \t\177");
        size_t last = line.find_last_not_of(" \t\177");
        if (first == std::string::npos)
            return("");
        return (line.substr(first, last - first + 1));
    }

    std::vector<std::string> split(const std::string &str, const std::string delimiters)
    {
        std::vector<std::string> result;
        size_t start = 0, end = 0;

        while ((end = str.find_first_of(delimiters, start)) != std::string::npos)
        {
            std::string token = str.substr(start, end - start);
            result.push_back(token);

            start = end + 1;
        }

        result.push_back(str.substr(start));

        return result;
    }

    std::string host2string(in_addr_t host)
    {
        struct in_addr addr;
        addr.s_addr = host;
        return inet_ntoa(addr);
    }

}