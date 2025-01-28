#include "webserv.hpp"
#include "locations.hpp"
#include "clients.hpp"
#include "server.hpp"
#include "utils.hpp"

std::map<string, loc_details> &Server::get_locations()
{
	return (this->locations);
}

Server::Server() : server_fds(),
				   locations(),
				   port(-1),
				   server_name(),
				   host(-1),
				   socket_fd(-1),
				   address(),
				   index(0),
				   _pfd(),
				   _timeout(10),
				   _is_up(false)
{
}

Server::Server(const Server &cpy)
{
	*this = cpy;
}

Server &Server::operator=(const Server &cpy)
{
	if (this != &cpy)
	{
		server_fds = cpy.server_fds;
		locations = cpy.locations;
		port = cpy.port;
		server_name = cpy.server_name;
		host = cpy.host;
		socket_fd = cpy.socket_fd;
		address = cpy.address;
		index = cpy.index;
		_pfd = cpy._pfd;
		_timeout = cpy._timeout;
	}
	return *this;
}

void Server::print() const
{
	cout << "Server Name: " << server_name << std::endl;
	cout << "Port: " << port << std::endl;
	cout << "Host: " << wbs::host2string(host) << std::endl;
	cout << "Timeout: " << _timeout << endl;
	cout << "Locations:" << std::endl;
	for (std::map<std::string, loc_details>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		cout << "  Location: " << it->first << std::endl;
		it->second.print();
	}
}

void ServersManager::init_servers(Server server)
{
	servers.push_back(server);
}

void ServersManager::init_servers(std::vector<Server> &new_servers)
{
	for (size_t i = 0; i < new_servers.size(); ++i)
	{
		servers.push_back(new_servers[i]);
	}
}

void ServersManager::setup()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		try
		{
			servers[i].setup();
			pollfd tmp = {.fd = servers[i].socket_fd, .events = POLLIN, .revents = 0};
			servers_pollfds.push_back(tmp);
			servers[i]._is_up = true;
		}
		catch (runtime_error &e)
		{
			cerr << "webserv: " << RED "error" RESET << " on server [" WHITE << i + 1 << RESET "] => " << e.what() << "." << endl;
			cerr << "Skipping...\n"
				 << endl;
			continue;
		}
	}
	cout << "Servers: " << endl;
	for (size_t i = 0; i < servers.size(); ++i) {
        std::string status = servers[i]._is_up ? GREEN "running" RESET : RED "failed" RESET;

        std::cout << "    http://" << servers[i]._server_info.remote_addr 
                  << ":" << servers[i]._server_info.server_port 
                  << std::setw(35 - (servers[i]._server_info.remote_addr.length() + servers[i]._server_info.server_port.length())) 
                  << status << std::endl;
    }
}

void ServersManager::print()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		servers[i].print();
	}
}

std::vector<pollfd> &ServersManager::get_fds()
{
	manager_fds.clear();
	manager_fds.reserve(client_pool.size() + servers_pollfds.size());
	for (std::map<int, Client *>::iterator it = client_pool.begin(); it != client_pool.end(); ++it)
	{
		manager_fds.push_back(it->second->get_fd());
	}
	manager_fds.insert(manager_fds.end(), servers_pollfds.begin(), servers_pollfds.end());
	return manager_fds;
}

bool ServersManager::is_server(int fd)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		if (servers_pollfds[i].fd == fd)
			return true;
	}
	return false;
}

void Server::setup()
{
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_fd == -1)
		throw runtime_error(string("socket() failed: ") + strerror(errno));

	int fd_flags = ::fcntl(this->socket_fd, F_GETFL, 0);
	if (fd_flags == -1)
		throw runtime_error(string("fcntl( F_GETFL ) failed: ") + strerror(errno));
	fd_flags = ::fcntl(this->socket_fd, F_SETFL, fd_flags | O_NONBLOCK);
	if (fd_flags == -1)
		throw runtime_error(string("fcntl( F_SETFL ) failed: ") + strerror(errno));

	this->address.sin_family = AF_INET;
	this->address.sin_port = htons(this->port);
	this->address.sin_addr.s_addr = this->host;

	int enable = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw runtime_error(string("setsockopt(SO_REUSEADDR) failed: ") + strerror(errno));
	enable = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable)) < 0)
              throw runtime_error(string("setsockopt(SO_NOSIGPIPE) failed: ") + strerror(errno));

	int bind_t = ::bind(this->socket_fd, (const struct sockaddr *)&(this->address), sizeof(this->address));
	if (bind_t == -1)
			throw runtime_error(string("bind() failed: ") + std::string(strerror(errno)));
	
	int listen_t = ::listen(this->socket_fd, SOMAXCONN);
	if (listen_t == -1)
		throw runtime_error(string("listen() failed: ") + std::string(strerror(errno)));


	sockaddr_in	socket_info;
	socklen_t 	socket_len = sizeof(socket_info);
	int socket_name_t = getsockname(this->socket_fd, (sockaddr *)(&socket_info), &socket_len);
	if (socket_name_t == -1)
		throw runtime_error(string("getsockname() failed: ") + std::string(strerror(errno)));

	this->_server_info.remote_addr = wbs::host2string(this->host);
	this->_server_info.server_name = this->server_name;
	this->_server_info.server_port = wbs::to_string(ntohs(socket_info.sin_port));

	this->_pfd.events = POLLIN;
	this->_pfd.revents = 0;
	this->_pfd.fd = this->socket_fd;
}

