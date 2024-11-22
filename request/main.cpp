#include "Response.hpp"

int main()
{
    std::string all = 
        "GET / HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 32\r\n"
        "\r\n";
    
    response res(all);
}