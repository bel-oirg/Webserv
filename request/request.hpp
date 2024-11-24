#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#define MAX_URI_SIZE 2048
#define GLOBAL_CLIENT_MAX_BODY_SIZE 4000

#define p std::cout << 

struct loc_details
{
    int status_code;
    std::string redir_to;
    bool auto_index;
    bool has_cgi;
    bool add_slash;
    std::string root;
    size_t client_max_body_size;
    std::vector<std::string> index_files;
    std::vector<std::string> allowed_methods;

    loc_details()
        : status_code(0),
          redir_to(""),
          auto_index(false),
          has_cgi(false),
          add_slash(false),
          root(""),
          client_max_body_size(0),
          index_files(),
          allowed_methods() {}

    loc_details(
        int status_code,
        std::string redir_to,
        bool auto_index,
        bool has_cgi,
        bool add_slash,
        std::string root,
        size_t client_max_body_size,
        std::vector<std::string> index_files,
        std::vector<std::string> allowed_methods
    )
        : status_code(status_code),
          redir_to(redir_to),
          auto_index(auto_index),
          has_cgi(has_cgi),
          add_slash(add_slash),
          root(root),
          client_max_body_size(client_max_body_size),
          index_files(index_files),
          allowed_methods(allowed_methods)
    {}
};

class request
{
    protected:
        loc_details current_loc;
        std::unordered_map<std::string, loc_details> locations;
        std::unordered_map<std::string, std::string> headers;
        std::string resource_path;
        std::string req;
        std::string method;
        std::string HTTP;
        std::string URI;
        int         stat_code;
        bool        add_slash;
        bool        has_body;
        std::string _body;
        bool respond_with_autoindex;

        //POST
        std::unordered_map<std::string, std::string> upload_headers;


    public:
        request(std::string raw_req);
        void display_req();
        bool valid_method();
        bool valid_elem(std::string elem);
        int is_req_well_formed();
        bool get_matched_loc_for_req_uri();
        // bool get_resource_type();
        bool is_location_have_redir();
        bool is_method_allowed_in_loc();
        int get_request_resource();
        bool is_uri_has_slash_in_end();
        bool is_dir_has_index_files();
        bool if_location_has_cgi();
        int req_arch();
        int GET();
        int POST();
        int DELETE();

        //GET
        bool get_auto_index();

        //POST
        int if_loc_support_upload();

        //DELETE
        bool has_write_access_on_folder();
        bool delete_all_folder_content(std::string ress_path);
};