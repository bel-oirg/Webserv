#include "response.hpp"
#include "server.hpp"
#include <algorithm>
#include "utils.hpp"


request::request(std::string raw_req, vector<Server> &p_servers, int port) : req(raw_req), servers(p_servers)
{
	cl_entry_port = port;
    // cout << servers.size() << endl;
	upload_eof = false;
    chunked = false;
    add_slash = false;
    PATH_first = false;
    body = "";

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
            //TODO do a cpp11 check
    }
    return (base + file);
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
    for (size_t index = 0; index < field.size() ; ++index)
    {
        if (allowed_field.find(field[index]) == std::string::npos)
            return (false);
    }
    std::getline(line, value);

    value = wbs::get_trimed(value);
    if (value.empty() || value[value.size() - 1] != '\r')
        return (false);
    
    for (size_t index = 0; index < value.size() - 1 ; ++index)
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
    size_t query_pos = URI.rfind("?");
    if (query_pos != std::string::npos)
    {
        this->query = URI.substr(query_pos + 1);
        this->URI = URI.substr(0, query_pos);
    }
    size_t find_dot = URI.rfind(".");
    if (find_dot != string::npos)
    {
        size_t find_slash = URI.find("/", find_dot);
        if (find_slash == string::npos)
            return (true);
        
       this->extension =  URI.substr(find_dot + 1, find_slash - find_dot - 1);

        this->PATH_INFO = URI.substr(find_slash); // INFO i changed this so path with a slash
        this->PATH_INFO_URI = URI.substr(0, find_slash);
    }
    return (true);
}

