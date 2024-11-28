#pragma once

/**
 * @file pollfd.h
 * @brief 
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <vector>
#include <poll.h>


#include <map>
#include <poll.h>
#include <unistd.h>
#include "server.hpp"

struct pollfd;

using std::vector;
using std::deque;
using std::map;

class	Server;

class Pollfd 
{
private:
    std::map<int, std::vector<pollfd> > pool;
    size_t n_events;

public:
    Pollfd() : n_events(0) {}

	void change_event(int fd, int event)
	{
		for (map<int, vector<pollfd> >::iterator server = pool.begin(); server != pool.end(); server++)
		{
			vector<pollfd>::iterator fds;
			for (fds = server->second.begin(); fds != server->second.end() ; fds++)
			{
				if (fds->fd == fd)
				{
					fds->events = event;
					break;
				}
			}  
        }
	}

    void add_server(int server_fd)
	{
        pollfd nu;
        nu.fd = server_fd;
        nu.events = POLLIN;
        nu.revents = 0;

        pool[server_fd].push_back(nu);
        n_events++;    
	}

	void	add_client(int client_fd, int server_fd)
	{
		map<int , vector<pollfd> >::iterator it = pool.find(server_fd);
		if (it != pool.end())
		{
			pollfd nu;
			nu.fd = client_fd;
			nu.events = POLLIN;
			nu.revents = 0;

			pool[server_fd].push_back(nu);
			n_events++;
		}
	}


    void remove(int fd)
	{
        for (map<int, vector<pollfd> >::iterator server = pool.begin(); server != pool.end(); server++)
		{
			vector<pollfd>::iterator fds;
			for (fds = server->second.begin(); fds != server->second.end() ; fds++)
			{
				if (fds->fd == fd)
					break;
			}  
            if (fds != server->second.end())
			{
                close(fd);
                server->second.erase(fds);
                n_events--;
                return;
            }
        }
    }

    size_t size() const {
        return n_events;
    }

    bool is_server(int fd) const {
        return pool.find(fd) != pool.end();
    }

    std::vector<pollfd> data() const 
	{
        std::vector<pollfd> result;
        for (std::map<int, vector<pollfd> >::const_iterator server = pool.begin(); server != pool.end(); server++)
		{
			result.insert(result.end(), server->second.begin(), server->second.end());
        }
        return result;
    }

	Server &get_server(int fd, vector<Server> &servers);
};
