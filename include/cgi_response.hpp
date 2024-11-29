#pragma once

#include <map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

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
        void    set_body(std::string cgi_body);
        void    set_server();
        void    set_connection();
        std::string get_cgi_response();

        cgi_response(std::string body, int cgi_stat_code);
};