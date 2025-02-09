#include "webserv.hpp"
#include "locations.hpp"
#include "server.hpp"
#include "parse.hpp"
#include "clients.hpp"
#include <algorithm>
#include "utils.hpp"

using namespace std;

void	Parse::display_help()
{
	cout << HELP << endl;
}

bool	skip_ws(istringstream &stream)
{
	char c = stream.peek();
	if (::isspace(c))
	{
		stream >> ws;
		return (true);
	}
	return (false);
}

bool Parse::is_server(const std::string &line)
{
	std::string trimmed = wbs::get_trimed(line);

	if (trimmed.find("server") != 0)
		return false;

	std::string rem = trimmed.substr(6);
	rem = wbs::get_trimed(rem);

	if (rem.empty() || (rem == "{"))
		return true;

	return false;
}


bool Parse::is_location(const std::string &line, string &path)
{
	size_t pos = line.find("location");
	if (pos != std::string::npos)
	{
		size_t openBracket = line.find("{", pos);
		if (openBracket == string::npos)
			throw std::runtime_error("Config Error: Invalid syntax in 'location' directive. Expected opening bracket or EOF declaration.");
		if (openBracket != line.size() - 1)
			throw std::runtime_error("Config Error: Invalid syntax in 'location' directive. Expected new line after bracket.");
		path = line.substr(pos + 8, openBracket - (pos + 8));
		path = wbs::get_trimed(path);
		if (path.empty())
			throw std::runtime_error("Config Error: Syntax error in 'location' directive. Expected value after location declaration.");
		path.erase(0, path.find_first_not_of(" \t"));
		path.erase(path.find_last_not_of(" \t") + 1);
		return (!path.empty() && openBracket == line.length() - 1);
	}
	return false;
}

string	Parse::non_empty(string &key, string &value)
{
	if (value.empty())
		throw runtime_error("Config Error: The '" + key + "' directive cannot be empty. Provide a valid file name.");
	else
		return value;
}

bool	Parse::bool_type_parse(string &key, string &value)
{
	bool	ret_val;

	if (value == "on")
			ret_val = true;
	else if (value == "off")
			ret_val = false;
	else
		throw runtime_error("Config Error: Invalid value for '" + key + "': '" + value + "'. Valid options are: 'on' or 'off'.");
	return (ret_val);
}

std::vector<string>	Parse::allowed_methods(string &value)
{
	vector<string> methodes = wbs::split(value, " /t");

	for (size_t i = 0; i < methodes.size(); ++i)
	{
		if ((	methodes[i] != "GET"      && 
				  methodes[i] != "POST"     && 
		      		methodes[i] != "DELETE")  && 
			(methodes.size() > 1 || methodes[i] != "NONE"))
		throw runtime_error("Config Error: Invalid method: '" + methodes[i] + "'. Allowed methods are: GET, POST, DELETE or NONE.");
	}
	return (methodes);
}

void Parse::defaults(std::map<string, string>::iterator iter, Server &server, loc_details &loc)
{
	string key = wbs::get_trimed(iter->first);
	string value = wbs::get_trimed(iter->second);
	if (value[0] == ',')
		value.erase(0, 1);

	if (key == "listen")
	{
		int port;
		if (!wbs::all_of(value.begin(), value.end(), ::isdigit))
			throw runtime_error("Config Error: Invalid port value: '" + value + "'. Port must be a numeric value.");
		else
		{
			istringstream cnv(value);
			if (!(cnv >> port) || port > 65535 || port < 0)
				throw runtime_error("Config Error: Port value '" + value + "' is out of range. Valid range is 0 to 65535.");
		}
		server.port = port;
	}
	else if (key == "server_names")
	{
		server.server_names = wbs::split(value, ",");
		for_each(server.server_names.begin(), server.server_names.end(), wbs::trim_line);
	}
	else if (key == "host")
	{
		if (value == "localhost")
			value = "127.0.0.1";
		in_addr_t host;
		host = inet_addr(value.c_str());
		if (host == static_cast<in_addr_t> (-1))
			throw runtime_error("Config Error: Invalid host address: '" + value + "'. Provide a valid IPv4 address.");
		server.set_host(host);
	}
	else if (key == "error_page")
	{
		int code;
		string page;
		std::vector<string> pages = wbs::split(value, ",");
		for (size_t i = 0; i < pages.size(); i++)
		{
			std::istringstream stream(pages[i]);
			if (!(stream >> code && skip_ws(stream) && std::getline(stream >> std::ws, page)))
				throw runtime_error("Config Error: Syntax error in 'error_page' directive. Expected format: '<error_code> <page_path>'.");
			loc.error_pages.insert(make_pair(code, page));
		}
	}
	else if (key == "client_max_body_size")
	{
		if (wbs::all_of(value.begin(), value.end(), ::isdigit))
		{
			uint64_t cmbs_value;
			cmbs_value = atoll(value.c_str());
			loc.client_max_body_size = cmbs_value;
		}
		else
			throw runtime_error("Config Error: Invalid value for 'client_max_body_size': '" + value + "'. Expected a numeric value.");
	}
	else if (key == "root")
		loc.root = value;
	else if (key == "allowed_methods")
		loc.allowed_methods = allowed_methods(value);
	else if (key == "autoindex")
		loc.auto_index = bool_type_parse(key, value);
	else if (key == "cgi_executor")
	{
		std::vector<string> splited = wbs::split(value, ",");
		for (size_t i = 0; i < splited.size(); ++i)
		{
			 string extention, path;
 			istringstream stream(splited[i]);
 			stream >> skipws;
 			if (!(stream >> extention >> path && stream.eof()))
 				throw runtime_error("Config Error: Invalid value for 'cgi_executor': '" + extention + ":" + path + "'. Expected format: '<extention> <excuter>'.");
 			loc.cgi_excutor[extention] = path;
		}
	}
	else if (key == "upload_path")
		loc.upload_path = non_empty(key, value);
	else
		throw runtime_error("Config Error: Unknown argument: '" + key + "'. Check 'webserv -h' for valid options.");
}



