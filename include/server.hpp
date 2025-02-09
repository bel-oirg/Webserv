#ifndef SERVER_HPP
#define SERVER_HPP

// #include "clients.hpp"
#include "locations.hpp"

class Client;

class ServersManager;


class	Server
{
	public :
		std::map<string, loc_details>	locations;
		std::vector<pollfd>				server_fds;
		uint32_t						port;
		vector<string>					server_names;
		server_info						_server_info;
		bool							_is_up;

	private :
		in_addr_t						_host;
		int 							socket_fd;
		struct sockaddr_in				address;
		pollfd							_pfd;

	public :
		Server();

		std::map<string, loc_details>&		get_locations(); 
		server_info&						get_info();
		in_addr_t							get_host();
		void 								setup();
		void								print() const;
		int 								socket();
		void								set_host(in_addr_t host);
};

class ServersManager
{
	private :
		std::vector<Server>				servers;
		std::vector<pollfd>				manager_fds;
		std::map< int, Client* >		client_pool;
		int 							poll_timeout;
		std::vector<pollfd>				servers_pollfds;
		char							*reading_buffer;

	public :
		vector<pollfd>&		get_targets();
		void				add_client_to_pool(Client *new_client);

	public :
		ServersManager();
		~ServersManager();
	
		void		init_servers(Server server);
		void		init_servers(std::vector<Server> &new_servers);
		void		check_timeout(pollfd& fd);
		void		setup();
		void		print();
		void		run();
		bool		is_server(int fd);
		void 		accept_connections(pollfd &fd);
		void		get_request(pollfd &pfd);
		void		send_response(pollfd &pfd);
		void		remove_client(int fd);
		void		reset_servers();
};

#endif /* SERVER_HPP */

