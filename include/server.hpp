#ifndef SERVER_HPP
#define SERVER_HPP

#include "locations.hpp"

std::string hostToString(in_addr_t host);

class Client;
class ServersManager;


class	Server
{
	public :
		std::vector<pollfd>				server_fds;
		std::map<string, loc_details>	locations;
		uint32_t						port;
		string							server_name;
		in_addr_t						host;
		int 							socket_fd;
		struct sockaddr_in				address;
		int								index;
		pollfd							_pfd;
		uint64_t						_timeout;
		server_info						_server_info;

	public :
		Server();
		Server(const Server& cpy);
		Server& operator= (const Server &cpy);

		std::map<string, loc_details> &get_locations(); 
		void add_client(int fd, ServersManager &manager);
		void setup();
		static void run(std::vector<Server> &servers);
		void	accept_connections(ServersManager &manager);
		// void accept_connections();
		void print() const;
		pollfd& get_poll();
		std::vector<pollfd>	get_fds();
		Client*				get_client_by_fd(int fd);
		bool				erase(int fd);
		server_info		&get_info()
		{
			return (this->_server_info);
		}
};

class ServersManager
{
	private :
		std::vector<Server> servers;
		std::vector<pollfd>	manager_fds;
	public:
		std::map< int, Client* >	client_pool;
		std::vector<pollfd>		servers_pollfds;
		// std::vector<pollfd>		clients_pollfds;

	public :
		vector<pollfd>& get_fds();
		Client& get_client(int fd);



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

