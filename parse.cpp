// #include "parse.hpp"
#include "webserv.hpp"
#include "locations.hpp"
#include "server.hpp"
#include "parse.hpp"
#include "clients.hpp"
#include <algorithm>

using namespace std;

// POS checking the syntax of locations 

std::string hostToString(in_addr_t host)
{
    struct in_addr addr;
    addr.s_addr = host;
    return inet_ntoa(addr);
}

std::string trim(const std::string& str)
{
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
bool is_location(const std::string& line, string &path)
{
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

std::vector<std::string> split(const std::string& str, string delimiters)
{
	size_t start = 0, end = 0;
	std::vector<string> splited;


	try
	{
		while ( (end = str.find_first_of(delimiters, start)) != string::npos)
		{
			string cur_splited = str.substr(start, end - start);
			cur_splited.erase(0, cur_splited.find_first_not_of(" \t"));
			cur_splited.erase(cur_splited.find_last_not_of(" \t") + 1);
			if (!cur_splited.empty())
				splited.push_back(cur_splited);
			start = end + 1;
		}
		
		string last = str.substr(start);
		last.erase(0, last.find_first_not_of(" \t"));
		last.erase(last.find_last_not_of(" \t") + 1);
		splited.push_back(last);
	}
	catch (...)
	{
		throw runtime_error(__func__);
	}

    return splited;
}


// void	locations_syntax(loc_details &loc)
// {
// 	if (loc.root.empty())
// 	{

// 	}
// }

void	defaults(std::map<string, string>::iterator iter, Server &server, loc_details &loc)
{
	string key = iter->first;
	string value = iter->second;
	if (value[0] == '\127')
		value.erase(0, 1);

	if (key == "listen")
	{
		int port;
		if (!for_each(value.begin(), value.end(), ::isdigit))
			throw runtime_error("[Error] Invalid port value: '" + value + "'. Port must be a numeric value.");
		else
			port = atoi(value.c_str());
		if (port > 65535 || port < 0)
			throw runtime_error("[Error] Port value '" + value + "' is out of range. Valid range is 0 to 65535.");
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
		if (host == 0)
			throw runtime_error("[Error] Invalid host address: '" + value + "'. Provide a valid IPv4 address.");
		server.host = host;
	}

	else if (key == "index")
	{
		if (value.empty())
			throw runtime_error("[Error] The 'index' directive cannot be empty. Provide a valid file name.");
		else
			loc.index_path = value;
	}
	else if (key == "error_page")
	{
		int code ;
		string page;
		std::vector<string> pages = split(value, ",\127"); // TODO maybe check multipple delemeters ,,,
		for (size_t i = 0; i < pages.size(); i++)
		{
			std::istringstream stream(pages[i]);
			stream >> std::skipws;
			if (!(stream >> code >> page && stream.eof()))
				throw runtime_error("[Error] Syntax error in 'error_page' directive. Expected format: '<error_code> <page_path>'.");
			loc.error_pages.insert(make_pair(code, page));
		}
	}
	else if (key == "client_max_body_size")
	{
		if (all_of(value.begin(), value.end(), ::isdigit))
		{
			uint32_t cmbs_value;
			cmbs_value = atoll(value.c_str());
			loc.client_max_body_size = cmbs_value;
		}
		else 
			throw runtime_error("[Error] Invalid value for 'client_max_body_size': '" + value + "'. Expected a numeric value.");
	}
	else if (key == "root")
	{
		loc.root = value;
	}
	else if (key == "allowed_methods")
	{
		loc.allowed_methods = split(value, " /t");
		for (size_t i = 0; i < loc.allowed_methods.size(); i++)
		{
			if (!	(loc.allowed_methods[i] == "GET" ||
					loc.allowed_methods[i] == "POST" ||
					loc.allowed_methods[i] == "DELETE" ))
				throw runtime_error("[Error] Invalid method: '" + loc.allowed_methods[i] + "'. Allowed methods are: GET, POST, DELETE.");

		}
	}
	else if (key == "autoindex")
	{
		if (value == "on")
			loc.auto_index = true;
		else if (value == "off")
			loc.auto_index = false;
		else
			throw runtime_error("[Error] Invalid value for 'autoindex': '" + value + "'. Valid options are: 'on' or 'off'.");
	}
	else
	{
		throw runtime_error("[Error] Unknown argument: '" + key + "'. Check 'webserv -h' for valid options.");
	}
}


void	locations(map<string, string>::iterator loc, loc_details &dest)
{
	string key = loc->first;
	string value = loc->second;
	if (value[0] == '\127')
		value.erase(0, 1);
	if (key == "allowed_methods")
	{
		dest.allowed_methods = split(value, " /t");
		for (size_t i = 0; i < dest.allowed_methods.size(); i++)
		{
			if (!	(dest.allowed_methods[i] == "GET" ||
					dest.allowed_methods[i] == "POST" ||
					dest.allowed_methods[i] == "DELETE" ))
				throw runtime_error("[Error] Invalid method: '" + dest.allowed_methods[i] + "'. Allowed methods are: GET, POST, DELETE.");
		}
	}
	else if (key == "autoindex")
	{
		if (value == "on")
			dest.auto_index = true;
		else if (value == "off")
			dest.auto_index = false;
		else
			throw runtime_error("[Error] Invalid value for 'autoindex': '" + value + "'. Valid options are: 'on' or 'off'.");
	}
	else if (key == "root")
	{
		if (value.empty())
			throw runtime_error("[Error] The 'root' directive cannot be empty. Provide a valid file name.");
		dest.root = value;
	}
	else if (key == "index")
	{
		if (value.empty())
			throw runtime_error("[Error] The 'index' directive cannot be empty. Provide a valid file name.");
		dest.index_path = value;
		// TODO
	}
	else if (key == "return")
	{
		if (value.empty())
			throw runtime_error("[Error] The 'return' directive cannot be empty. Provide a valid file name.");
		dest.redir_to = value;
	}
	else if (key == "client_max_body_size") // FIXME may this nor allowed here
	{
		if (all_of(value.begin(), value.end(), ::isdigit))
		{
			uint32_t cmbs_value;
			cmbs_value = atoll(value.c_str());
			dest.client_max_body_size = cmbs_value;
		}
		else 
			throw runtime_error("[Error] Invalid value for 'client_max_body_size': '" + value + "'. Expected a numeric value.");
	}
	else if (key == "cgi_pass")
	{
		if (value == "on")
			dest.has_cgi = true;
		else if (value == "off")
			dest.has_cgi = false;
		else
			throw runtime_error("[Error] Invalid value for 'cgi_pass': '" + value + "'. Valid options are: 'on' or 'off'.");
	}
	else if (key == "upload_path")
	{
		if (value.empty())
			throw runtime_error("[Error] The 'upload_path' directive cannot be empty. Provide a valid file name.");
		dest.upload_path = value;
	}
	else if (key == "upload_enable")
	{
		if (value == "on")
			dest.enable_upload = true;
		else if (value == "off")
			dest.enable_upload = false;
		else
			throw runtime_error("[Error] Invalid value for 'upload_enable': '" + value + "'. Valid options are: 'on' or 'off'.");
	}
	else
	{
			throw runtime_error("[Error] Unknown argument for location directive: '" + key + "'. Check 'webserv -h'");
	}
}

std::vector<Server> Parse::config2server(std::vector<Config> configs)
{
	std::vector<Server> servers;
	loc_details	tmp;

	try 
	{
		// reserve size for number of servers
		servers.reserve(configs.size());

		// loop over all configs and check syntax and parameters
		for (std::vector<Config>::iterator it = configs.begin(); it != configs.end(); it++)
		{
			 Server cur_server;
			 // iterate over all all defaults in one server
			loc_details 		server_default;
			 for (std::map<std::string, std::string>::iterator iter = it->defaults.begin();
			 	iter != it->defaults.end(); ++iter)
				{
					defaults(iter, cur_server, server_default);
				}
			cur_server.locations["default"] = server_default;
			for (std::map< string, std::map<string, string> >::iterator iter = it->location.begin();
			 	iter != it->location.end(); ++iter)
				{
					// cout << "\tLocation : "  << iter->first << endl;
					for (std::map<string, string>::iterator keys_iterator = iter->second.begin();
							keys_iterator != iter->second.end() ; keys_iterator++)
							{
								locations(keys_iterator, tmp);
							}
					std::vector<string> paths = split(iter->first, " ");
					for (size_t i = 0; i < paths.size(); i++)
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
        size_t firstspace = line.find_first_of(" \t");
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
                cur_config.defaults[key] +=  ( "\127" + value);
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
		cerr << "webserv : `" << configFile.name() << "' " << e.what() << endl;
		configFile.close();
	}

	return (servers);
}
