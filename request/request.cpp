#include "Response.hpp"
#include "server.hpp"

request::request(std::string raw_req, std::map<std::string, loc_details> locations) : locations(locations), req(raw_req)
{
    this->stat_code = req_arch();
}

inline void    stat_(int status_code)
{
    std::cout << status_code << std::endl;
}

inline void     err_(const std::string &err)
{
    std::cerr << err << std::endl;
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
    p "METHODS  \t" << this->method << std::endl;
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
    this->has_body = false;

    if (req.empty())
        return (err_("EMPTY"), 400);
    std::stringstream ss(req);
    std::getline(ss, l1_s, '\n');

    std::stringstream l1(trim_line(l1_s));

    std::getline(l1, this->method, ' ');
    if (!valid_method())
        return (err_("METHOD NOT VALID"), 400);

    std::getline(l1, this->URI, ' ');
    if (!is_valid_URI(this->URI))
        return (err_("invalid URI"), 400);
    if (this->URI.size() > MAX_URI_SIZE)
        return (414);
    std::getline(l1, this->HTTP, '\r');

    //LINE 2
    //locate the second line to \r\n
    size_t head_beg = req.find("\r\n");
    size_t head_end = req.find("\r\n\r\n");
    std::stringstream headers_raw(req.substr(head_beg, head_end - head_beg));

    while(std::getline(headers_raw, tmp_line, '\n'))
    {
        std::stringstream raw(tmp_line);
        std::getline(raw, field, ':');
        std::getline(raw, value, '\r');
        this->headers.insert(std::make_pair(field, trim_line(value)));
    }
    if (headers.find("Host") == headers.end())
        return (err_("NO HOST FOUND"), 400);

    if (req.size() > head_end + 4)
    {
        this->_body = req.substr(head_end + 4);
        
        if (this->method != "POST")
            return (err_("there is body but non-POST method is used"), 400);
        if (this->_body.size() > GLOBAL_CLIENT_MAX_BODY_SIZE)
            return (err_("Big BODY"), 413);
        this->has_body = true;
    }

    if (this->headers.find("Transfer-Encoding") != this->headers.end()
        && this->headers["Transfer-Encoding"] != "chunked")
        return (501);

    if (this->headers.find("Transfer-Encoding") == this->headers.end()
        && this->headers.find("Content-Length") == this->headers.end()
        && this->method == "POST")
        return (err_("no Trans/Cont length and the method is POST"), 400);
    return (0);
}

bool request::get_matched_loc_for_req_uri() //REQ
{
    // std::string head_val = headers["Location"];
    size_t tmp_size = 0;
    std::vector<std::string> potential_locations;
    std::string correct_loc;
    for (std::map<std::string, loc_details>::iterator it = locations.begin(); it != locations.end(); it++)
    {
        if (URI.rfind(it->first) == 0) //TODO maybe change it with begin_with
            potential_locations.push_back(it->first);
    }
    if (!potential_locations.size())
    {
        if (locations["default"].root.empty())
            return (false);
        else
            potential_locations.push_back("default");
    }
    for (std::vector<std::string>::iterator it = potential_locations.begin(); it != potential_locations.end(); it++)
    {
        //TODO why *it.size() do not work?
        if (tmp_size < it->size())
        {
            tmp_size = it->size();
            correct_loc = *it;
        }
    }
    current_loc = this->locations[correct_loc];
    return (true);
}

bool request::is_location_have_redir() //REQ
{
    //TODO init the status_code with 0
    return (!current_loc.status_code);
}

bool request::is_method_allowed_in_loc() //REQ
{
    std::vector<std::string> met = current_loc.allowed_methods;

    std::cout <<  current_loc.root << "DKLSADMS\n"; 
    
    return (std::find(met.begin(), met.end(), this->method) != met.end());
}

int request::get_request_resource() //get_resource_type()
{
    this->resource_path = current_loc.root + this->URI;
    struct stat s;
    if (!stat(this->resource_path.c_str(), &s))
    {
        if (S_ISDIR(s.st_mode))
            return (1);
        else if (S_ISREG(s.st_mode))
            return (2);
        else
            return (err_("get_request_resource"), -1);
    }
    else
        return (err_("Resource not found"), 0);
    return (err_("Error on getcwd"), -2);
}

inline bool request::is_uri_has_slash_in_end()
{
    return (this->URI.back() == '/');
}

bool request::is_dir_has_index_path()
{
    std::string to_check;
    struct stat s;

    for (size_t index = 0; index < current_loc.index_path.size() ; index++)
    {
        to_check = current_loc.root + "/" + current_loc.index_path[index];
        if (!stat(to_check.c_str(), &s) && S_ISREG(s.st_mode))
        {
            resource_path = to_check;
            return (true);
        }
    }
    return (false);
}

bool request::if_location_has_cgi()
{
    return (current_loc.has_cgi);
}

void    request::display_req()
{
    std::cout << this->method << " " << this->URI << " " << this->HTTP << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin() ; it != headers.end() ; it++)
        std::cout << it->first << ": " << it->second << std::endl;
}

int     request::GET()
{
    int resource_type = get_request_resource();
    // p this->resource_path << "PPPOO" << std::endl;
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (301); //redir to same path + "/"
        this->add_slash = true;
        if (!is_dir_has_index_path())
        {
            if (!get_auto_index())
                return (err_("NO AUTO_INDEX"), 403);
            return (200); //return autoindex of the directory
        }
    }
    if (!if_location_has_cgi())
        return (200);
    /*
    else
        RUN CGI WITH GET - STAT CODE DEP ON CGI
    */
   return (0); //tmp
}

