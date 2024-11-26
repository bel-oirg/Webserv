#pragma once

#include <string>
#include <limits>
#include <stdint.h>
#include <map>
#include <iostream>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <assert.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <fstream>
#include "config.hpp"
#include <cstdint>
#include "pollfd.hpp"
// #include "../request/Response.hpp"

// class response;

using std::string;
using std::cout;
using std::endl;
using std::cerr;


//		int status_code;
//     bool auto_index;
//     bool has_cgi;
//     std::string root;
//     std::vector<std::string> index_path;
//     std::vector<std::string> allowed_methods;

// 		uint16_t						_port;
// 		in_addr_t						_host;
// 		std::string						_server_name;
// 		std::string						_root;
// 		unsigned long					_client_max_body_size;
// 		std::string						_index;
// 		bool							_autoindex;
// 		std::map<short, std::string>	_error_pages;
// 		std::vector<Location> 			_locations;
//         struct sockaddr_in 				_server_address;
//         int     						_listen_fd;


typedef	uint16_t	PORT;
typedef	in_addr_t	ip_addr;



struct	loc_details
{
	int 				status_code;
	string 				index_path; //index_path
	std::vector<string> allowed_methods;
	bool				auto_index;
	bool				has_cgi;
	string 				root;
	string 				redir_to; // redir_to 
    size_t				client_max_body_size;
	std::map<int, string>			error_pages;
	bool				has_slash;


	 void print() const {
        cout << "    Status Code: " << status_code << endl;
        cout << "    Index Path: " << index_path << endl;
        // cout << "    Allow Methods: " << allowed_methods << endl;
        cout << "    Autoindex: " << (auto_index ? "true" : "false") << endl;
        cout << "    Has CGI: " << (has_cgi ? "true" : "false") << endl;
        cout << "    Root: " << root << endl;
        cout << "    Return Path: " << redir_to << endl;
    }
};

class	Server
{
	public :
		// int 					server_index;	
		static	Pollfd			pool;
		std::map<string, loc_details>	locations;

		uint16_t	port;
		string		server_name;
		in_addr_t	host;
		int 		socket_fd;
		struct sockaddr_in	address;
		int				index;

	public :
		Server():
		port(0), server_name(""), host(0) { }

		// void	run(std::vector<Server> &servers)
		void	setup();
		static void	run(std::vector<Server> &servers);
		void	accept_connections();

		Server& operator= (const Server &cpy)
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

		Server(const Server &cpy)
		{
			*this = cpy; 
		}

		 void print() const {
        std::cout << "Server Name: " << server_name << std::endl;
        std::cout << "Port: " << port << std::endl;
        std::cout << "Host: " << host << std::endl;
        std::cout << "Index: " << index << std::endl;

        // Print error pages
        std::cout << "Error Pages:" << std::endl;

        // Print locations
        std::cout << "Locations:" << std::endl;
        for (std::map<std::string, loc_details>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
            std::cout << "  Location: " << it->first << std::endl;
            it->second.print(); // Print details of the location
        }

        std::cout << "-----------------------------" << std::endl;
    }
};

