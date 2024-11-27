#pragma once

/**
 * @file pollfd.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "server.hpp"
#include <vector>
#include <poll.h>


/**
 * @brief 
 * custum struct so i can add for every pollfd,
 * 			the server that should serve it
 */

struct pollfd;

struct	wbs_pollfd
{
	pollfd	fd;
	int		server;
};

typedef	std::map<int, std::map<int, pollfd> >::iterator pool_iter;


#include <map>
#include <vector>
#include <poll.h>
#include <unistd.h>

class Pollfd {

	private:
		std::map<int, std::map<int, pollfd> > pool; // server_index -> fd -> pollfd
		size_t n_events;

	public:
		Pollfd() : n_events(0) {}

		void add(int server_index, int fd) {
			pollfd nu;
			nu.fd = fd;
			nu.events = POLLIN;
			nu.revents = 0;

			if (pool[server_index].find(fd) == pool[server_index].end()) {
				pool[server_index][fd] = nu;
				n_events++;
			}
		}

		void remove(int fd) {
			for (std::map<int, std::map<int, pollfd> >::iterator server = pool.begin(); server != pool.end(); ++server) {
				std::map<int, pollfd>::iterator found = server->second.find(fd);
				if (found != server->second.end()) {
					close(fd);
					server->second.erase(found);
					n_events--;
					return;
				}
			}
		}

		size_t size() const {
			return n_events;
		}

		bool is_server(int fd) const 
		{
			return pool.find(fd) != pool.end();
		}

		std::vector<pollfd> data() const 
		{
			std::vector<pollfd> result;
			for (std::map<int, std::map<int, pollfd> >::const_iterator server = pool.begin(); server != pool.end(); ++server)
			{
				for (std::map<int, pollfd>::const_iterator fd_entry = server->second.begin(); fd_entry != server->second.end(); ++fd_entry) {
					result.push_back(fd_entry->second);
				}
			}
			return result;
		}
	 int get_server_index(int client_fd)
	 {
        for (pool_iter server = pool.begin(); server != pool.end(); ++server)
		{
            if (server->second.find(client_fd) != server->second.end())
			{
                return server->first;
            }
        }
        return -1;
    }
};
