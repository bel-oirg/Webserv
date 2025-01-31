#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include "server.hpp"
#include "cgi.hpp"
#include "response.hpp"

class Server;


class Client
{
	public :
		response*	_response;
		bool		_headers_sended;
		bool		first_response_read;

	private :
		Server&		_server;
		string		_request_buffer;
		pollfd		_pfd;
		time_t		_last_interaction;
		char		*reading_buffer;


	public:
		Client(Server &server, int fd);
		Client(const Client &other);
		Client&		operator=(const Client &other);
		~Client();


		Server&		server();
		pollfd&		get_pollfd();
		void		save_request(string request);
        void        change_event(int);
		void		change_event();
		void		register_interaction();
		time_t		get_last_interaction();
		string		&request_buffer();
};

#endif /* CLIENTS_HPP */
