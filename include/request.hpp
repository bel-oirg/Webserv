#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "server.hpp"
#include "webserv.hpp"

class request
{
    private:
        string boundary;
        string file_name;
        std::fstream outfile;
        string HTTP;
        string req;
        size_t uploaded_size;
        size_t length;

    protected:
        std::map<string, string> headers;
        std::map<string, loc_details> locations;
        loc_details current_loc;

        string resource_path;
        string PATH_INFO_URI;
        string method;
        string URI;
        string PATH_INFO;
        string body;
        string query;
        string correct_loc_name;
        int         stat_code;
        int         resource_type;
        bool        add_slash;
        bool        PATH_first;
        bool        chunked;

    public:
		bool		upload_eof;
        request(std::string raw_req, std::map<std::string, loc_details> locations);
        virtual ~request();
        void    display_req();
        bool    valid_method();
        bool    valid_elem(std::string elem);
        int     is_req_well_formed();
        bool    get_matched_loc_for_req_uri();
        bool    is_location_have_redir();
        bool    is_method_allowed_in_loc();
        int     get_request_resource();
        int     init_parse_req();
        bool    is_uri_has_slash_in_end();
        bool    is_dir_has_index_path();
        bool    prepare_index_path();
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
        bool unchunk_body();
        int process_multipart(std::string &body);

        //DELETE
        bool has_write_access_on_folder();
        bool delete_all_folder_content(std::string ress_path);
};


#endif /* REQUEST_HPP */
