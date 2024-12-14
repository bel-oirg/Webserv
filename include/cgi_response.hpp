#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include <map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>

// #define CGI_ERR_DIR "/Users/bel-oirg/Desktop/Webserv/Error_pages/"
#define CGI_ERR_DIR "/Users/abennar/Desktop/push/Error_pages/"

class cgi_response
{
    private:
        int cgi_stat_code;
        int _content_length;
        std::string _status;
        std::string _body;
        std::string _server;
        std::string _connection;
        std::string _content_type;

    public:
        void    set_status();
        void    set_content_length();
        void    set_body(); //it set also the content type
        void    set_server();
        void    set_connection();
        std::string get_cgi_response();

        cgi_response(std::string body, int cgi_stat_code);
};


#endif /* CGI_RESPONSE_HPP */
