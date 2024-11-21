#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>


class response
{
    private:
        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<int, std::string> status;
        std::string resource_path;
        std::string method;
        int stat_code;

        std::string _content_length; 
        std::string _location; 
        std::string _content_type; 
        std::string _host;
        std::string _server; 
        std::string _connection;
        std::string _transfer_encoding;
    
    public:
        void fill_status();
        void set_content_length();
        void set_server();
        void set_location();
        void set_content_type();

};

