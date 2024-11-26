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


// class	Pollfd
// {
// 	private :
// 		// the key is server idex pos & the data is the fd's under this server; 
// 		std::map<int, std::map<int, pollfd> >		pool;
// 		std::vector<pollfd> 						ret;
// 		size_t 										n_events;

// 	public	:
// 		Pollfd() : n_events(0) {}
	
// 		void	add(int server_index, int fd)
// 		{
// 			pollfd nu = {0};

// 			nu.fd = fd;
// 			nu.revents = POLLIN;
// 			nu.events = 0;

// 			this->n_events++;
// 			pool[server_index][fd] = nu;
// 		}

// 		// pollfd&	operator[] (int pos)
// 		// {
// 		// 	return (pool.at(pos).fd);
// 		// }

// 		void	remove(int fd)
// 		{
// 			close(fd);
// 			for (pool_iter it = pool.begin(); it != pool.end(); it++)
// 			{
// 				std::map<int, pollfd>::iterator found = it->second.find(fd);
// 				if ( found != it->second.end())
// 				{
// 					it->second.erase(found);
// 					this->n_events--;
// 					break;
// 				}
// 			}
//  		};

// 		size_t	size()
// 		{
// 			return (this->n_events);
// 		}

// 		// std::map<int, wbs_pollfd>::iterator begin()
// 		// {
// 		// 	return (pool.begin());
// 		// }

// 		// std::map<int, wbs_pollfd>::iterator end()
// 		// {
// 		// 	return (pool.end());
// 		// }


// 		bool	is_server(int fd)
// 		{
// 			for (pool_iter it = pool.begin(); it != pool.end(); it++)
// 			{
// 				if (fd == it->first)
// 					return true;
// 			}
// 			return  false;
// 		}


// 		pollfd	*data()
// 		{
// 			ret.clear();
// 			// ret.reserve(pool.size());
// 			for (pool_iter it = pool.begin(); it != pool.end(); it++)
// 			{
// 				for (std::map<int, pollfd>::iterator iter = it->second.begin(); iter != it->second.end(); iter++)
// 				{
// 					ret.push_back(iter->second);
// 				}
// 			}
// 			return (ret.data());
// 		}

// };


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

    bool is_server(int fd) const {
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
};
