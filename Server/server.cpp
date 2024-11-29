// #include "../Request/Response.hpp"
#include "server.hpp"
#include "config.hpp"
#include "response.hpp"

Pollfd Server::pool;
map<int, string> Server::responses;

#define REQUEST_MAX_SIZE 3000000


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

	Server::pool.add_server(this->socket_fd);
}

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
		Server::pool.add_client(ret, this->socket_fd);
	}
}


void get_request(pollfd &pfd, std::vector<Server> &servers)
{
	cout << "SERVER ENTRED" << endl;
	char buffer[REQUEST_MAX_SIZE] = {0};

	int valread;
	valread = read(pfd.fd, buffer, REQUEST_MAX_SIZE - 1);
	if (valread == -1)
	{
		perror("read() failed");
		exit(1);
	}
	else if (valread >= REQUEST_MAX_SIZE)
	{
		std::cout << "request lenght too large : action => discarding" << std::endl;
		Server::pool.remove(pfd.fd);
		return;
	}
	cout << "READ PASS" << endl;
	if (valread == 0)
	{
		std::cout << "Client disconnected" << std::endl;
		Server::pool.remove(pfd.fd);
	}
	else
	{
		std::string response_http;
		std::map<string, string> CGI_RAW;
		Server &cur_server = Server::pool.get_server(pfd.fd, servers);

		response resp(buffer, cur_server.locations);
		p "------REQ-----\n" << buffer << std::endl;

		response_http = resp.get_response();
		if (response_http == "CGI")
			CGI_RAW = resp.prepare_cgi();

		Server::responses.insert(make_pair(pfd.fd, response_http));
		Server::pool.change_event(pfd.fd, POLLOUT);
	}
}
// CGI -> crash -> try catch -> obj(crash, resp);
void	send_response(pollfd &pfd, std::vector<Server> &servers)
{
	Server cur_server;

	string  response = Server::responses[pfd.fd];
	int send_ret = send(pfd.fd, (void *)response.c_str(), response.size(), 0);
	if (send_ret == -1)
	{
		perror ("send() failed");
	}
	Server::responses.erase(pfd.fd);

	// TODO check if the client asks for closing connection todo so
	// now closing any way 
	Server::pool.remove(pfd.fd);
}

void Server::run(std::vector<Server> &servers)
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
					for (int j = 0; j < servers.size(); j++)
					{
						if (servers[j].socket_fd == fds[i].fd)
							servers[j].accept_connections();
					}
				}
				else
				{
					get_request(fds[i], servers);
				}
			}
			else if (fds[i].revents & POLLOUT)
			{
				send_response(fds[i], servers);
			}
		}
	}
}
