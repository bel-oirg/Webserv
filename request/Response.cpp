#include "response.hpp"

string response::get_script_path()
{
	return (this->resource_path);
}

string response::get_body()
{
	return (this->_body);
}

void response::set_status()
{
    static std::map<int, std::string> status_map;
    status_map[200] = std::string("OK");
    status_map[201] = std::string("Created");
    status_map[204] = std::string("No Content");
    status_map[301] = std::string("Moved Permanently");
    status_map[400] = std::string("Bad Request");
    status_map[403] = std::string("Forbidden");
    status_map[404] = std::string("Not Found");
    status_map[405] = std::string("Method Not Allowed");
    status_map[409] = std::string("Conflict");
    status_map[413] = std::string("Content Too Large");
    status_map[414] = std::string("URI Too Long");
    status_map[500] = std::string("Internal Server Error");
    status_map[501] = std::string("Not Implemented");
    status_map[504] = std::string("Gateway Timeout");
    _status = status_map[stat_code];
}

std::string response::get_response()
{
    if (this->stat_code == -1)
        return (p "GOTO CGI\n", "CGI");
    std::stringstream   line;
    line << "HTTP/1.1 " << _status << "\r\n";
    line << "Connection: " << this->_connection << "\r\n";
    line << "Server: " << this->_server << "\r\n";
    line << "Content-Type: " << this->_content_type << "\r\n";
    if (this->_content_length == -1)
        line << "Transfer-Encoding: " << this->_transfer_encoding << "\r\n";
    else
        line << "Content-Length: " << this->_content_length << "\r\n";

    if (!_cookies.empty())
        line << "Set-Cookie: " << _cookies << "\r\n";
        
    line << "\r\n";
    line << _body;

    // p "------RESP-----\n" << line.str() << std::endl;
    return (line.str());
}

void response::set_content_length()
{
    if (_content_length != -1)
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
    if (this->stat_code == 304)
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
    mime["atom"] = "application/atom+xml";
    mime["rss"] = "application/rss+xml";
    mime["txt"] = "text/plain";
    mime["png"] = "image/png";
    mime["json"] = "application/json";

    _content_type = "text/html";
    size_t dot_p = this->URI.find_last_of('.');
    if (dot_p == std::string::npos)
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
    if (this->stat_code == 301)
    {
        if (add_slash)
            this->_location = this->resource_path + "/";
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
            return (perror(NULL), false);

        if (S_ISDIR(s.st_mode))
            raw_body << "<li><a href=\"" << dp->d_name << "/\">" << dp->d_name << "</a>" << std::endl;
        else if (S_ISREG(s.st_mode))
            raw_body << "<li><a href=\"" << dp->d_name << "\">" << dp->d_name << "</a>" << std::endl;
    }
    raw_body << "</ul>\n<hr>\n</body>\n</html>\n";
    _body = raw_body.str();
    return (true);
}

inline void     err_(const std::string &err) //TODO dup in request.cpp
{
    std::cerr << "[-] " << err << std::endl;
}

void response::fill_body(const std::string &path)
{
    std::ifstream infile(path, std::ios::binary);

    if (!infile)
    {
        this->stat_code = 501;
        std::cerr << "Error opening error_page.html" << std::endl;
        return;
    }
    std::stringstream ss;

    infile.seekg(0, ios::end);
    size_t file_size = infile.tellg();
    infile.seekg(0);

    if (file_size > CHUNK_SIZE)
    {
        _content_length = -1;
        while(1)
        {
            char buff[CHUNK_SIZE + 1] = {0};
            infile.read(buff, CHUNK_SIZE);
            std::streamsize file_len = infile.gcount();
            if (!file_len)
                break;
            ss.clear();
            ss.str("");

            ss << std::hex << file_len << "\r\n";
            ss << buff << "\r\n";

            _body += ss.str();
        }
        _body += "0\r\n\r\n";
    }
    else
    {   
        ss << infile.rdbuf();
        _body = ss.str();
    }
}

//TODO chunked body with indexed


void response::set_body()
{
    _body = "";
    if (this->stat_code / 300)
        fill_body(ERR_DIR + std::to_string(this->stat_code) + ".html");
    else if (this->stat_code == 204)
        _body = "Content Uploaded Successfully";
    else if (this->stat_code == 200)
    {
        int res = get_request_resource();

        if (res == 1 && get_auto_index())
        {
            if (!prepare_autoindex())
                this->stat_code = 501;
        }
        else
            fill_body(this->resource_path);
    }
}

std::map<std::string, std::string>    response::prepare_cgi(Server &server)
{
    std::map<std::string, std::string> environ_vars;
    environ_vars["REQUEST_METHOD"] = this->method;
    environ_vars["SERVER_NAME"] = this->_server;
    environ_vars["SCRIPT_NAME"] = this->URI ;
    environ_vars["CONTENT_TYPE"] = this->_content_type;
    environ_vars["CONTENT_LENGTH"] = this->_content_length;
    environ_vars["SCRIPT_FILENAME"] = this->resource_path;
    environ_vars["HTTP_USER_AGENT"] = this->headers["User-Agent"];
    environ_vars["HTTP_COOKIE"] = this->_cookies;
    environ_vars["SERVER_PORT"] = to_string(server.port); // BUG c++ 11
	environ_vars["GATEWAY_INTERFACE"] = "CGI/1337";
	environ_vars["SERVER_SOFTWARE"] = "42 webserv (Unix)";
	environ_vars["SERVER_PROTOCOL"] =  "HTTP/1.1";
	environ_vars["REMOTE_ADDR"] = hostToString(server.host);
	environ_vars["REMOTE_HOST"] = server.server_name;
    environ_vars["QUERY_STRING"] = this->query;


    return (environ_vars);
}

response::response(std::string req, std::map<string, loc_details> locations) : request(req, locations)
{
    set_status();
    set_connection();
    set_content_type();
    set_server();
    set_cookies();
    set_body();
    set_content_length();
    set_transfer_encoding();
}
