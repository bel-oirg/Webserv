#include "../Request/Response.hpp"
#include "server.hpp"
#include "config.hpp"

Pollfd			Server::pool;


void	Server::accept_connections()
{
	for ( ;; )
	{
		struct pollfd tmp = {0};
		socklen_t	address_size = sizeof(this->address);
		int ret = accept(this->socket_fd, (sockaddr *) &(this->address), &address_size);
		if (ret < 0) 
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) 
			{
				std::clog << "no more clients" << endl;
            	break;
        	}
		 	else 
		 	{
            	perror("accept() failed");
            	exit(EXIT_FAILURE);
        	}
		}
			Server::pool.add(this->socket_fd, ret);
			cout << "fd connection: "  << ret << endl;
	}
}


 void	serve(int fd, std::vector<Server> &servers)
{
	// for (int i = 0 ; i < servers.size(); i++)
	// {
		// Config cur_server = servers[i];
		// if (Server::pool.is_server(fd))
		// {
		// 	servers[i].accept_connections();
		// 	cout << "new connections" << endl;
		// }
			char buffer[8192] = {0};
			int valread = read(fd, buffer, 8192);
			if (valread == 0)
			{
				std::cout << "Client disconnected" << std::endl;
				Server::pool.remove(fd);
			}
			else
			{
				cout << buffer << endl;
				/*
					req here --> resp
				*/
				response resp(buffer); 

				send(fd, (void*)http_response, sizeof(http_response), 0);
				Server::pool.remove(fd);
				cout << "data sended" << endl;
			}
	// 	}
	// }
}


void	Server::setup()
{
		struct pollfd tmp = {0};
		
		this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (this->socket_fd == -1)
		{
			perror("socket() faild");
			exit(EXIT_FAILURE);
		}

		int fd_flags = fcntl(this->socket_fd, F_GETFL, 0);
		if (fd_flags == -1)
		{
			perror("fcntl( F_GETFL ) faild");
			exit(EXIT_FAILURE);
		}
		fd_flags = fcntl(this->socket_fd, F_SETFL, fd_flags | O_NONBLOCK);
		if (fd_flags == -1)
		{
			perror("fcntl( F_SETFL ) faild");
			exit(EXIT_FAILURE);
		}

		this->address.sin_family = AF_INET;
		this->address.sin_port = htons(this->port);
		this->address.sin_addr.s_addr = this->host;

		int bind_t = ::bind(this->socket_fd, (const struct sockaddr *) &(this->address), sizeof(this->address));
		if (bind_t == -1)
		{
			perror ("dind() faild");
			exit(EXIT_FAILURE);
		}

		int listen_t = listen(this->socket_fd, 1000);
		if (listen_t == -1)
		{
			perror ("listen() faild");
			exit(EXIT_FAILURE);
		}

		Server::pool.add(this->socket_fd, socket_fd);
}

void	Server::run(std::vector<Server> &servers)
{
	cout << "num of fds :" << Server::pool.size() << endl;
	while (true)
	{
		std::vector<pollfd> fds = Server::pool.data();
		int ret = poll(fds.data(), Server::pool.size(),  -1);
		if (ret == -1)
		{
			perror ("poll() faild");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (Server::pool.is_server(fds[i].fd))
				{
					for (int j = 0; j < servers.size() ; j++)
					{
						if (servers[j].socket_fd == fds[i].fd)
							servers[j].accept_connections();
					}
				}
				else
					serve(fds[i].fd, servers);
			}
		}
	}
}

