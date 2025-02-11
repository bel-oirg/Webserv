#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "request.hpp"
#include "cgi.hpp"

void     err_(const std::string &err);

class response : public request
{
    private:
        uint64_t _content_length;
        char            *buffer;

        string          _status;
        string          _location; 
        string          _content_type; 
        string          _server; 
        string          _cookies; 
        string          _connection;
        string          _body;
        std::ifstream   infile;
        map<string, string> _cgi_headers;
        
        Cgi             *_cgi;
        string          _cgi_head;
        string          _cgi_str;
    
    public:
        bool            _is_cgi;
		bool			_is_closed;
        bool            _eof;
        response(string req, vector<Server> &servers, int);
        ~response();
        static std::string set_status(int stat_code);
        void set_content_length();
        void set_server();
        void set_location();
        void set_cookies();
        void set_connection();
        void set_content_type();
        void debug();

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
