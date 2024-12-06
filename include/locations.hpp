#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include "webserv.hpp"

using namespace std;

struct loc_details
{
    map<int, string>		error_pages;
    vector<string>			allowed_methods;
	string					index_path; // index_path
    string					root;
    string					redir_to; // redir_to
	string					upload_path;
    bool					auto_index;
	bool					enable_upload;
    bool					has_cgi;
    bool					has_slash;
	int						status_code;
    size_t					client_max_body_size;


	loc_details() : status_code(0), auto_index(false), enable_upload(false),
					has_cgi(false), has_slash(false), client_max_body_size(0) {}




   void print() const {
    std::cout << "----------------------------------------------------------------------" << std::endl;
    std::cout << "Location Details:" << std::endl;
    std::cout << "    Status Code: " << status_code << std::endl;
    std::cout << "    Index Path: " << index_path << std::endl;

    std::cout << "    Allowed Methods: ";
    if (allowed_methods.empty()) {
        std::cout << "None" << std::endl;
    } else {
        for (size_t i = 0; i < allowed_methods.size(); ++i) {
            std::cout << allowed_methods[i];
            if (i != allowed_methods.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "    Autoindex: " << (auto_index ? "true" : "false") << std::endl;
    std::cout << "    Has CGI: " << (has_cgi ? "true" : "false") << std::endl;
    std::cout << "    Root: " << root << std::endl;
    std::cout << "    Redirection Path: " << (redir_to.empty() ? "None" : redir_to) << std::endl;
    std::cout << "    Client Max Body Size: " << client_max_body_size << " bytes" << std::endl;

    std::cout << "    Error Pages:" << std::endl;
    if (error_pages.empty()) {
        std::cout << "        None" << std::endl;
    } else {
        for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            std::cout << "        " << it->first << ": " << it->second << std::endl;
        }
    }

    std::cout << "    Has Slash: " << (has_slash ? "true" : "false") << std::endl;
    // std::cout << "----------------------------------------------------------------------" << std::endl;
}

};


#endif /* LOCATIONS_HPP */
