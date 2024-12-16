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


	loc_details() : error_pages(),
          allowed_methods(),
          index_path(""),
          root(""),
          redir_to(""),
          upload_path(""),
          auto_index(false),
          enable_upload(false),
          has_cgi(false),
          has_slash(false),
          status_code(0),
          client_max_body_size(0) {}




   void print() const {
    cout << "----------------------------------------------------------------------" << std::endl;
    cout << "Location Details:" << std::endl;
    cout << "    Status Code: " << status_code << std::endl;
    cout << "    Index Path: " << index_path << std::endl;

    cout << "    Allowed Methods: ";
    if (allowed_methods.empty()) {
        cout << "None" << std::endl;
    } else {
        for (size_t i = 0; i < allowed_methods.size(); ++i) {
            cout << allowed_methods[i];
            if (i != allowed_methods.size() - 1)
                cout << ", ";
        }
        cout << std::endl;
    }

    cout << "    Autoindex: " << (auto_index ? "true" : "false") << std::endl;
    cout << "    Has CGI: " << (has_cgi ? "true" : "false") << std::endl;
    cout << "    Root: " << root << std::endl;
    cout << "    Redirection Path: " << (redir_to.empty() ? "None" : redir_to) << std::endl;
    cout << "    Client Max Body Size: " << client_max_body_size << " bytes" << std::endl;

    cout << "    Error Pages:" << std::endl;
    if (error_pages.empty()) {
        cout << "        None" << std::endl;
    } else {
        for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            cout << "        " << it->first << ": " << it->second << std::endl;
        }
    }

    cout << "    Has Slash: " << (has_slash ? "true" : "false") << std::endl;
    // cout << "----------------------------------------------------------------------" << std::endl;
}

};


#endif /* LOCATIONS_HPP */