void Parse::locations(map<string, string>::iterator loc, loc_details &dest)
{
	string key = wbs::get_trimed(loc->first);
	string value = wbs::get_trimed(loc->second);

	if (value[0] == ',')
		value.erase(0, 1);
	if (key == "allowed_methods")
		dest.allowed_methods = allowed_methods(value);
	else if (key == "autoindex")
		dest.auto_index = bool_type_parse(key, value);
	else if (key == "root")
		dest.root = non_empty(key, value);
	else if (key == "index")
	{
		vector<string> splited  = wbs::split(value, ",");
		for_each(splited.begin(), splited.end(), wbs::trim_line);
		dest.index_path = splited;
	}
	else if (key == "return")
	{
		std::istringstream stream(value);
		if (!(stream >> dest.status_code && skip_ws(stream) && std::getline(stream >> std::ws, dest.redir_to)))
			throw runtime_error("Config Error: Syntax error in 'return' directive. Expected format: '<redir_code> <url>'.");
		if (!(dest.status_code == 301 || dest.status_code == 302))
			throw runtime_error("Config Error: Syntax error in 'return' directive: Support only following codes [301, 302].");
	}
	else if (key == "cgi_pass")
		dest.has_cgi = bool_type_parse(key, value);
	else if (key == "upload_enable")
		dest.enable_upload = bool_type_parse(key, value);
	else if (key == "cgi_ext")
	{
		vector<string> splited  = wbs::split(value, ",");
		for_each(splited.begin(), splited.end(), wbs::trim_line);
		dest.cgi_extentions = splited;
	}
	else
		throw runtime_error("Config Error: Unknown argument for location directive: '" + key + "'. Check 'webserv -h'");
}

std::vector<Server> Parse::config2server(std::vector<Config> configs)
{
	std::vector<Server> servers;
	servers.reserve(configs.size());

	for (std::vector<Config>::iterator it = configs.begin(); it != configs.end(); ++it)
	{
		Server cur_server;
		loc_details server_default;

		for (std::map<string, std::map<string, string> >::iterator iter = it->location.begin();
				iter != it->location.end(); ++iter)
		{
			for (std::map<std::string, std::string>::iterator iter = it->defaults.begin();
					iter != it->defaults.end(); ++iter)
			{
				defaults(iter, cur_server, server_default);
			}
			loc_details tmp;
			for (std::map<string, string>::iterator keys_iterator = iter->second.begin();
					keys_iterator != iter->second.end(); ++keys_iterator)
			{
				locations(keys_iterator, tmp);
			}
			std::vector<string> paths = wbs::split(iter->first, " ");
			for (size_t i = 0; i < paths.size(); ++i)
			{
				if (paths[i][0] != '/')
					throw std::runtime_error("Config Error: Locations keys most bes start with as slash: `/'.");
				cur_server.locations[paths[i]] = tmp;
			}
		}
		cur_server.locations["default"] = server_default;

		if (cur_server.get_host() == static_cast<in_addr_t> (-1))
			throw std::runtime_error("Config Error: Host is required: A valid host must be specified for the server configuration.");
		if (cur_server.port == static_cast<uint32_t> (-1))
			throw std::runtime_error("Config Error: Port is required: A valid port must be specified for the server configuration.");
		if (cur_server.locations["default"].root.empty())
			throw std::runtime_error("Config Error: Root directory is required: Please specify a valid root path for the server.");
		
		for (map<string, loc_details>::iterator it = cur_server.locations.begin(); it != cur_server.locations.end(); ++it)
		{
			if (it->second.enable_upload == true && cur_server.locations["default"].upload_path.empty())
				throw std::runtime_error("Config Error: Uploading is enabled somewhere but no upload path is specifed.");
		}

		// for (size_t i = 0; i < servers.size(); ++i)
		// {
		// 	if (servers[i].port == cur_server.port)
		// 		throw std::runtime_error("Config Error: Port: " + wbs::to_string(cur_server.port) + " is already used by another server.");
		// }
		servers.push_back(cur_server);
	}
	return servers;
}

