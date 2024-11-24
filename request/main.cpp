#include "Response.hpp"

int main()
{
    std::string all = 
        "POST / HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: multipart/form-data; boundary=delimiter123\r\n"
        "Content-Length: 32\r\n"
        "\r\n"
        "--delimiter123\r\n"
        "Content-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\r\n\r\n"
        "value2\r\n"
        "--delimiter123--";
    
    response res(all);
}