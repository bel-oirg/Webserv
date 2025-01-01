
#include "webserv.hpp"

namespace wbs
{
    string to_string(int value);
    std::string host2string(in_addr_t host);
    std::string trim_line(const std::string &line);
    std::vector<std::string> split(const std::string &str, const std::string delimiters);


    template <typename Iterator, typename Predicate>
    bool all_of(Iterator begin, Iterator end, Predicate pred)
    {
        for (Iterator it = begin; it != end; ++it)
        {
            if (!pred(*it))
            {
                return false;
            }
        }
        return true;
    }
}