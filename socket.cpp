#include"socket.hpp"
#include "request/request.hpp"

Socket::Socket(){
    _ServerSocket = 0;
    _ClientSocket = 0;
    _maxFd = 0;
    _nclients = 0;
    _bytesRead = 0;
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    _ClientAddressSize = sizeof(_ClientAddress);
    memset(&_ServerAddress,0,sizeof(_ServerAddress));
    memset(&_ClientAddress,0,sizeof(_ClientAddress));
}

Socket::~Socket(){
    close(_ServerSocket);
    for(int i = 0 ; i < _nclients; i++){
        close(_pollfds[i].fd);
    }
}

void    Socket::setupServer(int port,std::string ip){
    _ServerSocket = socket(AF_INET,SOCK_STREAM,0);
    if (_ServerSocket < 0) {
        std::cout << "Error creating socket" << std::endl;
        close(_ServerSocket);
        exit(1);
    }
    std::cout << "Socket created" << std::endl;
    int server_flag = fcntl(_ServerSocket,F_GETFL,0);
    if (server_flag == -1 ) {
        std::cout << "Error getting socket flags" << std::endl;
        close(_ServerSocket);
        exit(1);
    }
    if(fcntl(_ServerSocket,F_SETFL,server_flag | O_NONBLOCK) == -1){
        std::cout << "Error setting socket flags" << std::endl;
        close(_ServerSocket);
        exit(1);
    }
    std::cout << "Socket flags set" << std::endl;
    int opt = 1;
    if (setsockopt(_ServerSocket,SOL_SOCKET,SO_REUSEADDR ,&opt,sizeof(opt))) {  //segpipe
        std::cout << "Error setting socket options" << std::endl;
        close(_ServerSocket);
        exit(1);
    }
    _ServerAddress.sin_family = AF_INET;
    if(ip == "0.0.0.0")
        _ServerAddress.sin_addr.s_addr = INADDR_ANY;
    else
        _ServerAddress.sin_addr.s_addr = inet_addr(ip.c_str());
    _ServerAddress.sin_port = htons(port);
    if (bind(_ServerSocket,(struct sockaddr *)&_ServerAddress,sizeof(_ServerAddress)) < 0) {
        std::cout << "Error binding socket" << std::endl;
        close(_ServerSocket);
        exit(1);
    }
    std::cout << "Socket binded" << std::endl;
    if (listen(_ServerSocket,SOMAXCONN) < 0) {
        std::cout << "Error listening socket" << std::endl;
        close(_ServerSocket);
        exit(1);
    }
    std::cout << "Socket listening" << std::endl;
    _maxFd = _ServerSocket + 1;
    std::cout << "Server setup complete" << std::endl;
    acceptConnection();
}

void    Socket::acceptConnection(){
    ParseRequest   request;
    while (true){
        FD_SET(_ServerSocket,&_read_set);
        // FD_SET(_ServerSocket,&_write_set);
        // if(_pollfds[i].events == POLLIN)
            select(_maxFd,&_read_set,NULL,NULL,NULL);
        if (FD_ISSET(_ServerSocket,&_read_set)) {
            int clientSocket = accept(_ServerSocket,(struct sockaddr *)&_ClientAddress, &_ClientAddressSize);
            if (clientSocket == -1) {
                if(errno != EWOULDBLOCK && errno != EAGAIN){
                    std::cout << "Error accepting socket" << std::endl;
                    close(_ServerSocket);
                    exit(1);
                }
            }
            std::cout << "Socket accepted" << std::endl;
            _pollfds.push_back((struct pollfd){clientSocket,POLLIN,0});
            FD_SET(_pollfds.back().fd,&_read_set);
            _nclients++;
            _maxFd = clientSocket + 1;
        }

        for(unsigned long i = 0 ; i < _pollfds.size(); i++){
            if (FD_ISSET(_pollfds[i].fd,&_read_set)) {
                char buffer[1024] = {0};
                _bytesRead = recv(_pollfds[i].fd,buffer,sizeof(buffer) - 1,0);
                if (_bytesRead == -1) {
                    if(errno != EWOULDBLOCK && errno != EAGAIN){
                        std::cout << "Error reading socket" << std::endl;
                        close(_ServerSocket);
                        exit(1);
                    }
                }
                if (_pollfds[i].events == POLLOUT) {
                    std::cout << "Socket closed" << std::endl;
                    FD_CLR(_pollfds[i].fd,&_read_set);
                    close(_pollfds[i].fd);
                    _pollfds.erase(_pollfds.begin() + i);
                    _nclients--;
                    _maxFd--;
                    if(_maxFd < _ServerSocket + 1)
                        _maxFd = _ServerSocket + 1;
                }
                else{
                    std::cout << "Socket read" << std::endl;
                    // std::cout << buffer << std::endl;
                    request.ParseHttpRequest(buffer);
                    close(_pollfds[i].fd);
                    FD_CLR(_pollfds[i].fd,&_read_set);
                    _pollfds.erase(_pollfds.begin() + i);
                    _nclients--;
                    _maxFd--;
                    if(_maxFd < _ServerSocket + 1)
                        _maxFd = _ServerSocket + 1;
                    
                    // detect the end of the request
                    // if(strstr(buffer,"\r\n\r\n") != NULL){
                        // std::cout << "End of request" << std::endl;
                        // FD_CLR(_pollfds[i].fd,&_read_set);
                        // FD_SET(_pollfds[i].fd,&_write_set);
                        // _pollfds[i].events = POLLOUT;
                    // }
                    // std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
                    // send(_pollfds[i].fd , hello.c_str() , hello.length() , 0 );
                    // _pollfds[i].events = POLLOUT;
                }
            }
            // else if(FD_ISSET(_pollfds[i].fd,&_write_set)){
            //     std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
            //     send(_pollfds[i].fd , hello.c_str() , hello.length() , 0 );
            //     FD_CLR(_pollfds[i].fd,&_write_set);
            //     FD_SET(_pollfds[i].fd,&_read_set);
            //     _pollfds[i].events = POLLOUT;
            // }
        }
    }
    
}