#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include "locations.hpp"

std::string hostToString(in_addr_t host);

using namespace std;

class Client;

class	Server
{
	public :
		std::vector<Client>				clients;
		std::vector<pollfd>				server_fds;
		std::map<string, loc_details>	locations;
		// uint16_t						port;
		uint64_t						port;
		string							server_name;
		in_addr_t						host;
		int 							socket_fd;
		struct sockaddr_in				address;
		int								index;
		pollfd							_pfd;

	public :
		Server();
		std::map<string, loc_details> &get_locations(); 
		void setup();
		static void run(std::vector<Server> &servers);
		void accept_connections();
		Server& operator= (const Server &cpy);
		Server(const Server &cpy);
		void print() const;
		pollfd& get_poll();
		std::vector<pollfd>	get_fds();
		Client*				get_client_by_fd(int fd);
		bool				erase(int fd);
};

class ServersManager
{
	private :
		std::vector<Server> servers;
		std::vector<pollfd>	manager_fds;

	public :
		void init_servers(Server server);
		void init_servers(std::vector<Server> &new_servers);
		void setup();
		void print();
		void run();
		vector<pollfd>& get_fds();
		bool is_server(int fd);
		void accept_connections();
		Client& get_client(int fd);
		void get_request(pollfd &pfd);
		void send_response(pollfd &pfd);
		void	remove_client(int fd);
};

#endif /* SERVER_HPP */

