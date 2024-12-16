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
    status_map[200] = std::string("200 OK");
    status_map[201] = std::string("201 Created"); //mainly it returns json of the uploaded data
    status_map[204] = std::string("204 No Content");
    status_map[301] = std::string("301 Moved Permanently");
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
    _status = status_map[stat_code];
}

std::string response::get_response_header() //_____SEND__RESP__HEAD
{
    if (this->stat_code == -1)
        return (pp "GOTO CGI\n", "CGI");
    
    std::stringstream   line;
    
    line << "HTTP/1.1 " << _status << "\r\n";

    if (!this->_location.empty())
        line << "Location: " << this->_location << "\r\n";

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

    line << "\r\n";

    return (line.str());
}

void response::set_content_length()
{
    if (!_body.empty())
        _content_length = _body.size();
    else if (_content_length != -1)
        _content_length = file_size;
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

    if (!this->_body.size() && !this->file_size)
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
        if (add_slash)
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

inline void     err_(const std::string &err) //TODO dup in request.cpp
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
        this->stat_code = 501;
        std::cerr << "Error opening " << path << std::endl;
        return (false);
    }

    infile.seekg(0, ios::end);
    file_size = infile.tellg();
    infile.seekg(0);
    return (true);
}

string response::get_to_send() //_____RESP_BODY_SEND__
{
    if (!_body.empty()) //in this case i am sure that the body is small (indexing a dir / ..)
        return (this->_eof = true, _body);

    char buff[CHUNK_SIZE + 1] = {0};
    infile.read(buff, CHUNK_SIZE);

    // if (file_size > CHUNK_SIZE) //in this case we will use always CHUNKED
    // {
    //     _content_length = -1;
    //     std::streamsize file_len = infile.gcount();
    //     if (!file_len)
    //         return(infile.close(), "0\r\n\r\n");

    //     ss << std::hex << file_len << "\r\n";
    // }
   size_t readden = infile.gcount(); // else if

   if (!readden)
        return(this->_eof = true, infile.close(), "");
    
    return(std::string(buff, readden));
}

//TODO chunked body with indexed

void response::set_body()
{
    _body = "";
    if (this->stat_code == 301)
        return;

    if (this->stat_code / 400)
    {
        if (current_loc.error_pages.find(this->stat_code) != current_loc.error_pages.end())
            prep_body(current_loc.root + current_loc.error_pages[this->stat_code]); //BUG CPP11
        else
            prep_body(ERR_DIR + std::to_string(this->stat_code) + ".html"); //BUG CPP11
    }
    else if (this->stat_code == 204)
        _body = "Content Uploaded Successfully"; //TODO 204 returns nothing
    else if (this->stat_code == 200)
    {
        if (resource_type == 1 && !current_loc.index_path.size() && get_auto_index())
        {
            if (!prepare_autoindex())
                this->stat_code = 501;
        }
        else
            prep_body(this->resource_path);
    }
    else
    {
        err_("stat_code unknown");
        stat_code = 501;
    }
}

std::map<std::string, std::string>    response::prepare_cgi(Server &server)
{
    std::map<std::string, std::string> environ_vars;
    environ_vars["REQUEST_METHOD"] = this->method;
    environ_vars["SERVER_NAME"] = this->_server;
    environ_vars["SCRIPT_NAME"] = this->URI ;
    environ_vars["CONTENT_TYPE"] = this->_content_type;
    environ_vars["CONTENT_LENGTH"] = to_string(this->_content_length); //BUG CPP11 TODO THIS IS WRONG
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
    this->_eof = false;
    set_status(); //JUST REP
    set_connection(); //JUST REP
    set_server(); //JUST REP
    set_cookies(); //JUST REP
    set_location(); //JUST REP
    set_body();
    set_content_type();
    set_content_length();
    set_transfer_encoding();
    if (stat_code != 204)
        this->upload_eof = true;
}

/*
TODO Set a default file to answer if the request is a directory.
TODO configure where they should be saved.
*/