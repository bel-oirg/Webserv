#include "GET.hpp"

//TODO get_ressource_type()

inline bool GET::is_uri_has_slash_in_end()
{
    return (this->URI.back() == '/');
}

bool GET::is_dir_has_index_files()
{
    std::string to_check;
    struct stat s;

    for (size_t index = 0; index < this->current_loc->second.index_files.size() ; index++)
    {
        //TODO should i add '/' ?
        to_check = this->current_loc->first + this->current_loc->second.index_files[index];
        if (!stat(to_check.c_str(), &s) && S_ISREG(s.st_mode))
        {
            ressource_path = to_check;
            return (true);
        }
    }
    return (false);
}

bool    GET::get_auto_index()
{
    return (this->current_loc->second.auto_index);
}

bool GET::if_location_has_cgi()
{
    return (this->current_loc->second.has_cgi);
}