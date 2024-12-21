#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "request.hpp"
#include "cgi.hpp"

void     err_(const std::string &err);

class response : public request
{
    private:
        int _content_length; //change 

        string          _status;
        string          _location; 
        string          _content_type; 
        string          _server; 
        string          _cookies; 
        string          _connection;
        string          _transfer_encoding;
        string          _body;
        std::ifstream   infile;
        size_t          _file_size;
		server_info		_server_info;
        
        Cgi             *_cgi;
        bool            _is_cgi;
        string          _cgi_head;
        string          _cgi_str;
    
    public:
		bool			_is_closed;
        bool            _eof;
        response(string req, std::map<string, loc_details> locations, server_info);
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
        void _20X();
        void _40X_50X();
        bool prep_cgi();
        void throw_err_body(string err);
        void set_body();
		string get_body();
        string get_to_send();

		string get_script_path();
        string get_response_header();
        bool prepare_autoindex();
        std::map<string, string>    prepare_env_cgi(/*Server &server*/);
};


#endif /* RESPONSE_HPP */
