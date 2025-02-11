#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include "webserv.hpp"

using namespace std; //TODO Dupl

struct server_info
{
	string server_name;
	string server_port;
	string remote_addr;
};

struct loc_details
{
	map<int, string>	error_pages;
	map<string, string>	cgi_excutor;
	vector<string>		allowed_methods;
	vector<string>		cgi_extentions;
	vector<string>		index_path;
	string				root;
	string				redir_to;
	string				upload_path;
	bool				auto_index;
	bool				enable_upload;
	bool				has_cgi;
	bool				has_slash; 
	int					status_code;
	uint64_t			client_max_body_size;

	loc_details()
	:	error_pages(),
		cgi_excutor(),
		allowed_methods(),
		cgi_extentions(),
		index_path(),
		root(""),
		redir_to(""),
		upload_path(""),
		auto_index(false),
		enable_upload(false),
		has_cgi(false),
		has_slash(false),
		status_code(0),
		client_max_body_size(0)
			{}


	// TODO  TMP 

	void print() const
	{
		cout << BLUE << "----------------------------------------------------------------------" << RESET << endl;
		cout << BLUE << "Location Details:" << RESET << endl;

		cout << GREEN << "    Status Code: " << WHITE << status_code << RESET << endl;
		cout << GREEN << "    Index Path: ";
		if (index_path.empty())
		{
			cout << RED << "None" << RESET << endl;
		}
		else
		{
			for (size_t i = 0; i < index_path.size(); ++i)
			{
				cout << CYAN << index_path[i] << RESET;
				if (i != index_path.size() - 1)
					cout << ", ";
			}
			cout << endl;
		}

		cout << GREEN << "    Allowed Methods: ";
		if (allowed_methods.empty())
		{
			cout << RED << "None" << RESET << endl;
		}
		else
		{
			for (size_t i = 0; i < allowed_methods.size(); ++i)
			{
				cout << CYAN << allowed_methods[i] << RESET;
				if (i != allowed_methods.size() - 1)
					cout << ", ";
			}
			cout << endl;
		}

		cout << GREEN << "    Autoindex: " << WHITE << (auto_index ? "true" : "false") << RESET << endl;
		cout << GREEN << "    Has CGI: " << WHITE << (has_cgi ? "true" : "false") << RESET << endl;
		cout << GREEN << "    Upload-enable: " << WHITE << (enable_upload ? "true" : "false") << RESET << endl;
		cout << GREEN << "    Root: " << WHITE << root << RESET << endl;
		cout << GREEN << "    Redirection Path: " << WHITE << (redir_to.empty() ? "None" : redir_to) << RESET << endl;
		cout << GREEN << "    Client Max Body Size: " << WHITE << client_max_body_size << " bytes" << RESET << endl;

		cout << GREEN << "    Error Pages:" << RESET << endl;
		if (error_pages.empty())
		{
			cout << RED << "        None" << RESET << endl;
		}
		else
		{
			for (map<int, string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
			{
				cout << CYAN << "        " << it->first << ": " << it->second << RESET << endl;
			}
		}

		cout << GREEN << "    CGI Extensions:" << RESET << endl;
		if (cgi_extentions.empty())
		{
			cout << RED << "        None" << RESET << endl;
		}
		else
		{
			for (vector<string>::const_iterator it = cgi_extentions.begin(); it != cgi_extentions.end(); ++it)
			{
				cout << CYAN << "        " << "'" <<  *it << "'" << RESET << endl;
			}
		}

		cout << GREEN << "    CGI Excutors:" << RESET << endl;
		if (cgi_excutor.empty())
		{
			cout << RED << "        None" << RESET << endl;
		}
		else
		{
			for (map<string, string>::const_iterator it = cgi_excutor.begin(); it != cgi_excutor.end(); ++it)
			{
				cout << CYAN << "         [" << it->first << "] :" << it->second << RESET << endl;
			}
		}
		cout << BLUE << "----------------------------------------------------------------------" << RESET << endl;
	}
};

#endif /* LOCATIONS_HPP */
