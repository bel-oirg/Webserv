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
		string							server_name;
		time_t							_timeout;

	private :
		in_addr_t						_host;
		int 							socket_fd;
		struct sockaddr_in				address;
		pollfd							_pfd;
		server_info						_server_info;
		bool							_is_up;

	public :
		Server();
		Server(const Server& cpy);
		Server& operator= (const Server &cpy);

		static void 						run(std::vector<Server> &servers);
		std::map<string, loc_details>&		get_locations(); 
		void 								add_client(int fd, ServersManager &manager);
		void 								setup();
		void								accept_connections(ServersManager &manager);
		void								print() const;
		pollfd&								get_poll();
		std::vector<pollfd>					get_fds();
		Client*								get_client_by_fd(int fd);
		bool								erase(int fd);
		server_info&						get_info();
		bool&								up();
		int 								socket();
		void								set_host(in_addr_t host);
		in_addr_t							get_host();
};

class ServersManager
{
	private :
		std::vector<Server>				servers;
		std::vector<pollfd>				manager_fds;
		std::map< int, Client* >		client_pool;
		std::vector<pollfd>				servers_pollfds;

	public :
		vector<pollfd>&		get_targets();
		void				add_client_to_pool(Client *new_client);

	public :
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
};

#endif /* SERVER_HPP */

