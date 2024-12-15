#ifndef PARSE_HPP
#define PARSE_HPP

#include "webserv.hpp"
#include "config.hpp"
#include "server.hpp"

#define METHODES "GET POST DELETE"

class Parse
{
public:
	static std::vector<Server> get_servers(std::string file_name);
	static std::vector<Server> config2server(std::vector<Config> configs);
};

class wbs_ifstream : public ifstream
{
private:
	string file_name;

public:
	wbs_ifstream(string name) : std::ifstream(name), file_name(name) {}
	string name(void)
	{
		return (this->file_name);
	}
};

#endif /* PARSE_HPP */
