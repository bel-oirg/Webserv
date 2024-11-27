#include "parse.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <arpa/inet.h>

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to check if a line starts with "server" and is valid
bool is_server(const std::string& line)
{
    std::string trimmed = trim(line);

    if (trimmed.find("server") != 0)
        return false;

    std::string rem = trimmed.substr(6);
    rem = trim(rem);

    if (rem.empty() || (rem == "{"))
        return true;

    return false;
}

// Function to check if a line starts with "location" and ends with "{"
bool is_location(const std::string& line, string &path) {
    size_t pos = line.find("location");
    if (pos != std::string::npos) 
	{
        size_t openBracket = line.find("{", pos);
        path = line.substr(pos + 8, openBracket - (pos + 8));
        path.erase(0, path.find_first_not_of(" \t"));
        path.erase(path.find_last_not_of(" \t") + 1);
        return (!path.empty() && openBracket == line.length() - 1);
    }
    return false;
}

std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void	check_syntax(std::map<string, string>::iterator iter, Server &server, loc_details &loc)
{
	string key = iter->first;
	string value = iter->second;

	if (key == "listen")
	{
		int port;
		if (!for_each(value.begin(), value.end(), ::isdigit))
			throw runtime_error("invalid port");
		else
			port = atoi(value.c_str());
		if (port > 65535 || port < 0)
			throw runtime_error("port richded limit");
		server.port = port;
	}
	else if (key == "server_name")
	{
		server.server_name = value;
	}
	else if (key == "host")
	{
		in_addr_t host;
		host = inet_addr(value.c_str());
		if (host == -1)
			throw runtime_error("invalid host");
		server.host = host;
	}

	else if (key == "index") // for location
	{
		if (value.empty())
			throw runtime_error("index can't be empty");
		else
			loc.index_path = value;
	}
	else if (key == "error_page") // for location
	{
		int code ;
		string page;
		std::vector<string> pages = split(value, ',');
		for (int i = 0; i < pages.size(); i++)
		{
			std::istringstream stream(pages[i]);
			if (!(stream >> code >> page))
				throw runtime_error("syntax error for `error_page'");
			loc.error_pages[code] = page;
		}
	}
	else if (key == "client_max_body_size")
	{
		uint32_t cmbs_value;
		cmbs_value = atoll(value.c_str());
		loc.client_max_body_size = cmbs_value;
		cout << "max : " << cmbs_value << endl;
		// else 
		// 	throw runtime_error("invalid value for client_max_body_size");
	}
	else if (key == "root")
	{
		loc.root = value;
	}
	
}


void	set_location_data(map<string, string>::iterator loc, loc_details &dest)
{
	string key = loc->first;
	string value = loc->second;
	if (key == "allowed_methods")
	{
		dest.allowed_methods = split(value, ' ');
	}
	else if (key == "autoindex")
	{
		if (value == "on")
			dest.auto_index = true;
		else if (value == "off")
			dest.auto_index = false;
		else
			throw runtime_error ("invalid format for autoindex");
	}
	else if (key == "root")
	{
		dest.root = value;
		// else throw "Root is  invalid\n"; //TODO
	}
	else if (key == "index")
	{
		dest.index_path = value;
		// TODO
	}
	else if (key == "return")
	{
		dest.redir_to = value;
	}
	else if (key == "client_max_body_size")
	{
		dest.client_max_body_size = atoi(value.c_str());
	}
}

