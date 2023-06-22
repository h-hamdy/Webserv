#include "socket.hpp"

void    Get_response(ParseRequest &request){
    std::cout << "GET response" << std::endl;
    std::cout << "request._method: " << request._method << std::endl;
    std::cout << "request._url: " << request._url << std::endl;
    std::cout << "request._http_v: " << request._http_v << std::endl;
    std::cout << "request._body: " << request._body << std::endl;
    std::cout << "request._headers: " << std::endl;
    for (std::map<std::string, std::string>::iterator it = request._headers.begin(); it != request._headers.end(); ++it) {
        std::cout << "   " << it->first << ": " << it->second << std::endl;
    }
}