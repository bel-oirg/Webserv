
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
		std::string		filename("./test.conf");



		servers = Parse::get_servers(filename);

		for (size_t i = 0; i < servers.size(); i++)
		{
			servers[i].index = i;
			servers[i].print();
			servers[i].setup();

		}


		Server::run(servers);
}