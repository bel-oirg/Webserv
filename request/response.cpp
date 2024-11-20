#include "response.hpp"

void response::fill_status()
{
    status[200] = std::string("OK");
    status[201] = std::string("Created");
    status[204] = std::string("No Content");
    status[301] = std::string("Moved Permanently");
    status[400] = std::string("Bad Request");
    status[403] = std::string("Forbidden");
    status[404] = std::string("Not Found");
    status[405] = std::string("Method Not Allowed");
    status[409] = std::string("Conflict");
    status[413] = std::string("Content Too Large");
    status[414] = std::string("URI Too Long");
    status[500] = std::string("Internal Server Error");
    status[501] = std::string("Not Implemented");
}

