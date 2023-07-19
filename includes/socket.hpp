#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/stat.h>
#include "webserv.hpp"


#define MAXLINE 1024
class Server;
class Socket{
    private:
        std::vector<Server *> _servers;
        timeval timeout;
        std::map<std::string, std::string> _types;
    public:
        Socket(char *file);
        ~Socket();
        void    setupServer();
        void    acceptConnection();
        void    check_methods_allowed(Server &server,int j);
        void    check_methods(Server &server,int j);
        void    status_response(Server &server,int j);
        void    content_type();
        void    content_type(Server &server,int j);
        void    prepare_response(Server &server,int j);
};
// void    Get_response(ParseRequest &request);
#endif