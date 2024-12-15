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

Server::Server() : port(0), server_name(""), host(0), _timeout(10)
{}

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
	cout << "Server Name: " << server_name << std::endl;
	cout << "Port: " << port << std::endl;
	cout << "Host: " << hostToString(host) << std::endl;
	cout << "Locations:" << std::endl;
	// for (std::map<std::string, loc_details>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	// {
	// 	cout << "  Location: " << it->first << std::endl;
	// 	it->second.print();
	// }
}

void ServersManager::init_servers(Server server)
{
	servers.push_back(server);
}

void ServersManager::init_servers(std::vector<Server> &new_servers)
{
	for (size_t i = 0; i < new_servers.size(); i++)
	{
		servers.push_back(new_servers[i]);
	}
}

void ServersManager::setup()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		servers[i].setup();
		pollfd tmp = {.fd = servers[i].socket_fd, .events = POLLIN, .revents = 0};
		servers_pollfds.push_back(tmp);
	}
}

void ServersManager::print()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		servers[i].print();
	}
}

std::vector<pollfd> &ServersManager::get_fds()
{
	manager_fds.clear();
	for (auto it = client_pool.begin(); it != client_pool.end(); it++)
	{
		manager_fds.push_back(it->second->get_fd());
	}
	manager_fds.insert(manager_fds.end(), servers_pollfds.begin(), servers_pollfds.end());
	return manager_fds;
}

bool ServersManager::is_server(int fd)
{
	for (size_t i = 0; i < servers.size(); i++)
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
	{
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	int fd_flags = fcntl(this->socket_fd, F_GETFL, 0);
	if (fd_flags == -1)
		perror("fcntl( F_GETFL ) failed");
	 fd_flags = fcntl(this->socket_fd, F_SETFL, fd_flags | O_NONBLOCK);
	if (fd_flags == -1)
		perror("fcntl( F_SETFL ) failed"); // TODO maybe skip creating the server

	this->address.sin_family = AF_INET;
	this->address.sin_port = htons(this->port);
	this->address.sin_addr.s_addr = this->host;

	const int enable = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	std::cerr << "setsockopt(SO_REUSEADDR) failed" << endl;

	int bind_t = ::bind(this->socket_fd, (const struct sockaddr *)&(this->address), sizeof(this->address));
	if (bind_t == -1)
	{
		perror("dind() failed");
		exit(EXIT_FAILURE);
	}

	int listen_t = listen(this->socket_fd, SOMAXCONN);
	if (listen_t == -1)
	{
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}

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
			return ;// break; // No more clients 
		perror("accept() failed");
			return;
	}

	cout << "New client accepted with fd: " << client_fd << std::endl;
	Client *client = new Client(*this, client_fd);
	client->register_interaction();

	manager.client_pool[client_fd] = client;
}


void ServersManager::remove_client(int fd)
{
	std::map<int, Client * >::iterator it = client_pool.find(fd);
	if (it != client_pool.end())
	{
		delete it->second;
		client_pool.erase(it);
	}
	close(fd);
}

#define REQUEST_MAX_SIZE 20000

void ServersManager::get_request(pollfd &pfd)
{
	char buffer[REQUEST_MAX_SIZE] = {0};

	int valread;
	valread = recv(pfd.fd, buffer, REQUEST_MAX_SIZE - 1, 0);
	if (valread == -1)
	{
		perror("read() failed");
		exit(1);
	}
	else if (valread >= REQUEST_MAX_SIZE)
	{
		cout << "request lenght too large : action => discarding" << std::endl;
		return;
	}
	if (valread == 0)
	{
		cout << "Client disconnected" << std::endl;
		this->remove_client(pfd.fd);
		return;
	}

	// buffer[valread] = '\0';
	Client *cur_client = client_pool[pfd.fd];
	cur_client->register_interaction();

	try
    {
		cout << MAGENTA << "SIZE before :"<< valread << RESET << endl;
        cur_client->save_request(std::string(buffer, valread));
		if (cur_client->_response->upload_eof)
		{
			cur_client->change_event();
		}
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in save_request: " << e.what() << std::endl;
		this->remove_client(pfd.fd); // Clean up if save_request fails
    }
}

void ServersManager::send_response(pollfd &pfd)
{
	Client *cur_client = client_pool[pfd.fd];
	string response;

	// if (cur_client->_is_cgi)
	// {
	// 	if (cur_client->_cgi.is_cgi_ready())
	// 	{
	// 		cur_client->cgi_exit_code = cur_client->_cgi.cgi_get_code();
	// 		if (cur_client->cgi_exit_code != 200)
	// 		{
	// 			cgi_response cgi_resp("", cur_client->cgi_exit_code);
	// 			response = cgi_resp.get_cgi_response();
	// 		}
	// 		else
	// 		{
	// 			cgi_response cgi_resp(cur_client->_cgi.cgi_get_response(), cur_client->cgi_exit_code);
	// 			response = cgi_resp.get_cgi_response();
	// 		}
	// 	}
	// 	else
	// 		return;
	// }
	// else
	// {
	if (!cur_client->_headers_sended)
	{
		response = cur_client->_response->get_response_header();
		cur_client->_headers_sended = true;
	}
	else
	{
		response = cur_client->_response->get_to_send();
	}

	cur_client->register_interaction();

	int wr_ret = send(pfd.fd, (void *)response.c_str(), response.size(), 0);
	cout << BLUE << response << RESET << endl;
	if (wr_ret < 0)
	{
		perror ("send() ");
	}
	if (cur_client->_response->_eof || cur_client->_is_cgi)
	{
		cout << "eof: " << std::boolalpha << cur_client->_response->_eof  << std::endl;
		cout << "is cgi: " << std::boolalpha << cur_client->_is_cgi << std::endl;
		remove_client(pfd.fd);

	}

}

// server core

void	ServersManager::check_timeout(pollfd& fd)
{
	if (is_server(fd.fd))
		return;

	Client	*client = client_pool[fd.fd];

	clock_t	last_inter = client->get_last_interaction();
	clock_t	time_now = clock();

	double elapsed_time = double(time_now - last_inter) / CLOCKS_PER_SEC;

    if (elapsed_time >= client->_server._timeout)
    {
		// TODO log for timeout 
        this->remove_client(fd.fd);
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
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (is_server(fds[i].fd)) // TODO may remove this
				{
					for (size_t j = 0; j < servers.size(); j++)
					{
						if (servers[j].socket_fd == fds[i].fd)
						{
							servers[j].accept_connections(*this);
							break;
						}
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
			// else
			// {
			// 	check_timeout(fds[i]);
			// }
		}
	}
}