std::vector<Server> Parse::get_servers(std::string filename)
{
	ifstream configFile(filename);
	std::vector<Server> servers;

	Config	cur_config;
	string	cur_path;

	bool inServer = false;
	bool inLocation = false;
	uint64_t	dotPosition;

	int serverBracketCount = 0;
	int locationBracketCount = 0;

	try
	{
		dotPosition = filename.rfind(".");
		if (dotPosition == string::npos || filename.substr(dotPosition) != ".conf")
			throw runtime_error("Invalid file type.");

		if (!configFile.is_open())
			throw runtime_error("Unable to open config file");

		std::vector<Config> configs;
		std::string line;
		while (std::getline(configFile, line))
		{
			line = wbs::get_trimed(line);

			if (line.empty() || line[0] == '#')
				continue;

			if (is_server(line))
			{
				if (inServer)
					throw runtime_error("Syntax Error: Nested server blocks are not allowed.");
				inServer = true;

				if (line.find("{") != std::string::npos)
					++serverBracketCount;
				else
				{
					std::getline(configFile, line);
					line = wbs::get_trimed(line);
					if (line != "{")
						throw runtime_error("Syntax Error: Missing '{' after server declaration.");
					++serverBracketCount;
				}
				continue;
			}

			if (is_location(line, cur_path))
			{
				if (!inServer)
					throw runtime_error("Syntax Error: Location block outside of server block.");
				if (inLocation)
					throw runtime_error("Syntax Error: Nested location blocks are not allowed.");
				if (cur_config.location.find(cur_path) == cur_config.location.end())
						cur_config.location[cur_path];
					else
						throw runtime_error("Config Error: Location: '" + cur_path + "' is already defined in server config #");
				inLocation = true;
				++locationBracketCount;
				continue;
			}

			if (line == "}")
			{
				if (inLocation)
				{
					--locationBracketCount;
					if (locationBracketCount == 0)
						inLocation = false;
				}
				else if (inServer)
				{
					--serverBracketCount;
					if (serverBracketCount == 0)
					{
						inServer = false;
						configs.push_back(cur_config);
						cur_config.clear();
					}
				}
				else
					throw runtime_error("Syntax Error: Unmatched closing bracket.");
				continue;
			}

			size_t semicolonPos = line.find(';');
			size_t firstspace = line.find_first_of(" \t");
			if (semicolonPos != std::string::npos && line[line.size() - 1] == ';')
			{
				std::string key = wbs::get_trimed(line.substr(0, firstspace));
				std::string value = wbs::get_trimed(line.substr(firstspace + 1));
				value.resize(value.size() - 1);

				if (key.empty() || value.empty())
					throw runtime_error("Syntax Error: Malformed key-value pair.");

				if (inLocation)
				{
					if (key == "cgi_ext")
						cur_config.location[cur_path][key] += ("," + value);
					else if (cur_config.location[cur_path].find(key) == cur_config.location[cur_path].end())
						cur_config.location[cur_path][key] = value;
					else
						throw runtime_error("Config Error: '" + key + "' is already defined in Location: " + cur_path + "server config#");
				}
				else if (inServer)
				{
					if (key == "error_page" || key == "cgi_executor")
						cur_config.defaults[key] += ("," + value);
					else if (cur_config.defaults.find(key) == cur_config.defaults.end())
						cur_config.defaults[key] = value;
					else
						throw std::runtime_error("Config Error: '" + key + "' is already defined in server config#");
				}
				else
					throw runtime_error("Syntax Error: Key-value pair outside of valid block.");
			}
			else
				throw runtime_error("Syntax Error: Missing ';' at the end of the line.");
		}

		if (serverBracketCount != 0)
			throw runtime_error("Syntax Error: Unmatched brackets in server block.");
		if (locationBracketCount != 0)
			throw runtime_error("Syntax Error: Unmatched brackets in location block.");

		servers = config2server(configs);
		if (!servers.size())
			throw runtime_error("Config Error: contain no server directive");
		configFile.close();
	}
	catch (runtime_error &e)
	{
		cerr << "webserv: `" << filename << "' " << e.what() << endl;
		configFile.close();
		std::exit(EXIT_FAILURE);
	}

	return (servers);
}
