
#pragma once

/**

Global Parameters
client_max_body_size

Purpose: Limits the size of the request body (e.g., for file uploads).
Default: None (unlimited).
error_page

Purpose: Path to custom error pages for HTTP errors like 404 or 500.
Default: None (use generic HTTP responses).
worker_connections (Implied by non-blocking requirements)

Purpose: Sets the maximum number of simultaneous connections the server can handle.
Default: System-defined or no limit.
keepalive_timeout (Optional for HTTP 1.1)

Purpose: Sets the duration for keeping idle connections alive.
Default: None (close connections after each request).
server_tokens (Optional)

Purpose: Whether to display server information in responses (for security).
Default: On (displays version info).

http {
    listen 8080;                  # Default port to listen on
    server_name localhost;        # Server's hostname
    root ./;                      # Root directory for serving files
    index index.html;             # Default file for directory requests
    error_page 404 /error_pages/404.html; # Default error page
    client_max_body_size 1m;      # Maximum client body size (default example)

    server {
        location / {
            allowed_methodes GET;          # Allowed methods for root
            autoindex off;              # Directory listing disabled
        }

        location /tours {
            allowed_methodes GET POST PUT HEAD;   # Allowed methods
            autoindex on;                      # Directory listing enabled
            index tours1.html;                 # Specific index file
        }

        location /red {
            return 301 /tours;         # Redirect to /tours
        }

        location /cgi-bin {
            root ./;                          # Root directory for CGI
            allowed_methodes GET POST DELETE;   # Methods allowed for CGI
            index time.py;                   # Default CGI script
            cgi_path /usr/bin/python3 /bin/bash;  # Paths to interpreters
            cgi_extensions .py .sh;          # Extensions for CGI
        }
    }
}


*/

#pragma once


#include "server.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include "clients.hpp"

using std::string;
using std::cout;
using std::endl;
using std::cerr;

enum GlobalDir
{
	MAX_BODY = 1,	// client_max_body_size
	ERR_PAGE,		// error_page
	WORK_CONN,		// worker_s
	KEEP_ALV,		// keepalive_timeout
	// SRV_TOKN		// server_tokens (optional )
};


const char http_response[] =
    "HTTP/1.1 200 OK\n"
    "Accept-Ranges: bytes\r\n"
    "Age: 294510\r\n"
    "Cache-Control: max-age=604800\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
    "Etag: \"3147526947\"\r\n"
    "Expires: Fri, 28 Jun 2024 14:18:33 GMT\r\n"
    "Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT\r\n"
    "Server: ECAcc (nyd/D10E)\r\n"
	"connection: close\r\n"
    "X-Cache: HIT\r\n"
    "Content-Length: [body-lenght] \r\n"
    "\r\n"
    "<!doctype html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
    "    <title>Server Test - It's Working</title>\n"
    "    <style>\n"
    "        body {\n"
    "            font-family: Arial, sans-serif;\n"
    "            text-align: center;\n"
    "            margin-top: 50px;\n"
    "        }\n"
    "        .message {\n"
    "            font-size: 2em;\n"
    "            color: #4CAF50;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div class=\"message\"> Server is Runing!</div>\n"
    "</body>\n"
    "</html>";

typedef enum
{
	SERVER = 1,
	OPEN_BLOCK,
	CLOSE_BLOCK,
	LOCATION
}	TYPES;

struct	loc_details;

typedef std::map<string, string>::iterator default_iter;
typedef std::map< string, std::map<string, string> >::iterator locations_iter;


class	Config
{
    
	public :
        std::map<string, string>										defaults;
		std::map< string, std::map<string, string> >					location;
		// struct sockaddr_in 				address;
        // int                             socket_fd;
		// socklen_t						socket_len;

    // public :
		// Config() { address = {0}; socket_len = sizeof(address);};
        // int get_port() {
        //         return atoi(this->defaults["listen"].c_str()); };
        // int get_host() {
        //         return inet_addr(this->defaults["host"].c_str());
        // }

        // void set_socket(int fd) {
		// 	if (fd < 0)
		// 		perror ("set_socket faild : Invalid fd");
        //     this->socket_fd = fd ;
        // }
        // int get_socket() {
        //     // TODO add protections of invalid fd;
        //     return (this->socket_fd);
        // }

		// socklen_t get_socklen() { 
		// 	return (this->socket_len);
		// }

		// socklen_t *get_socklenp() { 
		// 	return (&socket_len);
		// }

		// const struct sockaddr* get_address()
		// {
		// 	return ((const struct sockaddr *)&this->address);
		// }
		// struct sockaddr* get_address(int)
		// {
		// 	return ((struct sockaddr *)&this->address);
		// }

        // void	accept_connections();
		// void	send_response(int index);
};