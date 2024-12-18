#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "request.hpp"
#include "cgi.hpp"
#define CHUNK_SIZE 1024

#define	ERR_DIR "/Users/abennar/Desktop/lastpush/Error_pages/"

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
        
        Cgi             _cgi;
        bool            _is_cgi;
        string          _cgi_head;
        string          _cgi_str;
    
    public:
        bool            _eof;
        response(string req, std::map<string, loc_details> locations);
        ~response();
        static std::string set_status(int stat_code);
        void set_content_length();
        void set_server();
        void set_location();
        void set_cookies();
        void set_transfer_encoding();
        void set_connection();
        void set_content_type();

        bool prep_body(const string &path);
        bool prep_cgi();
        void set_body();
		string get_body();
        string get_to_send();

		string get_script_path();
        string get_response_header();
        bool prepare_autoindex();
        std::map<string, string>    prepace_env_cgi(/*Server &server*/);
};


#endif /* RESPONSE_HPP */
