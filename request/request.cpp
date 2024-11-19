#include "request.hpp"

request::request(std::string raw_req) : req(raw_req) {}

bool request::is_valid_URI()
{
    const std::regex path_regex("^(/[-a-zA-Z0-9._~%!$&'()*+,;=:@]*)*$");
    return (std::regex_match(URI, path_regex));
}

inline void    stat_(int status_code)
{
    std::cout << status_code << std::endl;
}

std::string trim_line(const std::string &line)
{
    size_t first = line.find_first_not_of(" \t");
    size_t last = line.find_last_not_of(" \t");
    std::string trimed; //TODO maybe remove this
    if (first == std::string::npos)
        trimed = "";
    else
        trimed = line.substr(first, last - first + 1);
    return (trimed);
}

bool    request::valid_method()
{
    std::vector<std::string> all;
    all.push_back("POST");
    all.push_back("DELETE");
    all.push_back("GET");
    return (std::find(all.begin(), all.end(), this->method) != all.end());
}

bool    request::valid_elem(std::string elem)
{
    std::string field, value, allowed_field = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'*+-.^_`|~";
    std::stringstream line(elem);
    std::getline(line, field, ':');

    if (isdigit(field.front()))
        return (false);
    for (size_t index = 0; index < field.size() ; index++)
    {
        if (allowed_field.find(field[index]) == std::string::npos)
            return (false);
    }
    std::getline(line, value);

    value = trim_line(value);
    if (value.empty() || value.back() != '\r')
        return (false);
    
    for (size_t index = 0; index < value.size() - 1 ; index++)
    {
        if (value[index] <= 31 || value[index] >= 127)
            return (false);
    }
    return (true);
}

bool request::is_req_well_formed() //REQ
{
    //LINE 1
    std::string l1_s, tmp_line, field, value;
    bool blank_line = false;
    this->max_body_size = 200;
    this->has_body = false;

    if (req.empty())
        return (printf("EMPTY\n"), false);
    std::stringstream ss(req);
    std::getline(ss, l1_s, '\n');

    std::stringstream l1(trim_line(l1_s));

    std::getline(l1, this->method, ' ');
    if (!valid_method())
        return (printf("METHOD REGEX\n"), false);

    std::getline(l1, this->URI, ' ');
    if (!request::is_valid_URI())
        return (stat_(400), false);
    if (this->URI.size() > 2048)
        return (stat_(414), false);
    std::getline(l1, this->HTTP, '\r');

    //LINE 2

    while(std::getline(ss, tmp_line, '\n'))
    {
        if (!request::valid_elem(tmp_line))
        {
            if (tmp_line != "\r" && !blank_line)
                return (false);
            blank_line = true;
            continue;
        }
        std::stringstream raw(tmp_line);
        std::getline(raw, field, ':');
        std::getline(raw, value, '\r');
        this->headers.insert(std::make_pair(field, trim_line(value)));
    }
    if (headers.find("Host") == headers.end())
        return (printf("NO HOST FOUND\n"), false);
    std::getline(ss, tmp_line);
    if (tmp_line.size())
    {
        if (tmp_line.size() >= this->max_body_size)
            return (stat_(413), false);
        this->has_body = true;
        this->headers.insert(std::make_pair("Body", tmp_line));
    }

    if (this->headers.find("Transfer-Encoding") != this->headers.end())
    {
        if (this->headers["Transfer-Encoding"] != "chunked")
            return (stat_(501), false);
    }
    else
    {
        if (this->headers.find("Content-Length") == this->headers.end() && this->method == "POST")
            return (stat_(400), false);
    }
    return (true);
}

bool request::get_matched_loc_for_req_uri() //REQ
{
    std::unordered_map<std::string, std::string>::iterator loc;

    loc = std::find(headers.begin(), headers.end(), "Location");
    if (loc == headers.end())
        return (printf("No location found in the header\n", false));
    this->current_loc = std::find(locations.begin(), locations.end(), *loc);
    return (this->current_loc != locations.end());
}

bool request::is_location_have_redir() //REQ
{
    //TODO init the status_code with 0
    return (!this->current_loc->second.status_code);
}

bool request::is_method_allowed_in_loc() //REQ
{
    std::vector<std::string> met = this->current_loc->second.allowed_methods;
    return (std::find(met.begin(), met.end(), this->method) != met.end());
}

//TODO check_which_req_method();

int request::get_request_resource() //get_resource_type()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
        //TODO i think this path is invalid
        this->ressource_path = std::string(buffer) + this->current_loc->second.root + this->URI;
        struct stat s;
        if (!stat(this->ressource_path.c_str(), &s))
        {
            if (S_ISDIR(s.st_mode))
                return (1);
            else if (S_ISREG(s.st_mode))
                return (2);
            else
                return (-1);
        }
        else
            return (printf("Resource not found\n"), 0);
    }
    return (printf("Error on getcwd\n"), -2);
}

inline bool request::is_uri_has_slash_in_end()
{
    return (this->URI.back() == '/');
}

bool request::is_dir_has_index_files()
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

bool request::if_location_has_cgi()
{
    return (this->current_loc->second.has_cgi);
}

void    request::display_req()
{
    std::cout << this->method << " " << this->URI << " " << this->HTTP << std::endl;
    for (std::unordered_map<std::string, std::string>::iterator it = headers.begin() ; it != headers.end() ; it++)
        std::cout << it->first << ": " << it->second << std::endl;
}