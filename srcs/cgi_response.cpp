#include "cgi_response.hpp"
#include "webserv.hpp"
#include "utils.hpp"

void cgi_response::set_status()
{
    std::map<int, std::string> status_map;

    status_map[500] = std::string("Internal Server Error");
    status_map[200] = std::string("200 OK");
    status_map[504] = std::string("Gateway Timeout");
    _status = status_map[cgi_stat_code];
}

cgi_response::cgi_response(int cgi_stat_code) : cgi_stat_code(cgi_stat_code)
{
    // cout <<  "RAW_RESPO--------###" << cgi_body << "ENDHERE-----\n";
    set_status();
    set_server();
    // set_body();
    set_connection();
    // set_content_length();
}

void cgi_response::set_body()
{
    if (cgi_stat_code != 200)
    {
        std::ifstream infile;
        infile.open((string(ERR_DIR) + wbs::to_string(cgi_stat_code) + string(".html")).c_str());
        if (!infile)
        {
            std::cerr << "Error opening error_file on CGI" << std::endl;
            return;
        }
        std::stringstream ss;
        ss << infile.rdbuf();
        _body = ss.str();
    }
    _content_type = "text/html";
}

void    cgi_response::set_content_length()
{
    _content_length = _body.size();
}

void cgi_response::set_server()
{
    this->_server = "CGI Webserv 2.0";
}

void cgi_response::set_connection()
{
    this->_connection = "close";
}

std::string cgi_response::get_cgi_response(int fdout)
{
    std::stringstream   line;
    line << "HTTP/1.1 " << _status << "\r\n";
    line << "Connection: " << this->_connection << "\r\n";
    line << "Server: " << this->_server << "\r\n";
    line << "Content-Type: " << "text/html" << "\r\n";

    line << "Content-Length: " << lseek(fdout, 0, SEEK_END) << "\r\n";

    line << "\r\n";
    // line << _body;

    lseek(fdout, 0, SEEK_SET);
    
    return (line.str());
}
