
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


// const char http_response[] =
//     "HTTP/1.1 200 OK\n"
//     "Accept-Ranges: bytes\r\n"
//     "Age: 294510\r\n"
//     "Cache-Control: max-age=604800\r\n"
//     "Content-Type: text/html; charset=UTF-8\r\n"
//     "Date: Fri, 21 Jun 2024 14:18:33 GMT\r\n"
//     "Etag: \"3147526947\"\r\n"
//     "Expires: Fri, 28 Jun 2024 14:18:33 GMT\r\n"
//     "Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT\r\n"
//     "Server: ECAcc (nyd/D10E)\r\n"
// 	"connection: close\r\n"
//     "X-Cache: HIT\r\n"
//     "Content-Length: [body-lenght] \r\n"
//     "\r\n"
//     "<!doctype html>\n"
//     "<html lang=\"en\">\n"
//     "<head>\n"
//     "    <meta charset=\"UTF-8\">\n"
//     "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
//     "    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
//     "    <title>Server Test - It's Working</title>\n"
//     "    <style>\n"
//     "        body {\n"
//     "            font-family: Arial, sans-serif;\n"
//     "            text-align: center;\n"
//     "            margin-top: 50px;\n"
//     "        }\n"
//     "        .message {\n"
//     "            font-size: 2em;\n"
//     "            color: #4CAF50;\n"
//     "        }\n"
//     "    </style>\n"
//     "</head>\n"
//     "<body>\n"
//     "    <div class=\"message\"> Server is Runing!</div>\n"
//     "</body>\n"
//     "</html>";

typedef enum
{
	SERVER = 1,
	OPEN_BLOCK,
	CLOSE_BLOCK,
	LOCATION
}	TYPES;

struct	loc_details;

typedef std::map<string, string>::iterator						default_iter;
typedef std::map< string, std::map<string, string> >::iterator	locations_iter;


class	Config
{
    
	public :
        std::map<string, string>										defaults;
		std::map< string, std::map<string, string> >					location;
};