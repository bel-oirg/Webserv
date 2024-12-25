#include "response.hpp"
#include <algorithm>
#include "utils.hpp"

string fix_slash(string base, string file);

string response::get_script_path()
{
	return (this->resource_path);
}

string response::get_body()
{
	return (this->_body);
}

std::string response::set_status(int stat_code)
{
    static std::map<int, std::string> status_map;
    status_map[200] = std::string("200 OK");
    status_map[201] = std::string("201 Created"); //mainly it returns json of the uploaded data
    status_map[204] = std::string("204 No Content");
    status_map[301] = std::string("301 Moved Permanently");
    status_map[302] = std::string("302 Found");
    status_map[400] = std::string("400 Bad Request");
    status_map[403] = std::string("403 Forbidden");
    status_map[404] = std::string("404 Not Found");
    status_map[405] = std::string("405 Method Not Allowed");
    status_map[409] = std::string("409 Conflict");
    status_map[413] = std::string("413 Content Too Large");
    status_map[414] = std::string("414 URI Too Long");
    status_map[500] = std::string("500 Internal Server Error");
    status_map[501] = std::string("501 Not Implemented");
    status_map[504] = std::string("504 Gateway Timeout");
    return (status_map[stat_code]);
}

bool response::prep_cgi()
{
    if (!_is_cgi)
    {
        _is_cgi = true;

        _cgi = new Cgi(resource_path, body, prepare_env_cgi(), this->current_loc, this->locations["default"]);
    }
    
    if (!_cgi->is_cgi_ready())
        return (false);

    this->stat_code = _cgi->cgi_get_code();
    if (this->stat_code / 400)
        return (_40X_50X(), true);

    _server = "CGI-2.0";
    _content_length = lseek(_cgi->get_outfd(), 0, SEEK_END);
    lseek(_cgi->get_outfd(), 0, SEEK_SET); //CHANGE HERE

    char buff[REQUEST_MAX_SIZE + 1];
    int readen = read(_cgi->get_outfd(), buff, REQUEST_MAX_SIZE);

    if (readen <= 0)
    {
        this->_eof = true;
        return(false);
    }
    this->_cgi_str = string(buff, readen);
    //TODO the headers your providing of cgi does not ends with /r/n
    //sometimes it includes 200 OK 
    //check the second arg on if below
    size_t cgi_head_end = this->_cgi_str.find("\n\n");
    if (cgi_head_end != string::npos && cgi_head_end == this->_cgi_str.find("\n"))
    {
        this->_cgi_head = this->_cgi_str.substr(0, cgi_head_end);
        this->_cgi_str = this->_cgi_str.substr(cgi_head_end + 2);
        _content_type = "";
        _content_length -= readen;
    }
    return (true);
}

void response::debug()
{
    time_t timestamp;
    time(&timestamp);
    string now = ctime(&timestamp);
    size_t last_sp = now.rfind(" ");
    size_t first_sp = now.rfind(" ", last_sp - 1);
    now = now.substr(first_sp + 1, last_sp - first_sp - 1);
    pp BLUE << "[" << now << "] " << WHITE << " " << this->URI << " " << YELLOW << set_status(stat_code) << BLUE << " " << _content_length << RESET << endl;
}

std::string response::get_response_header() //_____SEND__RESP__HEAD
{
	if(headers["Connection"] != "Keep-Alive")
		_is_closed = true;
    if (this->stat_code == -1 && !prep_cgi())
        return ("");

    std::stringstream   line;
    
    line << "HTTP/1.1 " << set_status(stat_code) << "\r\n";

    if (!this->_cgi_head.empty())
        line << _cgi_head << "\r\n";

    if (this->_connection.empty())
        this->_connection = "close";
    line << "Connection: " << this->_connection << "\r\n";
    
    line << "Server: " << this->_server << "\r\n";
    
    if (!this->_content_type.empty())
        line << "Content-Type: " << this->_content_type << "\r\n";
        
    if (this->_content_length == -1)
        line << "Transfer-Encoding: " << this->_transfer_encoding << "\r\n";
    else
        line << "Content-Length: " << this->_content_length << "\r\n";

    if (!_cookies.empty())
        line << "Set-Cookie: " << _cookies << "\r\n";
    
    if (!_location.empty())
        line << "Location: " << _location << "\r\n";

    line << "\r\n";

    if (!this->_cgi_str.empty())
        line << this->_cgi_str;

    debug();
    return (line.str());
}

void response::set_content_length()
{
    if (!_body.empty())
        _content_length = _body.size();
}

void response::set_server()
{
    this->_server = "Webserv 2.0";
}