bool is_valid_size_t(const string &str_num, size_t &num)
{
    if (str_num.empty())
        return(num = 0, true);

    if (str_num[0] == '-')
        return false;

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

    // pp YELLOW << req << RESET << endl;
    if (req.empty())
        return (err_("EMPTY"), 400);
    std::stringstream ss(req);
    std::getline(ss, l1_s, '\n');

    std::stringstream l1(wbs::get_trimed(l1_s));

    std::getline(l1, this->method, ' ');
    if (!valid_method())
        return (err_("METHOD NOT VALID"), 400);

    std::getline(l1, this->URI, ' ');
    if (!is_valid_URI())
        return (err_("invalid URI"), 400);
    if (this->URI.size() + this->query.size() + this->PATH_INFO.size() > MAX_URI_SIZE)
        return (414);
    std::getline(l1, this->HTTP, '\r');
    if (this->HTTP != "HTTP/1.1")
        return (err_("Invalid HTTP version"), 400);
    //LINE 2

    size_t head_beg = req.find("\r\n");
    size_t head_end = req.find("\r\n\r\n");
    if (head_beg == string::npos || head_end == string::npos)
        return (err_("incorrect carriage"), 400);
    std::stringstream headers_raw(req.substr(head_beg + 2, head_end - head_beg - 2));

    while(std::getline(headers_raw, tmp_line, '\n'))
    {
        std::stringstream raw(tmp_line);
        std::getline(raw, field, ':');
        std::getline(raw, value, '\r');
        value = wbs::get_trimed(value);
        field = wbs::get_trimed(field);
        if (field.empty())
            return (err_("Empty field"), 400);
        if (value.empty())
            continue;
        
        this->headers.insert(std::make_pair(field, value));
    }
    if (headers.find("Host") == headers.end())
        return (err_("NO HOST FOUND"), 400);

    this->host = headers["Host"];

    
    if (req.size() > head_end + 4)
    {
        this->body = req.substr(head_end + 4);
        
        if (this->method == "GET")
            return (err_("there is body but GET method is used"), 400);
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

int is_file_dir(const string &file)
{
    struct stat buffer;
    if (stat(file.c_str(), &buffer) == 0)
    {
        if (buffer.st_mode & S_IFDIR)
            return (1); //dir
        else if (buffer.st_mode & S_IFREG)
            return (2); //file
    }
    return (0);
}

bool request::get_matched_loc_for_req_uri() //REQ
{
    size_t tmp_size = 0;
    std::vector<std::string> potential_locations;
    for (std::map<std::string, loc_details>::iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (URI.find(it->first) == 0)
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
            if (is_file_dir(locations[*it].root + "/" + URI.substr(it->size()))
                || URI.find(fix_slash(*it, "/")) == 0)
            {
                tmp_size = it->size();
                correct_loc_name = *it;
            }
        }
    }
    current_loc = this->locations[correct_loc_name];
    return (true);
}

bool request::is_location_have_redir() //REQ`
{
    return (!current_loc.redir_to.empty());
}

bool request::is_method_allowed_in_loc() //REQ
{
    std::vector<std::string> met = current_loc.allowed_methods;
    if (!met.size())
        met = this->locations["default"].allowed_methods;
    return (std::find(met.begin(), met.end(), this->method) != met.end());
}

void request::set_resource_path()
{
    if (correct_loc_name != "default")
        this->resource_path = fix_slash(current_loc.root, this->URI.substr(correct_loc_name.size(), URI.size() - correct_loc_name.size()));
    else
        this->resource_path = fix_slash(current_loc.root, this->URI);
}

int request::get_request_resource() //get_resource_type()
{
    set_resource_path();
    struct stat s;
    if (!stat(this->resource_path.c_str(), &s))
    {
        if (S_ISDIR(s.st_mode))
        {
            if (access(this->resource_path.c_str(), R_OK | X_OK)) //exec so i can access the dir
                return (err_("Read/Exec access"), -11);
            return (1);
        }
        else if (S_ISREG(s.st_mode))
        {
            if (access(this->resource_path.c_str(), R_OK))
                return (err_("Read access"), -22);
            return (2);
        }
        else
            return (err_("get_request_resource"), -1);
    }
    else
    {
        if (!this->PATH_INFO.empty() && current_loc.has_cgi && !PATH_first)
        {
            this->URI = this->PATH_INFO_URI;
            set_resource_path();
            if (is_file_dir(this->resource_path) != 2)
                return (err_("PATH_INFO + dir " + this->resource_path), 0);
            for (std::vector<std::string>::iterator it = current_loc.cgi_extentions.begin(); it != current_loc.cgi_extentions.end(); ++it)
            {
                if (this->extension == *it)
                {
                    PATH_first = true;
                    return (get_request_resource());
                }
            }
            return (err_("PATH_INFO + invalid extension"), 0);
        }
        return (err_("Resource not found " + resource_path), 0);
    }
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

    if (!current_loc.index_path.empty())
    {
        for (size_t i = 0; i < current_loc.index_path.size(); ++i)
        {
            to_check = fix_slash(current_loc.root, current_loc.index_path[i]);
            if (!stat(to_check.c_str(), &s) && S_ISREG(s.st_mode))
            {
                resource_path = to_check;
                return (true);
            }
        }
    }
    return (false);
}

bool request::if_location_has_cgi()
{
    if (!current_loc.has_cgi)
        return (false);
    string ext = this->resource_path.substr(this->resource_path.rfind(".") + 1);
    vector<string>::iterator it = find(current_loc.cgi_extentions.begin(), current_loc.cgi_extentions.end(), ext);
    return (it != current_loc.cgi_extentions.end());
}

void    request::display_req()
{
    cout << this->method << " " << this->URI << " " << this->HTTP << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin() ; it != headers.end() ; ++it)
        cout << it->first << ": " << it->second << std::endl;
}

int     request::GET()
{

    if (!current_loc.has_cgi && !this->PATH_INFO.empty())
        this->URI += "/" + this->PATH_INFO;

    resource_type = get_request_resource();
    if (resource_type < -10)
        return (403);
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (301); //redir to same path + "/"
        this->add_slash = true;
        if (!is_dir_has_index_path())
        {
            if (!current_loc.index_path.empty() && !get_auto_index())
                return (err_("invalid index file"), 404);
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
    if (headers["Transfer-Encoding"] == "chunked" && !unchunk_body())
        return (400);

    if (headers["Content-Type"].find("multipart/form-data") != string::npos)
        return (if_loc_support_upload());

    resource_type = get_request_resource();
    if (resource_type < -10)
        return (403);
    if (resource_type <= 0)
        return (404);
    if (resource_type == 1) // dir
    {
        if (!is_uri_has_slash_in_end())
            return (301); //redir to same path + "/"
        this->add_slash = true;
        if (is_dir_has_index_path())
        {
            if (!current_loc.index_path.empty())
                return (err_("invalid index file"), 404);
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
    if (resource_type < -10)
        return (403);
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
                return (upload_eof = true, 204);
            if (has_write_access_on_folder())
                return (pp "Permission denied to delete file/dir\n", 500);
            return (403);
        }
        if (!is_dir_has_index_path())
        {
            if (!current_loc.index_path.empty())
                return (err_("invalid index file"), 404);
            return (403);
        }
        return (-1);
    }
    if (!if_location_has_cgi()) // file
    {
        if (remove(this->resource_path.c_str()))
            return (pp "Failed to delete the resource\n", 500);
        return (pp "File deleted succesfully\n", upload_eof = true, 204);
    }
    return (-1);
}

bool    request::set_locations()
{
	// cout << "PORT: " << cl_entry_port << endl ;
	for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i]._server_info.remote_addr + ":" + wbs::to_string(servers[i].port) == this->host
        ||  (servers[i]._server_info.remote_addr == this->host && servers[i].port == 80))
        {
            this->locations = servers[i].get_locations();
            this->server_info = servers[i]._server_info;
            this->server_info.server_name = servers[i]._server_info.remote_addr;
			return (servers[i].port == (uint32_t)cl_entry_port);
        }
		for (size_t j = 0; j < servers[i].server_names.size(); ++j)
		{
			if (servers[i].server_names[j] + ":" + wbs::to_string(servers[i].port) == host 
			||  servers[i].server_names[j] == servers[i]._server_info.remote_addr)
			{
				this->locations = servers[i].get_locations();
                this->server_info.server_name = servers[i].server_names[j];
                this->server_info = servers[i]._server_info;
				return (servers[i].port == (uint32_t)cl_entry_port);
			}
		}
    }
    return false; 
}

