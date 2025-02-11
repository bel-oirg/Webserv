#include "webserv.hpp"
#include "locations.hpp"
#include "clients.hpp"
#include "server.hpp"
#include "utils.hpp"

std::map<string, loc_details> &Server::get_locations()
{
	return (this->locations);
}

Server::Server()
{
	this->port = -1;
    this->_host = -1;
    this->_is_up = false;
    this->socket_fd = -1;
    this->_pfd.fd = -1;
    this->_pfd.events = 0;
    this->_pfd.revents = 0;
}

void ServersManager::add_client_to_pool(Client *new_client)
{
	client_pool[new_client->get_pollfd().fd] = new_client;
}

void Server::print() const
{
	cout << "Port: " << port << std::endl;
	cout << "Host: " << wbs::host2string(_host) << std::endl;
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

ServersManager::ServersManager()
{
	this->poll_timeout = 0;
	this->reading_buffer = new char [RW_BUFFER];
}

ServersManager::~ServersManager()
{
	delete this->reading_buffer;
}

void ServersManager::init_servers(std::vector<Server> &new_servers)
{
	for (size_t i = 0; i < new_servers.size(); ++i)
	{
		servers.push_back(new_servers[i]);
	}
}

server_info &Server::get_info()
{
	return (this->_server_info);
}

int Server::socket()
{
	return socket_fd;
}
void Server::set_host(in_addr_t host)
{
	this->_host = host;
}
in_addr_t Server::get_host()
{
	return _host;
}

void ServersManager::setup()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		try
		{
			servers[i].setup();
			pollfd tmp = {.fd = servers[i].socket(), .events = POLLIN, .revents = 0};
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

		string host = wbs::host2string(servers[i].get_host());
      	std::cout << "    http://" << host
          << ":" << (servers[i]._server_info.server_port.empty() 
                     ? wbs::to_string(servers[i].port) 
                     : servers[i]._server_info.server_port)
          << std::setw(35 - (host.length() + servers[i]._server_info.server_port.length())) 
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

std::vector<pollfd> &ServersManager::get_targets()
{
	manager_fds.clear();
	manager_fds.reserve(client_pool.size() + servers_pollfds.size());
	this->poll_timeout = (int)(client_pool.size() == 0) ? -1 : max((unsigned long)((2000 - (client_pool.size() * 50))), 100UL);
	for (std::map<int, Client *>::iterator it = client_pool.begin(); it != client_pool.end(); ++it)
	{
		if (it->second)
			manager_fds.push_back(it->second->get_pollfd());
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
	this->socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
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
	this->address.sin_addr.s_addr = this->get_host();

	int enable = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw runtime_error(string("setsockopt(SO_REUSEADDR) failed: ") + strerror(errno));
	enable = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
		throw runtime_error(string("setsockopt(SO_REUSEPORT) failed: ") + strerror(errno));
	enable = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable)) < 0)
		throw runtime_error(string("setsockopt(SO_NOSIGPIPE) failed: ") + strerror(errno));

	int bind_t = ::bind(this->socket_fd, (const struct sockaddr *)&(this->address), sizeof(this->address));
	if (bind_t == -1)
		throw runtime_error(string("bind() failed: ") + std::string(strerror(errno)));

	int listen_t = ::listen(this->socket_fd, SOMAXCONN);
	if (listen_t == -1)
		throw runtime_error(string("listen() failed: ") + std::string(strerror(errno)));

	sockaddr_in socket_info;
	socklen_t socket_len = sizeof(socket_info);
	int socket_name_t = getsockname(this->socket_fd, (sockaddr *)(&socket_info), &socket_len);
	if (socket_name_t == -1)
		throw runtime_error(string("getsockname() failed: ") + std::string(strerror(errno)));

	this->_server_info.remote_addr = wbs::host2string(this->get_host());
	// this->_server_info.server_names = this->server_names;
	this->_server_info.server_port = wbs::to_string(ntohs(socket_info.sin_port));

	this->_pfd.events = POLLIN;
	this->_pfd.revents = 0;
	this->_pfd.fd = this->socket_fd;
}

// void Server::accept_connections(ServersManager &manager)
// {
	
// }

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

	Client *cur_client = client_pool[pfd.fd];
	if (!cur_client)
	{
		this->remove_client(pfd.fd);
		return;
	}

	cur_client->register_interaction();
	if (cur_client->handshake)
	{
		cur_client->handshake = false;
		cur_client->get_pollfd().events = POLLIN;
		return ;
	}

	bzero(this->reading_buffer, RW_BUFFER);
	int valread;
	valread = recv(pfd.fd, this->reading_buffer, RW_BUFFER, 0);
	if (valread <= 0)
	{
		this->remove_client(pfd.fd);
		return;
	}

	cur_client->save_request(std::string(this->reading_buffer, valread));

	if (cur_client->request_buffer().empty() && (cur_client->_response && cur_client->_response->upload_eof))
	{
		cur_client->listen_to_write();
	}
}

void ServersManager::send_response(pollfd &pfd)
{
	Client *cur_client = client_pool[pfd.fd];
	if (!cur_client)
	{
		this->remove_client(pfd.fd);
		return;
	}

	if (!cur_client->_response)
		return;

	cur_client->register_interaction();
	if (cur_client->handshake)
	{
		cur_client->handshake = false;
		cur_client->get_pollfd().events = POLLIN;
		return ;
	}

	string response;
	if (!cur_client->_headers_sended)
	{
		response = cur_client->_response->get_response_header();
		if (response.empty())
			return;
		cur_client->_headers_sended = true;
	}
	else
	{
		response = cur_client->_response->get_to_send();
	}

	if (response == "\177")
		return;
	// pp GREEN << response << RESET << endl;  ///DEBUG

	int wr_ret = send(pfd.fd, (void *)response.c_str(), response.size(), 0);
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
		else
		{
			Client *new_client = new Client(pfd.fd, &servers, cur_client->entry_port);
			client_pool[pfd.fd] = new_client;
			delete cur_client;
		}
	}
}


void ServersManager::check_timeout(pollfd &fd)
{
	if (is_server(fd.fd))
		return;

	Client *client = client_pool[fd.fd];
	if (!client)
		return;

	time_t last_inter = client->get_last_interaction();
	time_t time_now = time(NULL);

	time_t elapsed_time = time_now - last_inter;

	time_t timeout = 5 + (this->client_pool.size() / 20);
	if (elapsed_time >= timeout )
	{
		this->remove_client(fd.fd);
	}
}

void ServersManager::accept_connections(pollfd &fd)
{
	socklen_t address_size;
	sockaddr_in addr;
	int client_fd = accept(fd.fd, (sockaddr *)&(addr), &address_size);
	if (client_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		perror("accept() failed");
		return;
	}

	sockaddr_in server_info;
    socklen_t len = sizeof(server_info);
    getsockname(client_fd, (struct sockaddr*)&server_info, &len);
	Client *client = new Client(client_fd, &servers, ntohs(server_info.sin_port));
	client->register_interaction();

	add_client_to_pool(client);
}

void ServersManager::reset_servers()
{
	for (map<int, Client *>::iterator it = client_pool.begin(); it != client_pool.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	client_pool.clear();
	manager_fds.clear();
}

void ServersManager::run()
{
	while (true)
	{
		try {

			std::vector<pollfd> &fds = this->get_targets();
			int ret = poll(fds.data(), fds.size(), this->poll_timeout);
			if (ret == -1)
			{
				perror("poll() failed");
				throw runtime_error("poll future");
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
		catch (...)
		{
			this->reset_servers();
		}
	}
}