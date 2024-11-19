#pragma once

#include "request.hpp"

class GET : public request
{
    public:
        bool is_uri_has_slash_in_end();
        bool is_dir_has_index_files();
        bool get_auto_index();
        bool if_location_has_cgi();
};