#include "response.hpp"
#include <algorithm>
#include <csignal>
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

    char buff[RW_BUFFER + 1]; // TODO mabe buff unused here
    int readen = read(_cgi->get_outfd(), buff, RW_BUFFER);

    if (readen <= 0)
    {
        this->_eof = true;
        return(false);
    }
    this->_cgi_str = string(buff, readen);

    size_t cgi_head_end = this->_cgi_str.find("\n\n");
    if (cgi_head_end != string::npos)
    {
        _content_length -= cgi_head_end + 2;
        std::string tmp_line, field, value;
        std::stringstream cgi_headers_raw(_cgi_str.substr(0, cgi_head_end + 1));
        _cgi_str = _cgi_str.substr(cgi_head_end + 2);

        while(std::getline(cgi_headers_raw, tmp_line, '\n'))
        {
            std::stringstream raw(tmp_line);
            std::getline(raw, field, ':');
            std::getline(raw, value, '\r');
            this->_cgi_headers.insert(std::make_pair(field, wbs::get_trimed(value)));
        }
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
    pp BLUE << "[" << now << "] " << WHITE << " " << this-> method << " " << this->URI << " " << YELLOW << set_status(stat_code) << BLUE << " " << _content_length << RESET << endl;
}

std::string response::get_response_header() //_____SEND__RESP__HEAD
{
    if (this->stat_code == -1 && !prep_cgi())
        return ("");

    std::stringstream   line;
    
    line << "HTTP/1.1 " << set_status(stat_code) << "\r\n";

    if (!this->_cgi_head.empty())
    {
        pp "CGI -> " << this->_cgi_head << endl;
        line << _cgi_head << "\r\n";
    }

    if (this->_connection.empty())
        this->_connection = "close";
    line << "Connection: " << this->_connection << "\r\n";
    
    line << "Server: " << this->_server << "\r\n";
    
    if (!this->_content_type.empty()
        && _cgi_headers.find("Content-Type") == _cgi_headers.end())
        line << "Content-Type: " << this->_content_type << "\r\n";
    

    line << "Content-Length: " << this->_content_length << "\r\n";
    
    if (!_location.empty())
        line << "Location: " << _location << "\r\n";

    for (map<string, string>::iterator it = _cgi_headers.begin(); it != _cgi_headers.end() ; it++)
    {
        line << it->first << ": " << it->second << "\r\n";
    }

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
    if (this->headers.find("Cookie") != this->headers.end())
        _cookies = headers["Cookie"];
}

void response::set_connection()
{
	if (headers.find("Connection") == headers.end()
        || headers["Connection"] != "keep-alive")
        this->_is_closed = true;

    this->_connection = (_is_closed) ? "close" : "keep-alive";
}

