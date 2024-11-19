#include "request.hpp"

request::request(std::string raw_req) : req(raw_req) {}


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

bool    is_valid_URI(std::string URI)
{
    std::string allowed_URI = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._~:/?#[]@!$&'()*+,;=%";
    for (size_t index = 0; index < URI.size() ; index++)
    {
        if (allowed_URI.find(URI[index]) == std::string::npos)
            return (false);
    }
    return (true);
}

int request::is_req_well_formed() //REQ
{
    //LINE 1
    std::string l1_s, tmp_line, field, value;
    bool blank_line = false;
    this->max_body_size = 200;
    this->has_body = false;

    if (req.empty())
        return (printf("EMPTY\n"), 400);
    std::stringstream ss(req);
    std::getline(ss, l1_s, '\n');

    std::stringstream l1(trim_line(l1_s));

    std::getline(l1, this->method, ' ');
    if (!valid_method())
        return (printf("METHOD NOT VALID\n"), 400);

    std::getline(l1, this->URI, ' ');
    if (!is_valid_URI(this->URI))
        return (400);
    if (this->URI.size() > MAX_URI_SIZE)
        return (414);
    std::getline(l1, this->HTTP, '\r');

    //LINE 2

    while(std::getline(ss, tmp_line, '\n'))
    {
        if (!request::valid_elem(tmp_line))
        {
            if (tmp_line != "\r" && !blank_line)
                return (400);
            blank_line = true;
            continue;
        }
        std::stringstream raw(tmp_line);
        std::getline(raw, field, ':');
        std::getline(raw, value, '\r');
        this->headers.insert(std::make_pair(field, trim_line(value)));
    }
    if (headers.find("Host") == headers.end())
        return (printf("NO HOST FOUND\n"), 400);
    std::getline(ss, tmp_line);
    if (tmp_line.size())
    {
        if (tmp_line.size() >= this->max_body_size)
            return (413);
        this->has_body = true;
        this->headers.insert(std::make_pair("Body", tmp_line));
    }

    if (this->headers["Transfer-Encoding"] != "chunked")
        return (501);

    if (this->headers["Transfer-Encoding"].empty() && this->headers["Content-Length"].empty() && this->method == "POST")
        return (400);
    return (0);
}

bool request::get_matched_loc_for_req_uri() //REQ
{
    if (this->headers["Location"].empty())
        return (printf("No location found in the header\n"), false);

    std::unordered_map<std::string, std::string>::iterator loc;
    loc = std::find(headers.begin(), headers.end(), "Location");

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

int     request::GET()
{
    int resource_type = get_request_resource();
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (301); //redir to same path + "/"
        if (!is_dir_has_index_files())
        {
            if (!get_auto_index())
                return (403);
            return (200); //return autoindex of the directory
        }
    }
    if (!if_location_has_cgi())
        return (200);
    /*
    else
        RUN CGI WITH GET - STAT CODE DEP ON CGI
    */
}

int     request::POST()
{
    if (if_loc_support_upload())
        return (201); //upload the POST req body

    int resource_type = get_request_resource();
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (301); //redir to same path + "/"
        if (is_dir_has_index_files())
        {
            if (!if_location_has_cgi())
                return (403);
            /*
            else
                RUN CGI WITH POST - STAT CODE DEP ON CGI
            */
        }
        return (403);
    }
    else if (resource_type == 2) //file
    {
        if (!if_location_has_cgi())
            return (403);
        /*
        else
            RUN CGI WITH POST - STAT CODE DEP ON CGI
        */
    }
    
}

int     request::DELETE()
{
    int resource_type = get_request_resource();
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (409);
        if (!if_location_has_cgi())
            return (403);


        if (is_dir_has_index_files())
        {
            if (if_location_has_cgi())
            {
                if (!is_dir_has_index_files())
                    return (403);
                /*
                else
                    RUN CGI WITH POST - STAT CODE DEP ON CGI
                */
            }
        }
    }
}

int    request::req_arch()
{
    int stat_code;
    stat_code = is_req_well_formed();
    if (stat_code)
        return ;
    if (!get_matched_loc_for_req_uri())
        return (404);
    if (!is_location_have_redir())
        return (301);
    if (!is_method_allowed_in_loc())
        return (405);
    
    if (this->method == "GET")
        GET();
    else if (this->method == "POST")
        POST();
    else
        DELETE();
}

//GET
bool    request::get_auto_index()
{
    return (this->current_loc->second.auto_index);
}


//POST
bool request::if_loc_support_upload()
{
    return (!this->client_max_body_size.empty());
}