void response::set_cookies()
{
    _cookies = headers["Cookie"];
}

void response::set_connection()
{
    this->_connection = headers["Connection"];
}

void response::set_content_type()
{
    this->_content_type = "";

    if (this->stat_code == -1)
        this->_content_type = "text/html";
    if (!this->_body.size() && !this->_content_length)
        return ;
    static std::map<std::string, std::string> mime;

    mime["html"] = "text/html";
    mime["htm"] = "text/html";
    mime["shtml"] = "text/html";
    mime["css"] = "text/css";
    mime["xml"] = "text/xml";
    mime["gif"] = "image/gif";
    mime["jpeg"] = "image/jpeg";
    mime["jpg"] = "image/jpeg";
    mime["js"] = "application/javascript";
    mime["mjs"] = "application/javascript";
    mime["atom"] = "application/atom+xml";
    mime["rss"] = "application/rss+xml";
    mime["txt"] = "text/plain";
    mime["png"] = "image/png";
    mime["json"] = "application/json";
    mime["pdf"] = "application/pdf";
    mime["zip"] = "application/zip";
    mime["tar"] = "application/x-tar";
    mime["mp3"] = "audio/mpeg";
    mime["wav"] = "audio/wav";
    mime["mp4"] = "video/mp4";
    mime["mpeg"] = "video/mpeg";
    mime["avi"] = "video/x-msvideo";
    mime["webm"] = "video/webm";
    mime["ico"] = "image/vnd.microsoft.icon";
    mime["svg"] = "image/svg+xml";
    mime["webp"] = "image/webp";
    mime["otf"] = "font/otf";
    mime["ttf"] = "font/ttf";
    mime["woff"] = "font/woff";
    mime["woff2"] = "font/woff2";
    mime["3gp"] = "video/3gpp";
    mime["3g2"] = "video/3gpp2";
    mime["ts"] = "video/mp2t";
    mime["eot"] = "application/vnd.ms-fontobject";
    mime["wasm"] = "application/wasm";
    mime["csv"] = "text/csv";
    mime["md"] = "text/markdown";
    mime["php"] = "application/x-httpd-php";
    mime["exe"] = "application/octet-stream";

    _content_type = "text/html";
    size_t dot_p = this->URI.find_last_of('.');
    if (dot_p == std::string::npos || this->stat_code != 200)
        return ;

    std::string ext = this->URI.substr(dot_p + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    std::map<std::string, std::string>::iterator it = mime.find(ext);
    if (it != mime.end())
        this->_content_type = it->second;
}

void response::set_transfer_encoding()
{
    if (this->_content_length == -1)
        this->_transfer_encoding = "chunked";
}

void response::set_location()
{
    this->_location = "";
    if (this->stat_code == 301)
    {
        if (current_loc.redir_to.empty())
        {
            this->_location = this->URI + "/";
        }
        else
            this->_location = current_loc.redir_to;
    }
}

bool response::prepare_autoindex()
{
    std::string dir = this->resource_path;
    DIR *dirp = opendir(dir.c_str());

    if (!dirp)
        return (perror("opendir failed"), false);
    
    std::stringstream raw_body;
    raw_body    << "<title>Directory listing for "<< dir
                << "</title>\n<body>\n<h2>Directory listing for "
                << dir << "</h2>\n<hr>\n<ul>\r";
    std::string current;
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        struct stat s;
        current = dp->d_name;
        if ((current == ".") || (current == ".."))
            continue;
        current = dir + dp->d_name;
        if (stat(current.c_str(), &s) < 0)
            return (closedir(dirp), perror(NULL), false);

        if (S_ISDIR(s.st_mode))
            raw_body << "<li><a href=\"" << dp->d_name << "/\">" << dp->d_name << "</a>" << std::endl;
        else if (S_ISREG(s.st_mode))
            raw_body << "<li><a href=\"" << dp->d_name << "\">" << dp->d_name << "</a>" << std::endl;
    }
    raw_body << "</ul>\n<hr>\n</body>\n</html>\n";
    _body = raw_body.str();
    return (closedir(dirp), true);
}

inline void     err_(const std::string &err)
{
    std::cerr << "[-] " << err << std::endl;
}

bool response::prep_body(const std::string &path)
{
    if (!_body.empty())
        return (true);
    infile.open(path, std::ios::binary);
    if (!infile)
    {
        this->stat_code = 500;
        std::cerr << "Error opening " << path << std::endl;
        return (false);
    }

    infile.seekg(0, ios::end);
    _content_length = infile.tellg();
    infile.seekg(0);
    return (true);
}

