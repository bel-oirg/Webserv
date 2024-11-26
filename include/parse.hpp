
#pragma once

#include <vector>
#include "server.hpp"
#include "server.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using std::string;
using std::cout;
using std::endl;
using std::cerr;


// typedef enum
// {
// 	SERVER = 1,
// 	OPEN_BLOCK,
// 	CLOSE_BLOCK,
// 	LOCATION
// }	TYPES;


enum token
{
	allow_methods_type = 1,
	autoindex_type,
	index_type,
	client_max_body_size_type,
	root_type,
	host_type,
	server_name_type,
	listen_type,
	return_type
};

#define METHODES "GET POST DELETE"

class Parse
{
    public :
		static std::vector<Server>	get_servers(std::string file_name);
		static std::vector<Server> config2server(std::vector<Config> configs);
};


class	wbs_ifstream : public ifstream 
{
	private :
		string file_name;

	public	:
			wbs_ifstream(string name) : 
			std::ifstream(name), file_name(name) {}
		string	name(void)
		{
			return (this->file_name);
		}
};