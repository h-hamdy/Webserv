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
#include "webserv.hpp"


#define MAXLINE 1024
class Server;
class Socket{
    private:
        std::vector<Server *> _servers;
        timeval timeout;
    public:
        Socket(char *file);
        ~Socket();
        void    setupServer();
        void    acceptConnection();
};
// void    Get_response(ParseRequest &request);
#endif