#pragma once

#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <list>
#include <sys/stat.h>

struct loc_details
{
    int status_code;
    bool auto_index;
    bool has_cgi;
    std::string root;
    std::vector<std::string> index_files;
    std::vector<std::string> allowed_methods;

    //POST
    std::string client_max_body_size; //TODO init with ""
};

class request
{
    private:
        std::unordered_map<std::string, std::string> headers;
        std::string req;
        std::string method;
        std::string HTTP;
        bool has_body;
        size_t max_body_size;

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
        bool is_req_well_formed();
        bool get_matched_loc_for_req_uri();
        // bool get_resource_type();
        bool is_location_have_redir();
        bool is_method_allowed_in_loc();
        int get_request_resource();

        bool is_valid_URI();
};

