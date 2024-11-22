#include "request.hpp"

int main()
{
    std::string all = 
        "POST /submit HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 32\r\n"
        "\r\n"
        "{\"username\":\"john\",\"password\":\"123\"}";
    
    request req(all);
    if (req.is_req_well_formed())
    {
        std::cout << "Valid" << std::endl;
        req.display_req();
    }
    else
        std::cout << "NOT Valid" << std::endl;
}



/*
#include "Response.hpp"

int main()
{
    response a;
}
*/