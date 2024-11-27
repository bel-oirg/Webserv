#pragma once

#include "request.hpp"
#define CHUNK_SIZE 2048

using std::string; 

class response : public request
{
    private:
        std::map<int, std::string> status;
        int _content_length;

        std::string _location; 
        std::string _content_type; 
        std::string _server; 
        std::string _connection;
        std::string _transfer_encoding;
        std::string _body;
    
    public:
        response(std::string req, std::string &http_response, std::map<std::string, loc_details> locations);
        void fill_status();
        void set_content_length();
        void set_server();
        void set_location();
        // void set_transfer_encoding();
        void set_connection();
        void set_content_type();
        void set_body();
        bool prepare_autoindex();
        std::string the_head();
};