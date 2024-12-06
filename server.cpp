#include "webserv.hpp"
#include "locations.hpp"
// #include "config.hpp"
#include "clients.hpp"
#include "server.hpp"
#include "cgi_response.hpp"

using namespace std;


std::map<string, loc_details> &Server::get_locations()
{
	return (this->locations);
}

Server::Server() : port(0), server_name(""), host(0) {}

Server &Server::operator=(const Server &cpy)
{
	if (this != &cpy)
	{
		port = cpy.port;
		server_name = cpy.server_name;
		host = cpy.host;
		locations = cpy.locations;
	}
	return *this;
}

Server::Server(const Server &cpy)
{
	*this = cpy;
}

void Server::print() const
{
	std::cout << "Server Name: " << server_name << std::endl;
	std::cout << "Port: " << port << std::endl;
	std::cout << "Host: " << hostToString(host) << std::endl;
	std::cout << "Locations:" << std::endl;
	for (std::map<std::string, loc_details>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		std::cout << "  Location: " << it->first << std::endl;
		it->second.print();
	}
}

// pollfd &Server::get_poll()
// {
// 	_pfd.fd = socket_fd;
// 	_pfd.events = POLLIN;
// 	_pfd.revents = 0;
// 	return (_pfd);
// }

std::vector<pollfd> Server::get_fds()
{
	server_fds.clear();
	server_fds.reserve(clients.size() + 1);
	for (int i = 0; i < clients.size(); i++)
	{
		server_fds.push_back(clients[i].get_fd());
	}
	server_fds.push_back(this->_pfd);
	return (server_fds);
}

Client *Server::get_client_by_fd(int fd)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].get_fd().fd == fd)
			return (&clients[i]);
	}
	return NULL;
}

void ServersManager::init_servers(Server server)
{
	servers.push_back(server);
}

void ServersManager::init_servers(std::vector<Server> &new_servers)
{
	for (int i = 0; i < new_servers.size(); i++)
	{
		servers.push_back(new_servers[i]);
	}
}

void ServersManager::setup()
{
	for (int i = 0; i < servers.size(); i++)
	{
		servers[i].setup();
	}
}

void ServersManager::print()
{
	for (int i = 0; i < servers.size(); i++)
	{
		servers[i].print();
	}
}

vector<pollfd> &ServersManager::get_fds()
{
	manager_fds.clear();
	for (int i = 0; i < servers.size(); i++)
	{
		vector<pollfd> tmp;
		tmp = servers[i].get_fds();
		manager_fds.insert(manager_fds.end(), tmp.begin(), tmp.end());
	}
	return (manager_fds);
}

bool ServersManager::is_server(int fd)
{
	for (int i = 0; i < servers.size(); i++)
	{
		if (servers[i].socket_fd == fd)
			return true;
	}
	return false;
}

void ServersManager::accept_connections()
{
	for (int i = 0; i < servers.size(); i++)
	{
		servers[i].accept_connections();
	}
}

Client &ServersManager::get_client(int fd)
{
	for (int i = 0; i < servers.size(); i++)
	{
		Client *cur_client = servers[i].get_client_by_fd(fd);
		if (cur_client)
			return (*cur_client);
	}
	throw runtime_error(" the servers dont have that client");
}

void Server::setup()
{
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
	pollfd tmp;
	tmp.fd = this->socket_fd;
	tmp.events = POLLIN;
	tmp.revents = 0;
	
	this->_pfd.events = POLLIN;
	this->_pfd.revents = 0;
	this->_pfd.fd = this->socket_fd;
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
		cout << "new client with fd : " << ret << endl;
		Client client(*this, ret);
		this->clients.push_back(client);
	}
}

#define REQUEST_MAX_SIZE 300000


bool	Server::erase(int fd)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->_pfd.fd == fd)
		{
			close(fd);
			clients.erase(it);
			return (true);
		}
	}
	return (false);
}

void	ServersManager::remove_client(int fd)
{
	for (int i = 0; i < servers.size(); i++)
	{
		if (servers[i].erase(fd))
		{
			cout << " Client removerd from fd : " << i << endl;
			break;
		}
	}
}


void ServersManager::get_request(pollfd &pfd)
{
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

		return;
	}
	if (valread == 0)
	{
		std::cout << "Client disconnected" << std::endl;
		this->remove_client(pfd.fd);
	}

	// search for the client that with the evented fd

	Client &cur_client = get_client(pfd.fd);

	cout << "\n\n" << "read " << buffer << "\n\n" << endl;

	cur_client.save_request(buffer);
	cur_client.change_event();
}

void ServersManager::send_response(pollfd &pfd)
{
	Client &cur_client = get_client(pfd.fd);
	string response;

	if (cur_client._is_cgi)
	{
		if (cur_client._cgi.is_cgi_ready())
		{
			cur_client.cgi_exit_code = cur_client._cgi.cgi_get_code();
			if (cur_client.cgi_exit_code != 200)
			{
				cgi_response cgi_resp("", cur_client.cgi_exit_code);
				response = cgi_resp.get_cgi_response();
			}
			else
			{
				cgi_response cgi_resp(cur_client._cgi.cgi_get_response(), cur_client.cgi_exit_code);
				response = cgi_resp.get_cgi_response();
			}
		}
		else
			return;
	}
	else
	{
		response = cur_client.get_response();
	}

	// cout << RED <<  response  << RESET << endl;
	send(pfd.fd, (void *)response.c_str(), response.size(), 0);
	remove_client(pfd.fd);

	// need to close the connections 
}

// server core

void ServersManager::run()
{
	while (true)
	{
		std::vector<pollfd> &fds = this->get_fds();
		// for (int i = 0; i < fds.size() ; i++)
		// {
		// 	cout << "fd [" << i << "] :" << fds[i].fd << endl;
		// } 

		int ret = poll(fds.data(), fds.size(), -1);
		if (ret == -1)
		{
			perror("poll() failed");
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (is_server(fds[i].fd)) // TODO may remove this
				{
					for (int j = 0; j < servers.size(); j++)
					{
						if (servers[j].socket_fd == fds[i].fd)
							servers[j].accept_connections();
					}
				}
				else
				{
					get_request(fds[i]);
				}
			}
			else if (fds[i].revents & POLLOUT)
			{
				send_response(fds[i]);
			}
		}
	}
}
