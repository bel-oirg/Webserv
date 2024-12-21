#include "response.hpp"
#include "server.hpp"
#include <algorithm>
 
request::request(std::string raw_req, std::map<std::string, loc_details> locations) : req(raw_req), locations(locations)
{
	upload_eof = false;

    this->stat_code = init_parse_req();
    if (!this->stat_code)
        this->stat_code = req_arch();
    
}

string fix_slash(string base, string file)
{
    if (!base.empty() && base[base.size() - 1] == '/')
    {
        if (!file.empty() && file[0] == '/')
            file.erase(0, 1);
    }
    else
    {
        if (!file.empty() && file[0] != '/')
            base.append(1, '/');
    }
    return (base + file);
}

std::string trim_line(const std::string &line)
{
    size_t first = line.find_first_not_of(" \t");
    size_t last = line.find_last_not_of(" \t");
    if (first == std::string::npos)
        return("");
    return (line.substr(first, last - first + 1));
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

    if (isdigit(field[0]))
        return (false);
    for (size_t index = 0; index < field.size() ; index++)
    {
        if (allowed_field.find(field[index]) == std::string::npos)
            return (false);
    }
    std::getline(line, value);

    value = trim_line(value);
    if (value.empty() || value[value.size() - 1] != '\r')
        return (false);
    
    for (size_t index = 0; index < value.size() - 1 ; index++)
    {
        if (value[index] <= 31 || value[index] >= 127)
            return (false);
    }
    return (true);
}

bool    request::is_valid_URI()
{
    std::string allowed_URI = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    for (size_t index = 0; index < URI.size() ; index++)
    {
        if (allowed_URI.find(URI[index]) == std::string::npos)
            return (false);
    }
    size_t query_pos = URI.find("?");
    if (query_pos != std::string::npos)
    {
        this->query = URI.substr(query_pos + 1);
        this->URI = URI.substr(0, query_pos);
    }
    return (true);
}

bool is_valid_size_t(const string &str_num, size_t &num)
{
    if (str_num.empty())
        return(num = 0, true);

    std::stringstream ss(str_num);
    char c;

    if (!(ss >> num))
        return (false);
    if (ss >> c)
        return (false);
    return (true);
}

int request::is_req_well_formed() //REQ
{
    //LINE 1
    std::string l1_s, tmp_line, field, value;

    if (req == "Internal_err")
        return(err_("CGI_ERR"), 500);
    if (req == "Timeout_err")
        return(err_("CGI_ERR"), 504);

    if (req.empty())
        return (err_("EMPTY"), 400);
    std::stringstream ss(req);
    std::getline(ss, l1_s, '\n');

    std::stringstream l1(trim_line(l1_s));

    std::getline(l1, this->method, ' ');
    if (!valid_method())
        return (err_("METHOD NOT VALID"), 400);

    std::getline(l1, this->URI, ' ');
    if (!is_valid_URI())
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
        this->body = req.substr(head_end + 4);
        
        if (this->method != "POST")
            return (err_("there is body but non-POST method is used"), 400);

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
    size_t tmp_size = 0;
    std::vector<std::string> potential_locations;
    for (std::map<std::string, loc_details>::iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (URI.rfind(it->first) == 0)
            potential_locations.push_back(it->first);
    }
    if (!potential_locations.size())
    {
        if (locations["default"].root.empty())
            return (false);
        else
            potential_locations.push_back("default");
    }
    for (std::vector<std::string>::iterator it = potential_locations.begin(); it != potential_locations.end(); ++it)
    {
        if (tmp_size < it->size())
        {
            tmp_size = it->size();
            correct_loc_name = *it;
        }
    }
    current_loc = this->locations[correct_loc_name];
    return (true);
}

bool is_valid_int(const string &str_num, int &num)
{
    std::stringstream ss(str_num);
    char c;

    if (!(ss >> num))
        return (false);
    if (ss >> c)
        return (false);
    return (true);
}

bool request::is_location_have_redir() //REQ
{
    if (current_loc.redir_to.empty())
        return (false);

    size_t space_pos = current_loc.redir_to.find(" ");
    if (space_pos == string::npos || space_pos == current_loc.redir_to.size())
        return (err_("invalid redir_to"), false);
    string str_stat_code = current_loc.redir_to.substr(0, space_pos);
    if (!is_valid_int(str_stat_code, current_loc.status_code))
        return (err_("invalid int in redir_to"), false);
    current_loc.redir_to = current_loc.redir_to.substr(space_pos + 1, current_loc.redir_to.size() - space_pos - 1);

    return (current_loc.status_code == 301);
}

bool request::is_method_allowed_in_loc() //REQ
{
    std::vector<std::string> met = current_loc.allowed_methods;
    if (!met.size())
        met = this->locations["default"].allowed_methods;
    return (std::find(met.begin(), met.end(), this->method) != met.end());
}

int request::get_request_resource() //get_resource_type()
{
    
    if (correct_loc_name != "default")
        this->resource_path = fix_slash(current_loc.root, this->URI.substr(correct_loc_name.size(), URI.size() - correct_loc_name.size()));
    else
        this->resource_path = fix_slash(current_loc.root, this->URI);

    struct stat s;
	// cout << MAGENTA << resource_path << RESET << endl;
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
        return (err_("Resource not found" + resource_path), 0);
    return (err_("Error on getcwd"), -2);
}

inline bool request::is_uri_has_slash_in_end()
{
    if (!URI.empty())
        return (this->URI[URI.size() - 1] == '/');
    return (false);
}

bool request::is_dir_has_index_path()
{
    std::string to_check;
    struct stat s;

    if (current_loc.index_path.size())
    {
        to_check = fix_slash(current_loc.root, current_loc.index_path);
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
    cout << this->method << " " << this->URI << " " << this->HTTP << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin() ; it != headers.end() ; ++it)
        cout << it->first << ": " << it->second << std::endl;
}

