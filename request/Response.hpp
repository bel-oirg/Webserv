#pragma once

#include "Request.hpp"
#define CHUNK_SIZE 2048

class response : public request
{
    private:
        std::unordered_map<int, std::string> status;
        int stat_code;
        int _content_length; 

        std::string _location; 
        std::string _content_type; 
        std::string _server; 
        std::string _connection;
        std::string _transfer_encoding;
        std::string _body;
    
    public:
        response(std::string req);
        void fill_status();
        void set_content_length();
        void set_server();
        void set_location();
        // void set_transfer_encoding();
        void set_connection();
        void set_content_type();
        void set_body();
        std::string the_head();
};