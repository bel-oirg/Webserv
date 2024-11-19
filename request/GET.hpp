#pragma once

#include "request.hpp"

class GET : public request
{
    public:

        bool get_auto_index();
};