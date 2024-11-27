// #include "../Request/Response.hpp"
#include "server.hpp"
#include "config.hpp"
#include "Response.hpp"

Pollfd Server::pool;

void Server::accept_connections()
{
	for (;;)
	{
		struct pollfd tmp = {0};
		socklen_t address_size = sizeof(this->address);
		int ret = accept(this->socket_fd, (sockaddr *)&(this->address), &address_size);
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
		cout << "fd connection: " << ret << endl;
	}
}

#define REQUEST_MAX_SIZE 40000

void serve(int fd, std::map<int, Server> &servers)
{
	char buffer[REQUEST_MAX_SIZE] = {0};
	string readed_request;

	int valread;
	valread = read(fd, buffer, REQUEST_MAX_SIZE - 1);
	if (valread == -1)
	{
		perror("read() failed");
		exit(1);
	}
	else if (valread >= REQUEST_MAX_SIZE)
	{
		std::cout << "request lenght too large : action => discarding" << std::endl;
		Server::pool.remove(fd);
		return;
	}
	cout << "READ PASS" << endl;
	if (valread == 0)
	{
		std::cout << "Client disconnected" << std::endl;
		Server::pool.remove(fd);
	}
	else
	{
		buffer[valread] = '\0';
		cout << buffer << endl;
		string http_response;
		// get the server index that responsible to serve the client
		int server_index = Server::pool.get_server_index(fd);
		cout << "THE SERVER FOR : " << fd << "IS : " << server_index << endl;
		if (server_index == -1)
		{
			clog << "can't find client fd in pool : action => closing connection" << endl;
			Server::pool.remove(fd);
		}
		response resp(buffer, http_response, servers[0].locations);


		send(fd, (void *)http_response.c_str(), http_response.size(), 0);
		Server::pool.remove(fd);
		cout << "data sended" << endl;
	}
}

void Server::setup()
{
	struct pollfd tmp = {0};

	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_fd == -1)
	{
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	int fd_flags = fcntl(this->socket_fd, F_GETFL, 0);
	if (fd_flags == -1)
	{
		perror("fcntl( F_GETFL ) failed");
		exit(EXIT_FAILURE);
	}
	fd_flags = fcntl(this->socket_fd, F_SETFL, fd_flags | O_NONBLOCK);
	if (fd_flags == -1)
	{
		perror("fcntl( F_SETFL ) failed");
		exit(EXIT_FAILURE);
	}

	this->address.sin_family = AF_INET;
	this->address.sin_port = htons(this->port);
	this->address.sin_addr.s_addr = this->host;

	int bind_t = ::bind(this->socket_fd, (const struct sockaddr *)&(this->address), sizeof(this->address));
	if (bind_t == -1)
	{
		perror("dind() failed");
		exit(EXIT_FAILURE);
	}

	int listen_t = listen(this->socket_fd, 1000);
	if (listen_t == -1)
	{
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}

	Server::pool.add(this->socket_fd, this->socket_fd);
}

void Server::run(std::map<int , Server> &servers)
{
	while (true)
	{
		std::vector<pollfd> fds = Server::pool.data();
		int ret = poll(fds.data(), Server::pool.size(), -1);
		if (ret == -1)
		{
			perror("poll() failed");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (Server::pool.is_server(fds[i].fd))
				{
					for (auto server_iter = servers.begin(); server_iter != servers.end(); server_iter++)
					{
						// cout << FD
						// if (server_iter->first == fds[i].fd)
						// {
						// 	cout << " Server Event Found. "  << endl;
						// 	server_iter->second.accept_connections();
						// }
					}
				}
				else
				{
					;;// serve(fds[i].fd, servers);
				}
			}
			// cout << "IN LOOP" << endl;
		}
	}
}