int     request::init_parse_req()
{
    int stat_code = is_req_well_formed();

	if (!set_locations())
        return (404);

    if (stat_code)
        return (stat_code);
    if (!get_matched_loc_for_req_uri())
        return (404);

    if (!is_valid_size_t(this->headers["Content-Length"], length))
        return (err_("invalid content_length"), 400);
    if (length > locations["default"].client_max_body_size)
        return (err_("BODY > CLIENT_MAX_BODY _SIZE"), 413);

    if (is_location_have_redir())
        return (current_loc.status_code); //301 || 302

    vector<string> vec = current_loc.allowed_methods;
    if (find(vec.begin(), vec.end(), "NONE") != vec.end())
        return (pp "NONE method\n", 405);
    if (!is_method_allowed_in_loc())
        return (pp "method not allowed\n", 405);
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

bool create_dir_if_not_exist(string path)
{
    struct stat metadata;

    //CREATING ONLY LVL 1 OF DIR (NO SUBDIRS)
    if (stat(path.c_str(), &metadata))
    {
        if (mkdir(path.c_str(), 0777))
            return (false);
    }
    return (true);
}

int request::process_multipart(std::string &current_part) //____UPLOAD_REQ_
{
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

        if (!create_dir_if_not_exist(this->locations["default"].upload_path))
            return (err_("Error creating the upload_dir"), 0);

        outfile.open((this->locations["default"].upload_path + file_name).c_str(), std::ios::out | std::ios::binary);
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
        return (pp "file uploaded successfully\n", upload_eof = true);
    }
    outfile << current_part;
    uploaded_size += current_part.size();
    return (1);
}

bool request::unchunk_body()
{
    std::stringstream test;
    size_t part_size, next(0);
    string unchunked_body(""), new_part, unchunked(this->body), size_str;
    while(1)
    {
        size_t beg_hex = next;
        if (beg_hex == std::string::npos)
            return (err_("Error on beg_hex"), false);
        
        size_t end_hex = unchunked.find("\r\n", beg_hex);
        if (end_hex == std::string::npos)
            return (err_("end_hex on chunked npos"), false);

        size_str = unchunked.substr(beg_hex, end_hex - beg_hex);
        if (!wbs::all_of(size_str.begin(), size_str.end(), ::isalnum))
            return (err_("invalid size part of the chunked request"), false);

        test.clear();
        test.str("");
        test << size_str;
        test >> std::hex >> part_size;
        if (!part_size)
            break;
        size_t end_part = unchunked.find("\r\n", end_hex + 2);
        if (end_part == std::string::npos)
            return (err_("Error end_part on chunked npos"), false);
        new_part = unchunked.substr(end_hex + 2, end_part - end_hex - 2);
        if (!part_size || !test)
        {
            if (new_part != "0")
                return (err_("Bad ending of chunked data"), false);
            break;
        }
        if (new_part.size() != part_size)
            return (err_("Invalid unchunked size"), false);

        unchunked_body += new_part;
        next = end_part + 2;
    }
    body = unchunked_body;
    chunked = true;
    return (true);
}

//POST
int request::if_loc_support_upload()
{        
    if (!current_loc.enable_upload)
        return (err_("Upload is not supported on this location"), 403);
    
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
