
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


// bool	is_server(vector<Config> servers, int index)
// {
// 	for (int i = 0; i < servers.size(); i++)
// 	{
// 		if ( Config::fds[index].fd == servers[i].get_socket())
// 			return (true);
// 	}
// 	return  false;
// }


// void	serve(std::vector<Config> &servers, int index)
// {
// 	for (int i = 0 ; i < servers.size(); i++)
// 	{
// 		// Config cur_server = servers[i];
// 		if (Config::fds[index].fd == servers[i].get_socket() && Config::fds[index].revents & POLLIN)
// 		{
// 			servers[i].accept_connections();
// 			cout << "new connections" << endl;
// 		}
// 		else if (!is_server(servers, index) && Config::fds[index].revents & POLLIN)
// 		{
// 			char buffer[8192] = {0};
// 			int valread = read(Config::fds[index].fd, buffer, 8192);

// 			if (valread == 0)
// 			{
// 				std::cout << "Client disconnected" << std::endl;
// 				close(Config::fds[index].fd);
// 			}
// 			else
// 			{
// 				cout << buffer << endl;
// 				send(Config::fds[index].fd, (void*)http_response, sizeof(http_response), 0);
// 				close(Config::fds[index].fd);
// 				cout << "data sended" << endl;
// 			}
// 		}
// 	}
// }

// void	init(std::vector<Config> servers)
// {
// 	for (int i = 0; i < servers.size(); i++)
// 	{
// 		struct pollfd tmp = {0};
		
// 		servers[i].set_socket(socket(AF_INET, SOCK_STREAM, 0));

// 		int flags = fcntl(servers[i].get_socket(), F_GETFL, 0);
// 		flags = fcntl(servers[i].get_socket(), F_SETFL, flags | O_NONBLOCK);
		
// 		if (flags == -1)
// 			perror ("faild to fcntl()");

// 		servers[i].address.sin_family = AF_INET;
// 		servers[i].address.sin_port = htons(servers[i].get_port());
// 		servers[i].address.sin_addr.s_addr = servers[i].get_host();

// 		int bret = bind(servers[i].get_socket(), servers[i].get_address(), servers[i].get_socklen());
// 		if (bret < 0)
// 			perror ("87"), exit(errno);

// 		int lsn = listen(servers[i].get_socket(), 5);
// 		if (lsn < 0)
// 			perror ("faild to listen()");

// 		tmp.events = POLLIN;
// 		tmp.fd = servers[i].get_socket();
// 		tmp.revents = 0;

// 		Config::fds[Config::numfds] = tmp;
// 		Config::numfds++;

// 	}
	
// 		clog << "ALL servers created secsesfully" << endl;


// 		while (true)
// 		{
// 			int ret = poll(Config::fds, Config::numfds, -1);
// 			if (ret == 0)
// 				throw("poll() timed out");
// 			for (size_t index = 0; index < Config::numfds; index++)
// 			{
// 				if (Config::fds[index].revents == 0)
// 					continue;
// 				serve( servers, index);
// 			}
// 		}

// }


int	main(/*int argc, char **argv*/)
{
		std::vector<Server>	servers;
		std::string		filename("./test.conf");



		servers = Parse::get_servers(filename);

		for (size_t i = 0; i < servers.size(); i++)
		{
			servers[i].index = i;
			servers[i].setup();
		}

		Server::run(servers);
}