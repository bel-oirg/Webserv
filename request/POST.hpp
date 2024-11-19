#pragma once

#include "request.hpp"

class POST : public request
{
    public:
        bool if_loc_support_upload();
};