void response::set_content_type()
{
    pp "-> " << _content_type << endl;
    if (this->stat_code / 400)
    {
        this->_content_type = "text/html";
        return ;
    }

    if (this->stat_code == -1)
        this->_content_type = "text/html";
    if (!this->_body.size() && !this->_content_length)
        return ;
    static std::map<std::string, std::string> mime;

    mime["html"]    = "text/html";
    mime["htm"]     = "text/html";
    mime["shtml"]   = "text/html";
    mime["css"]     = "text/css";
    mime["xml"]     = "text/xml";
    mime["gif"]     = "image/gif";
    mime["jpeg"]    = "image/jpeg";
    mime["jpg"]     = "image/jpeg";
    mime["js"]      = "application/javascript";
    mime["mjs"]     = "application/javascript";
    mime["atom"]    = "application/atom+xml";
    mime["rss"]     = "application/rss+xml";
    mime["txt"]     = "text/plain";
    mime["png"]     = "image/png";
    mime["json"]    = "application/json";
    mime["pdf"]     = "application/pdf";
    mime["zip"]     = "application/zip";
    mime["tar"]     = "application/x-tar";
    mime["mp3"]     = "audio/mpeg";
    mime["wav"]     = "audio/wav";
    mime["mp4"]     = "video/mp4";
    mime["mpeg"]    = "video/mpeg";
    mime["avi"]     = "video/x-msvideo";
    mime["webm"]    = "video/webm";
    mime["ico"]     = "image/vnd.microsoft.icon";
    mime["svg"]     = "image/svg+xml";
    mime["webp"]    = "image/webp";
    mime["otf"]     = "font/otf";
    mime["ttf"]     = "font/ttf";
    mime["woff"]    = "font/woff";
    mime["woff2"]   = "font/woff2";
    mime["3gp"]     = "video/3gpp";
    mime["3g2"]     = "video/3gpp2";
    mime["ts"]      = "video/mp2t";
    mime["eot"]     = "application/vnd.ms-fontobject";
    mime["wasm"]    = "application/wasm";
    mime["csv"]     = "text/csv";
    mime["md"]      = "text/markdown";
    mime["php"]     = "application/x-httpd-php";
    mime["exe"]     = "application/octet-stream";

    size_t dot_p = this->URI.find_last_of('.');
    if (dot_p == std::string::npos || this->stat_code != 200)
        return ;
    pp "-> " << _content_type << endl;

    //TODO content0-type on html does not work well
    std::string ext = this->URI.substr(dot_p + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    std::map<std::string, std::string>::iterator it = mime.find(ext);
    if (it != mime.end())
        this->_content_type = it->second;
}


void response::set_location()
{
    this->_location = "";
    if (this->stat_code == 301 || this->stat_code == 302)
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
        return (err_("opendir failed on prepare_autoindex()"), false);
    
    std::stringstream raw_body;
    raw_body    << "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
                << "<title>Directory listing for "<< dir
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
        current = dir + "/" + dp->d_name;
        if (stat(current.c_str(), &s) < 0)
            return (closedir(dirp), err_("stat < 0 on prepare_auto_index()"), false);

        if (S_ISDIR(s.st_mode))
            raw_body << "<li><a href=\"" << dp->d_name << "/\">" << "📁 " << dp->d_name << "</a>" << std::endl;

        else if (S_ISREG(s.st_mode))
            raw_body << "<li><a href=\"" << dp->d_name << "\">" << "📄 " << dp->d_name << "</a>" << std::endl;
    }
    raw_body << "</ul>\n<hr>\n</body>\n</html>\n";
    _body = raw_body.str();
    this->_content_type = "text/html";
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
    struct stat s;
    infile.open(path.c_str(), std::ios::binary);
    if (!infile.is_open() || stat(path.c_str(), &s) || !(s.st_mode & S_IFREG))
    {
        this->stat_code = 500;
        this-> _body = "<!DOCTYPE html>\n"
                        "<html>\n"
                        "    <head><title>500 Internal Server Error</title></head>\n"
                        "    <body>\n"
                        "        <div>\n"
                        "            <h1>500 Internal Server Error</h1>\n"
                        "            <p>The server encountered an unexpected condition that prevented it from fulfilling the request.</p>\n"
                        "        </div>\n"
                        "    </body>\n"
                        "</html>";
        _content_length = _body.size();
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
            return "\177";
        stat_code = _cgi->cgi_get_code();
        if (stat_code == 200)
        {
            int readen = read(_cgi->get_outfd(), this->buffer, 2000);
            // pp string(buff, readen) << endl;
            if (readen > 0)
                return (string(this->buffer, readen));
            return (this->_eof = true,  "");
        }
    }
    if (!_body.empty()) //in this case i am sure that the body is small (indexing a dir / ..)
        return (this->_eof = true, _body);

    infile.read(this->buffer, RW_BUFFER);
    size_t readden = infile.gcount();

    if (!readden)
        return(this->_eof = true, infile.close(), "");
    
    return(std::string(this->buffer, readden));
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
        return;
    else if (this->stat_code == 200)
    {
        if (resource_type == 1)
        {
            if (is_dir_has_index_path())
                prep_body(this->resource_path);
            else if (get_auto_index() && !prepare_autoindex())
            {
		        throw_err_body("prepare_autoindex() failed");
            }
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
    if (locations["default"].error_pages.find(this->stat_code) != locations["default"].error_pages.end())
        prep_body(fix_slash(locations["default"].root,locations["default"].error_pages[this->stat_code]));
    else //the error page not found
    {
        switch (this->stat_code)
        {
            case 400:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>400 Bad Request</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>400 Bad Request</h1>\n"
                            "            <p>The server could not understand the request due to invalid syntax.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 403:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>403 Forbidden</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>403 Forbidden</h1>\n"
                            "            <p>You do not have permission to access this resource.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 404:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>404 Not Found</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>404 Not Found</h1>\n"
                            "            <p>Unable to find a representation of the requested resource.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 405:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>405 Method Not Allowed</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>405 Method Not Allowed</h1>\n"
                            "            <p>The requested method is not supported for the resource.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 409:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>409 Conflict</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>409 Conflict</h1>\n"
                            "            <p>The request could not be completed due to a conflict with the current state of the resource.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 413:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>413 Payload Too Large</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>413 Payload Too Large</h1>\n"
                            "            <p>The request entity is larger than the server is willing or able to process.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 414:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>414 URI Too Long</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>414 URI Too Long</h1>\n"
                            "            <p>The URI requested by the client is longer than the server can interpret.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 501:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>501 Not Implemented</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>501 Not Implemented</h1>\n"
                            "            <p>The server does not support the functionality required to fulfill the request.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            case 504:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>504 Gateway Timeout</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>504 Gateway Timeout</h1>\n"
                            "            <p>The server, while acting as a gateway or proxy, did not receive a timely response from the upstream server.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                break;
            
            default:
                this->_body = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "    <head><title>500 Internal Server Error</title></head>\n"
                            "    <body>\n"
                            "        <div>\n"
                            "            <h1>500 Internal Server Error</h1>\n"
                            "            <p>The server encountered an unexpected condition that prevented it from fulfilling the request.</p>\n"
                            "        </div>\n"
                            "    </body>\n"
                            "</html>";
                pp this->stat_code << " | " << "Default Internal err" << endl;
                this->stat_code = 500;
                break;
        }
        this->_content_length = _body.size();
    }
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
    environ_vars["SERVER_NAME"] = this->server_info.server_name;
    environ_vars["SCRIPT_NAME"] = this->URI ;
    environ_vars["CONTENT_TYPE"] = this->_content_type;

    _content_length = body.size();
    environ_vars["CONTENT_LENGTH"] = wbs::to_string(this->_content_length);
    environ_vars["SCRIPT_FILENAME"] = this->resource_path;
    environ_vars["HTTP_USER_AGENT"] = this->headers["User-Agent"];
    environ_vars["HTTP_COOKIE"] = this->_cookies;
    environ_vars["SERVER_PORT"] = this->server_info.server_port;
	environ_vars["GATEWAY_INTERFACE"] = "CGI/1337";
	environ_vars["SERVER_SOFTWARE"] = "42 webserv (Unix)";
	environ_vars["SERVER_PROTOCOL"] =  "HTTP/1.1";
	environ_vars["REMOTE_ADDR"] = this->server_info.remote_addr;
    environ_vars["QUERY_STRING"] = this->query;
    environ_vars["PATH_INFO"] = this->PATH_INFO;
    //TODO srv port/name are empty

    return (environ_vars);
}

response::response(std::string req, vector<Server> &servers, int p_cfd) : request(req, servers, p_cfd)
{
    this->_eof = false;
    this->_is_cgi = false;
	this->_is_closed = false;
    _content_length = 0;
	_cgi = NULL;
	// this->_server_info = info;
    buffer = new char[RW_BUFFER];
    set_connection();
    set_cookies();
    set_server();
    set_location();
    set_body();
    set_content_type();
    set_content_length();
    if (stat_code != 204)
        this->upload_eof = true;
}

response::~response()
{
    delete buffer;
    infile.close();
	if (_cgi)
    {
        int child_pid = _cgi->get_pid();
        if (child_pid)
            kill(_cgi->get_pid(), SIGKILL);
		delete _cgi;
    }
}
