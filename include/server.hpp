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



// struct	loc_details
// {
// 	int 				status_code;
// 	string 				index_path; //index_path
// 	std::vector<string> allowed_methods;
// 	bool				auto_index;
// 	bool				has_cgi;
// 	string 				root;
// 	string 				redir_to; // redir_to 
//     size_t				client_max_body_size;
// 	std::map<int, string>		error_pages;
// 	bool				has_slash;


// 	 void print() const {
//         cout << "    Status Code: " << status_code << endl;
//         cout << "    Index Path: " << index_path << endl;
//         // cout << "    Allow Methods: " << allowed_methods << endl;
//         cout << "    Autoindex: " << (auto_index ? "true" : "false") << endl;
//         cout << "    Has CGI: " << (has_cgi ? "true" : "false") << endl;
//         cout << "    Root: " << root << endl;
//         cout << "    Return Path: " << redir_to << endl;
//     }
// };

std::string hostToString(in_addr_t host);

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct loc_details
{
    map<int, string>		error_pages;
    vector<string>			allowed_methods;
	string					index_path; // index_path
    string					root;
    string					redir_to; // redir_to
	string					upload_path;
    bool					auto_index;
	bool					enable_upload;
    bool					has_cgi;
    bool					has_slash;
	int						status_code;
    size_t					client_max_body_size;


	loc_details() : status_code(0), auto_index(false), enable_upload(false),
					has_cgi(false), has_slash(false), client_max_body_size(0) {}



    void print() const {
		cout << "----------------------------------------------------------------------" << endl;
        cout << "Location Details:" << endl;
        cout << "    Status Code: " << status_code << endl;
        cout << "    Index Path: " << index_path << endl;

        cout << "    Allowed Methods: ";
        if (allowed_methods.empty()) {
            cout << "None" << endl;
        } else {
            for (size_t i = 0; i < allowed_methods.size(); ++i) {
                cout << allowed_methods[i];
                if (i != allowed_methods.size() - 1) cout << ", ";
            }
            cout << endl;
        }

        cout << "    Autoindex: " << (auto_index ? "true" : "false") << endl;
        cout << "    Has CGI: " << (has_cgi ? "true" : "false") << endl;
        cout << "    Root: " << root << endl;
        cout << "    Redirection Path: " << (redir_to.empty() ? "None" : redir_to) << endl;
        cout << "    Client Max Body Size: " << client_max_body_size << " bytes" << endl;

        cout << "    Error Pages:" << endl;
        if (error_pages.empty()) {
            cout << "        None" << endl;
        } else {
            for (const auto& [code, path] : error_pages) {
                cout << "        " << code << ": " << path << endl;
            }
        }

        cout << "    Has Slash: " << (has_slash ? "true" : "false") << endl;
		cout << "----------------------------------------------------------------------" << endl;
    }
};

class	Server
{
	public :
		// int 					server_index;	
		static	Pollfd			pool;
		static std::map<int , string>	responses;
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
        std::cout << "Host: " << hostToString(host) << std::endl; 

        // Print error pages

        // Print locations
        std::cout << "Locations:" << std::endl;
        for (std::map<std::string, loc_details>::const_iterator it = locations.begin(); it != locations.end(); ++it)
		{
            std::cout << "  Location: " << it->first << std::endl;
            it->second.print(); // Print details of the location
        }

    }
};

