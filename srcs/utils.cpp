
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

    std::string trim_line(const std::string &line)
    {
        size_t first = line.find_first_not_of(" \t");
        size_t last = line.find_last_not_of(" \t");
        if (first == std::string::npos)
            return ("");
        return (line.substr(first, last - first + 1));
    }

    std::vector<std::string> split(const std::string &str, string delimiters)
    {
        std::vector<string> splited;

        try
        {
            size_t start = 0, end = 0;
            while ((end = str.find_first_of(delimiters, start)) != string::npos)
            {
                string cur_splited = str.substr(start, end - start);
                cur_splited.erase(0, cur_splited.find_first_not_of(" \t"));
                cur_splited.erase(cur_splited.find_last_not_of(" \t") + 1);
                if (!cur_splited.empty())
                    splited.push_back(cur_splited);
                start = end + 1;
            }

            string last = str.substr(start);
            last.erase(0, last.find_first_not_of(" \t"));
            last.erase(last.find_last_not_of(" \t") + 1);
            splited.push_back(last);
        }
        catch (...)
        {
            throw runtime_error(__func__);
        }

        return splited;
    }

}