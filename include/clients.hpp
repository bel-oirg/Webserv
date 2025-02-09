#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include "server.hpp"
#include "cgi.hpp"
#include "response.hpp"

class Server;


class Client
{
	public :
		response*		_response;
		bool			_headers_sended;
		bool			first_response_read;
		bool			handshake;
		int				entry_port;
		vector<Server>	*servers;

	private :
		string			_request_buffer;
		pollfd			_pfd;
		time_t			_last_interaction;


	public:
		Client(int fd, vector<Server> *servers, int port);
		~Client();


		Server&		server();
		pollfd&		get_pollfd();
		void		save_request(string request);
		void		listen_to_write();
		void		register_interaction();
		time_t		get_last_interaction();
		string		&request_buffer();
};

#endif /* CLIENTS_HPP */
