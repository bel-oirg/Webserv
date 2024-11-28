
#include "server.hpp"
#include "pollfd.hpp"

struct find_by_fd {
    int fd;
    find_by_fd(int fd) : fd(fd) {}
    bool operator()(const pollfd& p) const {
        return p.fd == fd;
    }
};

Server &Pollfd::get_server(int fd, vector<Server> &servers)
{
	for (int i = 0; i < servers.size(); i++)
	{
		if (std::find_if(pool[servers[i].socket_fd].begin(), 
			pool[servers[i].socket_fd].end(), find_by_fd(fd)) != pool[servers[i].socket_fd].end())
			{
				return (servers[i]);
			}
	}
	throw runtime_error("Cant find the server");
}