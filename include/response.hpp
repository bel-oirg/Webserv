#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "request.hpp"
#define CHUNK_SIZE 1024
// #define ERR_DIR "/Users/bel-oirg/Desktop/tmptmp/Error_pages/"
#define	ERR_DIR "/Users/abennar/Desktop/push/Error_pages/"
//TODO change the err pages based on config file

using std::string; 

class response : public request
{
    private:
        int _content_length;

        string          _status;
        string          _location; 
        string          _content_type; 
        string          _server; 
        string          _cookies; 
        string          _connection;
        string          _transfer_encoding;
        string          _body;
        std::ifstream   infile;
        size_t          file_size;
    
    public:
        bool            _eof;
        response(string req, std::map<string, loc_details> locations);
		response();
		// void init(string req, std::map<string, loc_details> locations);
        void set_status();
        void set_content_length();
        void set_server();
        void set_location();
        void set_cookies();
        void set_transfer_encoding();
        void set_connection();
        void set_content_type();

        bool prep_body(const string &path);
        void set_body();
		string get_body();
        string get_to_send();

		string get_script_path();
        string get_response_header();
        bool prepare_autoindex();
        std::map<string, string>    prepare_cgi(Server &server);
};


#endif /* RESPONSE_HPP */
