
#include <sys/socket.h>
#include <assert.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <poll.h>
#include "server.hpp"
#include "config.hpp"
#include "parse.hpp"
#include <deque>


using namespace std;

// static struct pollfd 			fds[1000];
// static int						numfds;


#define NO_EVENT 0


int	main(/*int argc, char **argv*/)
{
		std::vector<Server>	servers;
		std::map<int , Server>	servers_as_map;
		std::string		filename("./test.conf");



		servers = Parse::get_servers(filename);


		for (size_t i = 0; i < servers.size(); i++)
		{
			servers[i].setup();
		}

		// TODO tmp until fixing get_servers func

		for (int i = 0; i < servers.size(); i++)
		{
			servers_as_map[servers[i].socket_fd] = servers[i];
			servers_as_map[servers[i].socket_fd].print();
		}


		Server::run(servers_as_map);
}