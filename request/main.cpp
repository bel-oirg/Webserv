#include "Response.hpp"

int main()
{
    std::string all = 
        "GET /pool/ HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: plain/text\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

    response res(all);
}