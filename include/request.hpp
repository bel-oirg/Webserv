#pragma once

#include <map>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "server.hpp"

#define UPLOAD_DIR "/Users/bel-oirg/Desktop/Webserv/Upload/"
//TODO change UPLOAD_DIR based on config file

#define MAX_URI_SIZE 2048
#define GLOBAL_CLIENT_MAX_BODY_SIZE 4000

#define p std::cout << 

class request
{
    private:
        string boundary;
        string file_name;
        std::fstream outfile;
        std::string HTTP;
        std::string req;
        std::map<std::string, loc_details> locations;

    protected:
        std::map<std::string, std::string> headers;
        loc_details current_loc;

        std::string resource_path;
        std::string method;
        std::string URI;
        std::string body;
        std::string query;
        std::string correct_loc_name;
        int         stat_code;
        bool        add_slash;
        bool respond_with_autoindex;

    public:
        request(std::string raw_req, std::map<std::string, loc_details> locations);
        void    display_req();
        bool    valid_method();
        bool    valid_elem(std::string elem);
        int     is_req_well_formed();
        bool    get_matched_loc_for_req_uri();
        bool    is_location_have_redir();
        bool    is_method_allowed_in_loc();
        int     get_request_resource();
        int     init_parse_req();
        int     set_to_get(string buffer, int seq);
        bool    is_uri_has_slash_in_end();
        bool    is_dir_has_index_path();
        bool    if_location_has_cgi();
        int     req_arch();
        bool    is_valid_URI();

        int     GET();
        int     POST();
        int     DELETE();

        //GET
        bool get_auto_index();

        //POST
        int if_loc_support_upload();
        bool prep_body_post();
        bool unchunk_body();
        bool process_multipart(std::string body);

        //DELETE
        bool has_write_access_on_folder();
        bool delete_all_folder_content(std::string ress_path);
};