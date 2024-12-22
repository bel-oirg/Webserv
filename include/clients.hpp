#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include "server.hpp"
#include "cgi.hpp"
#include "response.hpp"


 const char http_response[] =
    "HTTP/1.1 200 OK\n"
    "Accept-Ranges: bytes\r\n"
    "Age: 294510\r\n"
    "Cache-Control: max-age=604800\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
    "Etag: \"3147526947\"\r\n"
    "Expires: Fri, 28 Jun 2024 14:18:33 GMT\r\n"
    "Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT\r\n"
    "Server: ECAcc (nyd/D10E)\r\n"
	"connection: close\r\n"
    "X-Cache: HIT\r\n"
    "Content-Length: [body-lenght] \r\n"
    "\r\n"
    "<!doctype html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
    "    <title>Server Test - It's Working</title>\n"
    "    <style>\n"
    "        body {\n"
    "            font-family: Arial, sans-serif;\n"
    "            text-align: center;\n"
    "            margin-top: 50px;\n"
    "        }\n"
    "        .message {\n"
    "            font-size: 2em;\n"
    "            color: #4CAF50;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div class=\"message\"> Server is Runing!</div>\n"
    "</body>\n"
    "</html>";



class Server;
// class response;

class Client
{
	public:
		Server&		_server;
		string		_request;
		pollfd		_pfd;
		int			cgi_exit_code;
		response*	_response;
		string		_cgi_resp;
		bool		_headers_sended;
		bool		first_response_read;
		time_t		_last_interaction;
		int			_buff_num;
		string		tmp_request;

	public:
		Client(Server &server, int fd);
		Client(const Client &other)	;


		void		save_request(string request);
		pollfd&		get_fd();
		void		change_event();
		string		get_response();
		Client&		operator=(const Client &other);
		void		register_interaction();
		clock_t		get_last_interaction();
        void        change_event(int);
		void		reset()
		{
			if (_response)
				delete _response;
			_response = NULL;
			_headers_sended = false;
			first_response_read = true;
			_last_interaction = 0;
			_buff_num = 0;
			tmp_request.clear();
		}
		
		~Client()
		{
			if (_response)
				delete _response;
		}
};

#endif /* CLIENTS_HPP */
