#include "Response.hpp"

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

std::string response::the_head()
{
    std::stringstream line;
    fill_status();
    line << "HTTP/1.1 " << status[this->stat_code] << "\n";
    line << "Connection: " << this->_connection << "\n";
    line << "Server: " << this->_server << "\n";
    line << "Content-Type: " << this->_content_type << "\n";
    line << "Transfer-Encoding: " << this->_transfer_encoding << "\n";
    return (line.str());
}

void response::set_content_length()
{
    //specifies the content of the response BODY
    //unless using transfer-encoding : chunked
    this->_content_length = -1;
    if (!this->has_body)
        this->_content_length = 0;
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

void response::set_connection()
{
    this->_connection = "keep-alive";
}

void response::set_content_type()
{
    this->_content_type = "";
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

//TODO no need for this for now i will use only content_len
// void response::set_transfer_encoding()
// {
    //TODO if there a content-length, you MUST NOT USE TRANSF_ENC
    // this->_transfer_encoding = "";
    // if (this->_content_length == -1)
    //     this->_transfer_encoding = "chunked";
// }

void response::set_location()
{
    if (this->stat_code == 301)
    {
        if (add_slash)
            this->_location = this->resource_path + "/";
        else
            this->_location = this->current_loc->second.redir_to;
    }
}

void response::set_body()
{
    std::stringstream ss;
    std::ifstream infile;
    _body = "";
    if (this->stat_code / 400)
    {
        infile.open("/Users/bel-oirg/Desktop/Webserv/Error_pages/" + std::to_string(this->stat_code) + ".html");
        if (!infile)
        {
            std::cerr << "Error opening error_page.html" << std::endl;
            return;
        }
    }
    else if (this->stat_code / 300)
    {
        _body = "Redirecting to another location. Status Code: " + std::to_string(this->stat_code);
        _content_length = _body.size();     
        return ;
    }
    else if (this->stat_code == 200)
    {
        infile.open(this->resource_path);
        if (!infile)
        {
            std::cerr << "Error opening 200" << std::endl;
            return;
        }
    }
    ss << infile.rdbuf();
    _body = ss.str();
    _content_length = _body.size();     
}

// response::response()
// {
//     resource_path = "/any.php";
//     stat_code = 200;
//     method = "GET"; //WHY

//     set_content_length();
//     set_transfer_encoding();
//     set_connection();
//     set_content_type();
//     set_server();
//     std::cout << the_head() << std::endl;
// }

/*
TODO unchunk data
*/