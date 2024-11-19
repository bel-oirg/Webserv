#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#define MAX_URI_SIZE 2048

struct loc_details
{
    int status_code;
    bool auto_index;
    bool has_cgi;
    std::string root;
    std::vector<std::string> index_files;
    std::vector<std::string> allowed_methods;

};



class request
{
    private:
        std::unordered_map<std::string, std::string> headers;
        std::string req;
        std::string method;
        std::string HTTP;
        bool has_body;
        std::string client_max_body_size; //can be global or per

    protected:
        std::string URI;
        std::string ressource_path;
        std::unordered_map<std::string, loc_details> locations;
        std::unordered_map<std::string, loc_details>::iterator current_loc;

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
        bool if_loc_support_upload();

        //DELETE
        bool has_write_access_on_folder();
        bool delete_all_folder_content(std::string ress_path);
};

