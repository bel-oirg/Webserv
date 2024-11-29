#pragma once

#include "request.hpp"
#define CHUNK_SIZE 2048
#define ERR_DIR "/Users/bel-oirg/Desktop/Webserv/Error_pages/"
//TODO change the err pages based on config file

using std::string; 

class response : public request
{
    private:
        int _content_length;

        std::string _status;
        std::string _location; 
        std::string _content_type; 
        std::string _server; 
        std::string _cookies; 
        std::string _connection;
        std::string _transfer_encoding;
        std::string _body;
    
    public:
        response(std::string req, std::map<std::string, loc_details> locations);
        void set_status();
        void set_content_length();
        void set_server();
        void set_location();
        void set_cookies();
        // void set_transfer_encoding();
        void set_connection();
        void set_content_type();
        void set_body();
        std::string get_response();
        bool prepare_autoindex();
        std::map<std::string, std::string>    prepare_cgi();
};