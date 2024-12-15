#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"

using namespace std;

struct loc_details;

typedef std::map<string, string>::iterator default_iter;
typedef std::map<string, std::map<string, string> >::iterator locations_iter;

class Config
{

public:
	std::map<string, string> defaults;
	std::map<string, std::map<string, string> > location;

	void	clear()
	{
		this->defaults.clear();
		this->location.clear();
	}
};

#endif /* CONFIG_HPP */