std::vector<Server> Parse::config2server(std::vector<Config> configs)
{
	std::vector<Server> servers;
	loc_details	tmp = {0};

	try 
	{
		// reserve size for number of servers
		servers.reserve(configs.size());

		// loop over all configs and check syntax and parameters
		for (std::vector<Config>::iterator it = configs.begin(); it != configs.end(); it++)
		{
			 Server cur_server;
			 // iterate over all all defaults in one server
			loc_details 		server_default = {0};
			 for (std::map<std::string, std::string>::iterator iter = it->defaults.begin();
			 	iter != it->defaults.end(); ++iter)
				{
					check_syntax(iter, cur_server, server_default);
				}
			cur_server.locations["default"] = server_default;
			for (std::map< string, std::map<string, string> >::iterator iter = it->location.begin();
			 	iter != it->location.end(); ++iter)
				{
					// cout << "\tLocation : "  << iter->first << endl;
					for (std::map<string, string>::iterator keys_iterator = iter->second.begin();
							keys_iterator != iter->second.end() ; keys_iterator++)
							{
								// cout << "key : " << keys_iterator->first << "  data : " << keys_iterator->second << endl;
								set_location_data(keys_iterator, tmp);
								// set_location_data()
							}
					std::vector<string> paths = split(iter->first, ' ');
					for (int i = 0; i < paths.size(); i++)
					{
						cur_server.locations[paths[i]] = tmp;
					}
				}
				servers.push_back(cur_server);
		}
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
		exit(1);
	}
	return servers;
}

// HACK remove this
std::string hostToString(in_addr_t host) {
    struct in_addr addr;
    addr.s_addr = host;
    return inet_ntoa(addr);
}

std::vector<Server> Parse::get_servers(std::string file_name)
{
	std::vector<Config> configs;
	Config				cur_config;
	wbs_ifstream configFile(file_name);
	string		cur_path;
	std::vector<Server> servers;

	bool inServer = false;
    bool inLocation = false;

	int serverBracketCount = 0; 
    int locationBracketCount = 0; 

	std::map<std::string, std::string> currentServer;

try{
   		 if (!configFile.is_open())
			throw runtime_error("Unable to open config file");

		 std::string line;
    while (std::getline(configFile, line))
	{
        line = trim(line);
	
        if (line.empty() || line[0] == '#') continue;

		// xx :Check for server block
        if (is_server(line)) 
		{
            if (inServer)
				throw runtime_error("Syntax Error: Nested server blocks are not allowed.");
            inServer = true;

            // Check {
            if (line.find("{") != std::string::npos)
                ++serverBracketCount;
			else 
			{
                // Expect {
                std::getline(configFile, line);
                line = trim(line);
                if (line != "{")
					throw runtime_error("Syntax Error: Missing '{' after server declaration.");
                ++serverBracketCount;
            }
            continue;
        }
		// end xx


		// vv Check for location block
        if (is_location(line, cur_path))
		{
			// cout << "LOCATION FOUND" << "in path: " << cur_path << endl;
            if (!inServer)
				throw runtime_error("Syntax Error: Location block outside of server block.");
            inLocation = true;
            ++locationBracketCount;
            continue;
        }
		// end vv

        // bb Handle closing brackets
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
                    currentServer.clear();
                }
            }
			else 
                throw runtime_error("Syntax Error: Unmatched closing bracket.");
            continue;
        }
		// end bb

        // cc Handle the keys and there value
	
        size_t semicolonPos = line.find(';');
        size_t firstspace = line.find_first_of(' ');
        if (semicolonPos != std::string::npos)
		{
            std::string key = trim(line.substr(0, firstspace));
            std::string value = trim(line.substr(firstspace + 1));
			value.pop_back(); // BUG c++ 11

            if (key.empty() || value.empty()) 
                throw runtime_error("Syntax Error: Malformed key-value pair.");

			if (inLocation)
			{
                cur_config.location[cur_path][key] = value;
            }
			else if (inServer)
			{
                cur_config.defaults[key] = value;
            }
			else
                throw runtime_error("Syntax Error: Key-value pair outside of valid block.");
        }
		else
            throw runtime_error("Syntax Error: Missing ';' at the end of the line.");
		// end cc
    }

    // Final checks for unmatched brackets
    if (serverBracketCount != 0)
		throw runtime_error("Syntax Error: Unmatched brackets in server block.");
    if (locationBracketCount != 0)
		throw runtime_error("Syntax Error: Unmatched brackets in location block.");

	// Close configfile
    configFile.close();

	servers = config2server(configs);
}
	// Catch syntax errors 
	catch (runtime_error &e)
	{
		cerr << "webserv config: `" << configFile.name() << "' " << e.what() << endl;
		configFile.close();
		std::exit(1);
	}

	return (servers);
}