int     request::GET()
{
    resource_type = get_request_resource();
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
   return (-1);
}

int     request::POST()
{
    if (current_loc.enable_upload)
        return (if_loc_support_upload());
    else if (length)
        return (err_("The upload is not enabled on the config file"), 403);

    resource_type = get_request_resource();
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
                return (err_("NO CGI ON DIR"), 403);
            return (-1);
        }
        return (err_("NO INDEX PATH"), 403);
    }
    //file
    if (!if_location_has_cgi())
        return (err_("NO CGI ON FILE"), 403);
    return (-1);
}

int     request::DELETE()
{
    resource_type = get_request_resource();
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (409);
        this->add_slash = true;
        if (!if_location_has_cgi())
        {
            if (delete_all_folder_content(resource_path))
                return (204);
            if (has_write_access_on_folder())
                return (500);
            return (403);
        }
        if (!is_dir_has_index_path())
            return (403);
        return (-1);
    }
    if (!if_location_has_cgi()) // file
    {
        remove(this->resource_path.c_str());
        return (pp "File deleted succesfully", 204);
    }
    return (-1);
}

int     request::init_parse_req()
{
    int stat_code = is_req_well_formed();
    if (stat_code)
        return (stat_code);
    if (!get_matched_loc_for_req_uri())
        return (404);

    is_valid_size_t(this->headers["Content-Length"], length);
    if (length > current_loc.client_max_body_size)
        return (err_("BODY > CLIENT_MAX_BODY _SIZE"), 413);

    if (is_location_have_redir())
        return (301);

    if (!is_method_allowed_in_loc())
        return (405);
    return (0);
}

int    request::req_arch()
{
    if (this->method == "GET")
        return (GET());
    else if (this->method == "POST")
        return (POST());
    return (DELETE());
}

//GET
inline bool    request::get_auto_index()
{
    return (current_loc.auto_index);
}

int request::process_multipart(std::string &current_part) //____UPLOAD_REQ_
{
    ttt
    if (file_name.empty())
    {
        uploaded_size = 0;

        size_t file_beg = current_part.find("filename=\"");
        size_t file_end = current_part.find("\"\r\n", file_beg + 10);
        if (file_beg == std::string::npos || file_end == std::string::npos)
            return (err_("Cannot find name : ") ,0);

        file_name = current_part.substr(file_beg + 10, file_end - file_beg - 10);
        if (file_name.find("/") != std::string::npos)
            return (err_("Invalid file name") ,0);

        outfile.open(UPLOAD_DIR + file_name, std::ios::out | std::ios::binary);
        if (!outfile)
            return (err_("Failed to open the upload_file"), 0);

        size_t cont_beg = current_part.find("\r\n\r\n");
        if (cont_beg == std::string::npos)
            return (err_("No body found to upload"), 0);
        current_part =  current_part.substr(cont_beg + 4);

        length -= cont_beg + 12 + boundary.size();
    }

    if (uploaded_size + current_part.size() >= this->length)
    {
        outfile << current_part.substr(0, this->length - uploaded_size);
        outfile.close();
        return (pp "file uploaded successfuly\n", upload_eof = true);
    }
    outfile << current_part;
    uploaded_size += current_part.size();
    return (1);
}

bool request::unchunk_body()
{
    std::stringstream test;
    size_t part_size, next(0);
    string unchunked_body(""), new_part, unchunked(this->body);

    while(1)
    {
        size_t beg_hex = next;
        size_t end_hex = unchunked.find("\r\n", beg_hex);

        if (beg_hex == std::string::npos || end_hex == std::string::npos)
            return (err_("Error on beg_hex || end_hex on chunked npos"), false);
        string size_str = unchunked.substr(beg_hex, end_hex - beg_hex);
        test.clear();
        test.str("");
        test << size_str;
        test >> std::hex >> part_size;
        size_t end_part = unchunked.find("\r\n", end_hex + 2);
        if (end_part == std::string::npos)
            return (err_("Error end_part on chunked npos"), false);
        new_part = unchunked.substr(end_hex + 2, end_part - end_hex - 2);
        if (!part_size)
        {
            if (!new_part.empty())
                return (err_("Bad ending of chunked data"), false);
            break;
        }
        if (new_part.size() != part_size)
            return (err_("Invalid unchunked size"), false);

        unchunked_body += new_part;
        next = end_part + 2;
    }
    body = unchunked_body;
    return (true);
}

//POST
int request::if_loc_support_upload()
{        
    if (headers["Content-Type"].rfind("multipart/form-data") == std::string::npos)
        return (err_("multipart/form not found on loc_support_upload"), 415);

    if (headers["Transfer-Encoding"] == "chunked")
        if (!unchunk_body())
            return (400);

    size_t bound_beg = headers["Content-Type"].find("boundary=");
    if (bound_beg == string::npos)
        return (err_("boundary beg not found"), 400);
    size_t bound_end = headers["Content-Type"].size();
    if (bound_end == string::npos)
        return (err_("boundary end not found"), 400);
    this->boundary = headers["Content-Type"].substr(bound_beg + 9, bound_end - bound_beg - 9);

    if (!process_multipart(body))
        return (err_("process_multipart err"), 400);
    return (204);
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
    
    if (rmdir(ress_path.c_str()))
        return (err_("Cannot delete dir"), closedir(dir), false);
    return (closedir(dir), true);
}

bool request::has_write_access_on_folder()
{
    struct stat s;
    stat(resource_path.c_str(), &s);
    return (s.st_mode & S_IWUSR);
}

request::~request()
{
    outfile.close();
}
