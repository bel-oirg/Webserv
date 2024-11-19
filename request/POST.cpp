#include "POST.hpp"

bool POST::if_loc_support_upload()
{
    return (!this->current_loc->second.client_max_body_size.empty());
}

bool POST::get_request_resource()
{
    
}