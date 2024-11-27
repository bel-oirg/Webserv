// #include "../Request/Response.hpp"
#include "server.hpp"
#include "config.hpp"
#include "Response.hpp"

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
			char buffer[8192] = {0};
			string readed_request;

			int valread;

			// while (true)
			// {
				valread = read(fd, buffer, 8190); // TODO protect
				// if (valread == 0)
				// 	break;
				buffer[valread] = '\0';
				readed_request += buffer;
				// p buffer << std::endl;
			// }


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
				string http_response;
				p  "\t\t\t\tAAAAAAAA" << servers[0].locations["default"].root << std::endl;
				response resp(buffer, http_response ,servers[0].locations);//, servers); 

				send(fd, (void *) http_response.c_str(), http_response.size(), 0);
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

