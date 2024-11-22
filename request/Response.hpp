#pragma once

#include "../Request/request.hpp"

class response : public request
{
    private:
        std::unordered_map<int, std::string> status;
        std::string resource_path;
        int stat_code;
        int _content_length; 

        std::string _location; 
        std::string _content_type; 
        std::string _server; 
        std::string _connection;
        std::string _transfer_encoding;
    
    public:
        response();
        void fill_status();
        void set_content_length();
        void set_server();
        void set_location();
        void set_transfer_encoding();
        void set_connection();
        void set_content_type();
        std::string the_head();
};