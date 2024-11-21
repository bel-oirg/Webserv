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

void response::set_content_length()
{
    //specifies the content of the response BODY
    //unless using transfer-encoding : chunked
}

void response::set_server()
{
    this->_server = "Webserv 2.0";
}

// void response::set_location()
// {
//     if (!(this->stat_code / 300))
//     {

//     }
// }

void response::set_content_type()
{
    //TODO _content_type init with ""
    if (this->stat_code == 204 || this->stat_code == 304)
        return ;
    std::unordered_map<std::string, std::string> mime; //TODO static const for efficiency

    mime["html"] = "text/html";
    mime["htm"] = "text/html";
    mime["shtml"] = "text/html";
    mime["css"] = "text/css";
    mime["xml"] = "text/xml";
    mime["gif"] = "image/gif";
    mime["jpeg"] = "image/jpeg";
    mime["jpg"] = "image/jpeg";
    mime["js"] = "application/javascript";
    mime["atom"] = "application/atom+xml";
    mime["rss"] = "application/rss+xml";
    mime["txt"] = "text/plain";
    mime["png"] = "image/png";
    mime["json"] = "application/json";

    _content_type = "text/plain";
    size_t dot_p = this->resource_path.find_last_of('.');
    if (dot_p == std::string::npos)
        return ;

    std::string ext = this->resource_path.substr(dot_p + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    std::unordered_map<std::string, std::string>::iterator it = mime.find(ext);
    if (it != mime.end())
        this->_content_type = it->second;
}

void resopnse::set_transfer_encoding()
{
    this->_transfer_encoding = "chunked";
}


/*
unchunk data
*/