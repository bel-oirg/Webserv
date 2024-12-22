#ifndef PARSE_HPP
#define PARSE_HPP

#include "webserv.hpp"
#include "config.hpp"
#include "server.hpp"


class Parse
{
	public	:
		static std::vector<Server> get_servers(std::string file_name);
		static std::vector<Server> config2server(std::vector<Config> configs);

	private :
		static bool is_server(const std::string &line);
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
