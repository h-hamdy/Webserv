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
#include "../includes/webserv.hpp"


#define MAXLINE 1024
class Server;
class Socket{
    private:
        // int _ServerSocket;
        // int _ClientSocket;
        // int _maxFd;
        // int _nclients;
        // fd_set _read_set;
        // fd_set _write_set;
        // ssize_t _bytesRead;
        // struct sockaddr_in _ServerAddress;
        // struct sockaddr_in _ClientAddress;
        // socklen_t _ClientAddressSize;
        // std::map <std::string,struct sockaddr_in> _ClientAddress;
        // std::vector<struct pollfd> _pollfds;
        std::vector<Server *> _servers;
    public:
        Socket(char *file);
        ~Socket();
        void    setupServer();
        void    acceptConnection();
};

#endif