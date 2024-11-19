#pragma once

#include "request.hpp"

class DELETE : public request
{
    public:
        bool delete_all_folder_content(std::string ress_path);
        bool has_write_access_on_folder();
};

