#ifndef PARSE_HPP
#define PARSE_HPP

#include "webserv.hpp"
#include "config.hpp"
#include "server.hpp"


#define HELP "\
webserv Help\n\
=============\n\
\n\
Usage:\n\
------\n\
- webserv <path_to_config_file>\n\
- webserv -h -- Display this help page and exit\n\
\n\
Config File:\n\
--------------------------\n\
- The configuration file defines the server and location directives.\n\
- Each directive must end with a semicolon (;).\n\
- Server blocks define global settings for the server.\n\
- Location blocks define settings for specific URL paths within the server.\n\
- The `location` directive must always be within a `server` block.\n\
\n\
Directives for Server:\n\
-----------------------\n\
- listen: listen <portnum>;\n\
- server_name: server_name <name>;\n\
- host: host <IPv4_address>;\n\
- error_page: error_page <error_code> <file_path>;\n\
- client_max_body_size: client_max_body_size <size_in_bytes>;\n\
\n\
Directives for Location:\n\
-------------------------\n\
- root: root <path>;\n\
- index: index <file>;\n\
- allowed_methods: allowed_methods <method_list>;\n\
- autoindex: autoindex <on|off>;\n\
- cgi_executor: cgi_executor <extension> <path>;\n\
- upload_path: upload_path <directory>;\n\
- return: return <url>;\n\
- cgi_ext: cgi_ext <extension_list>;\n\
- upload_enable: upload_enable <on|off>;\n\
- client_max_body_size: client_max_body_size <size_in_bytes>;\n\
- cgi_pass: cgi_pass <on|off>;"




class Parse
{
	public	:
		static std::vector<Server> get_servers(std::string file_name);
		static void display_help();
		static std::vector<Server> config2server(std::vector<Config> configs);

	private :
		static bool is_server(const std::string &line);
		// std::vector<Server> get_servers(string filename);
		static bool is_location(const std::string &line, string &path);
		static string	non_empty(string &key, string &value);
		static bool	bool_type_parse(string &key, string &value);
		static vector<string>	allowed_methods(string &value);
		static void defaults(std::map<string, string>::iterator iter, Server &server, loc_details &loc);
		static void locations(map<string, string>::iterator loc, loc_details &dest);
		static void	check_consistency(std::vector<Server> &servers);
};



class wbs_ifstream : public ifstream
{
	private:
		string file_name;

	public:
		wbs_ifstream(string name) : std::ifstream(name.c_str()), file_name(name) {}
		string name(void)
		{
			return (this->file_name);
		}
};

#endif /* PARSE_HPP */