void Server::accept_connections(ServersManager &manager)
{
	socklen_t address_size = sizeof(this->address);
	int client_fd = accept(this->socket_fd, (sockaddr *)&(this->address), &address_size);
	if (client_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		perror("accept() failed");
		return;
	}

	Client *client = new Client(*this, client_fd);
	client->register_interaction();

	manager.client_pool[client_fd] = client;
}

void ServersManager::remove_client(int fd)
{
	std::map<int, Client *>::iterator it = client_pool.find(fd);
	if (it != client_pool.end())
	{
		delete it->second;
		client_pool.erase(it); 
	}
	close(fd);
}

void ServersManager::get_request(pollfd &pfd)
{
	char buffer[REQUEST_MAX_SIZE] = {0};

	int valread;
	valread = recv(pfd.fd, buffer, REQUEST_MAX_SIZE, SO_NOSIGPIPE);
	if (valread < 0)
	{
		this->remove_client(pfd.fd);
		return;
	}

	Client *cur_client = client_pool[pfd.fd];
	cur_client->register_interaction();

	cur_client->save_request(std::string(buffer, valread));
	if (cur_client->tmp_request.empty() && cur_client->_response->upload_eof)
	{
		cur_client->change_event();
	}
}

void ServersManager::send_response(pollfd &pfd)
{
	Client *cur_client = client_pool[pfd.fd];
	string response;

	cur_client->register_interaction();
	if (!cur_client->_headers_sended)
	{
		response = cur_client->_response->get_response_header();
		if (response.empty())
			return ;
		cur_client->_headers_sended = true;
	}
	else
	{
		response = cur_client->_response->get_to_send();
	}

	if (response == "\177")
		return ;

	int wr_ret = send(pfd.fd, (void *)response.c_str(), response.size(), SO_NOSIGPIPE);
	if (wr_ret < 0)
	{
		remove_client(pfd.fd);
		return;
	}

	if (cur_client->_response->_eof)
	{
		if (cur_client->_response->_is_closed)
		{
			remove_client(pfd.fd);
		}
		else // BUG keep-alive not working properly 
		{
			// cout << GREEN "is keep-alive"  RESET << endl; 
			// // cur_client->reset();
			// Client *new_client = new Client(cur_client->_server, pfd.fd);
			// client_pool[pfd.fd] = new_client;
			// delete cur_client;
		}
	}

}

// server core

void ServersManager::check_timeout(pollfd &fd)
{
	if (is_server(fd.fd))
		return;

	Client *client = client_pool[fd.fd];

	time_t last_inter = client->get_last_interaction();
	time_t time_now = time(NULL);

	double elapsed_time = difftime(time_now, last_inter);

	if (elapsed_time >= client->_server._timeout)
	{
		this->remove_client(fd.fd);
	}
}

void ServersManager::accept_connections(pollfd &fd)
{
	for (size_t j = 0; j < servers.size(); ++j)
	{
		if (servers[j].socket_fd == fd.fd)
		{
			servers[j].accept_connections(*this);
			break;
		}
	}
}

void ServersManager::run()
{
	while (true)
	{
		std::vector<pollfd> &fds = this->get_fds();
		int ret = poll(fds.data(), fds.size(), 10000); // TODO check the timeout and remove all of the clients
		if (ret == -1)
		{
			perror("poll() failed");
		}

		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLIN)
			{
				if (is_server(fds[i].fd))
				{
					accept_connections(fds[i]);
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
			else
			{
				check_timeout(fds[i]);
			}
		}
	}
}