int     request::POST()
{
    if (current_loc.client_max_body_size > 0)
        return (if_loc_support_upload()); //upload the POST req body

    int resource_type = get_request_resource();
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (301); //redir to same path + "/"
        this->add_slash = true;
        if (is_dir_has_index_path())
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
    return (0); //tmp
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
        this->add_slash = true;
        if (!if_location_has_cgi())
            return (403);
        if (is_dir_has_index_path())
        {
            if (if_location_has_cgi())
            {
                if (!is_dir_has_index_path())
                    return (403);
                /*
                else
                    RUN CGI WITH POST - STAT CODE DEP ON CGI
                */
            }
            if (delete_all_folder_content(resource_path))
                return (204);
            if (has_write_access_on_folder())
                return (500);
            return (403);
        }
    }
    else if (resource_type == 2)
    {
        if (!if_location_has_cgi())
            return (204);
        /*
        else
            RUN CGI WITH POST - STAT CODE DEP ON CGI
        */
    }
    return (0); //tmp
}

int    request::req_arch()
{
    int stat_code = is_req_well_formed();
    if (stat_code)
        return (stat_code);
    if (!get_matched_loc_for_req_uri())
        return (404);
    if (!is_location_have_redir())
        return (301);
    if (!is_method_allowed_in_loc())
        return (405);
    
    if (this->method == "GET")
        return (GET());
    else if (this->method == "POST")
        return (POST());
    else if (this->method == "DELETE")
        return (DELETE());
    return (501);
}

//GET
bool    request::get_auto_index()
{
    respond_with_autoindex = false;
    if (!current_loc.auto_index)
        return (false);
    respond_with_autoindex = true;
    return (true);
}

std::string get_file_name(const std::string &raw)
{
    size_t beg = raw.find("name=\"");
    size_t end = raw.find("\"", beg + 6);
    if (beg == std::string::npos || end == std::string::npos)
        return ("");
    return (raw.substr(beg + 6, end - beg - 6));
}

bool process_multipart(std::string _body, std::string _boundary)
{
    std::string line;
    size_t pos = 0;

    while((pos = _body.find("--" + _boundary, pos)) != std::string::npos)
    {
        size_t next = _body.find("--" + _boundary, pos + 2 + _boundary.size());
        std::string current_part = _body.substr(pos, next - pos);

        size_t file_beg = current_part.find("filename=\"");
        size_t file_end = current_part.find("\"", file_beg + 10);
        if (file_beg == std::string::npos || file_end == std::string::npos)
            return (err_("Cannot find name") ,false);
        std::string file_name = current_part.substr(file_beg + 10, file_end - file_beg - 10);

        size_t cont_beg = current_part.find("\r\n\r\n");
        size_t cont_end = current_part.size();
        if (cont_beg == std::string::npos || cont_end == std::string::npos)
            return (err_("No body found to upload"), false);

        std::ofstream outfile("/Users/bel-oirg/Desktop/Webserv/Upload/" + file_name);
        if (!outfile)
            return (err_("Failed to open the upload_file"), false);
        outfile << _body.substr(cont_beg + 4, cont_end - cont_beg - 4);

        if (_body.find("--" + _boundary + "--", next) != std::string::npos)
            break;
        pos = next;
    }
    return (true);
}

//POST
int request::if_loc_support_upload()
{
    if (_body.size() > current_loc.client_max_body_size)
        return (err_("body_size on loc_support_upload"), 413);

    if (headers["Content-Type"].rfind("multipart/form-data") == std::string::npos)
        return (err_("multipart/form not found on loc_support_upload"), 415);
    
    size_t bound_beg = headers["Content-Type"].find("boundary=");
    size_t bound_end = headers["Content-Type"].find_first_of(" \r\n", bound_beg);
    std::string _boundary = headers["Content-Type"].substr(bound_beg + 9, bound_end - bound_beg - 9);

    if (!process_multipart(_body, _boundary))
        return (err_("process_multipart err"), 400);
    return (0);
}

/*
    3 prob
        handled index ?? if there is index and autoindex check the index first
*/

bool request::delete_all_folder_content(std::string ress_path)
{
    DIR* dir = opendir(ress_path.c_str());
    if (!dir)
        return(err_("Cannot open dir"), false);
    struct dirent *entry;
    struct stat s;
    std::string sub;

    while((entry = readdir(dir)))
    {
        sub = entry->d_name;
        if (sub == "." || sub == "..")
            continue;
        
        std::string a_path = ress_path + sub;
        if (stat(a_path.c_str(), &s) < 0)
            return (err_("stat fails"), closedir(dir), false);
        if (S_ISDIR(s.st_mode))
        {
            if (!delete_all_folder_content(a_path))
                return (closedir(dir), false);
        }
        else
        {
            if (remove(a_path.c_str()))
                return (err_("Cannot delete file"), closedir(dir), false);
        }
    }
    closedir(dir);
    if (rmdir(ress_path.c_str()))
        return (err_("Cannot delete dir"), false);
    return (true);
}

bool request::has_write_access_on_folder()
{
    struct stat s;
    stat(resource_path.c_str(), &s);
    return (s.st_mode & S_IWUSR);
}