string response::get_to_send() //_____RESP_BODY_SEND__
{
    if (_is_cgi)
    { 
        if (!_cgi->is_cgi_ready())
            return "";
        stat_code = _cgi->cgi_get_code();
        if (stat_code == 200)
        {
            char buff[2001];
            int readen = read(_cgi->get_outfd(), buff, 2000);
            if (readen > 0)
                return (buff[readen] = 0, string(buff, readen));
            return (this->_eof = true,  "");
        }
    }
    if (!_body.empty()) //in this case i am sure that the body is small (indexing a dir / ..)
        return (this->_eof = true, _body);

    char buff[REQUEST_MAX_SIZE + 1] = {0};
    infile.read(buff, REQUEST_MAX_SIZE);
    size_t readden = infile.gcount();

    if (!readden)
        return(this->_eof = true, infile.close(), "");
    
    return(std::string(buff, readden));
}

void response::throw_err_body(string err)
{
    this->stat_code = 500;
    err_(err);
    _40X_50X();
}

void response::_20X()
{
    if (this->stat_code == 204)
    {
        _body = "File Deleted Successfully";
        if (method != "DELETE")
            _body = "File Uploaded Successfully";
    }
    else if (this->stat_code == 200)
    {
        if (resource_type == 1)
        {
            if (is_dir_has_index_path())
                prep_body(this->resource_path);
            else if (get_auto_index() && !prepare_autoindex())
		        throw_err_body("");
        }
        else if (resource_type == 2)
            prep_body(this->resource_path);
        else
            throw_err_body("resource_type unknown");
    }
    else
        throw_err_body("20X unknown");
}

void response::_40X_50X()
{
    if (current_loc.error_pages.find(this->stat_code) != current_loc.error_pages.end())
    {
        pp GREEN << current_loc.error_pages[this->stat_code] << RESET << endl;
        if (current_loc.error_pages[this->stat_code].size())
            prep_body(fix_slash(current_loc.root, current_loc.error_pages[this->stat_code])); //BUG CPP11
        else
            prep_body(fix_slash(current_loc.root, locations["default"].error_pages[this->stat_code])); //BUG CPP11
    }
    else
        prep_body(fix_slash(ERR_DIR, wbs::to_string(this->stat_code)) + ".html"); //BUG CPP11
}

void response::set_body()
{
    _body = "";

    if (this->stat_code / 400)
        _40X_50X();
    else if (this->stat_code / 300 || this->stat_code == -1)
        return ;
    else if (this->stat_code / 200)
        _20X();
	else
        throw_err_body("stat_code unknown");
}

std::map<std::string, std::string>    response::prepare_env_cgi()
{
    std::map<std::string, std::string> environ_vars;
    environ_vars["REQUEST_METHOD"] = this->method;
    environ_vars["SERVER_NAME"] = this->_server_info.server_name;
    environ_vars["SCRIPT_NAME"] = this->URI ;
    environ_vars["CONTENT_TYPE"] = this->_content_type;

    _content_length = body.size();
    environ_vars["CONTENT_LENGTH"] = wbs::to_string(this->_content_length); //BUG CPP11 TODO THIS IS WRONG
    
    environ_vars["SCRIPT_FILENAME"] = this->resource_path;
    environ_vars["HTTP_USER_AGENT"] = this->headers["User-Agent"];
    environ_vars["HTTP_COOKIE"] = this->_cookies;
    environ_vars["SERVER_PORT"] = this->_server_info.server_port;
	environ_vars["GATEWAY_INTERFACE"] = "CGI/1337";
	environ_vars["SERVER_SOFTWARE"] = "42 webserv (Unix)";
	environ_vars["SERVER_PROTOCOL"] =  "HTTP/1.1";
	environ_vars["REMOTE_ADDR"] = this->_server_info.remote_addr;
	environ_vars["REMOTE_HOST"] = this->_server_info.server_name;
    environ_vars["QUERY_STRING"] = this->query;

    return (environ_vars);
}

response::response(std::string req, std::map<string, loc_details> locations, server_info info) : request(req, locations)
{
    this->_eof = false;
    this->_is_cgi = false;
	this->_is_closed = false;
    _content_length = 0;
	_cgi = NULL;
	this->_server_info = info;
    set_connection();
    set_server();
    set_cookies();
    set_location();
    set_body();
    set_content_type();
    set_content_length();
    set_transfer_encoding();
    if (stat_code != 204)
        this->upload_eof = true;
}

response::~response()
{
    infile.close();
	if (_cgi)
		delete _cgi;
}

/*
    configure where they should be saved.
*/
