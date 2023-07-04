# include "../includes/socket.hpp"
# include "../includes/webserv.hpp"

Socket::Socket(char *file) {
    std::cout<<"creating Socket"<<std::endl;
    std::cout<<"file: "<<file<<std::endl;
    std::vector<Server *> servers = getServers(file);
    std::cout<<"servers size: "<<servers.size()<<std::endl;
    this->_servers = servers;
    this->timeout.tv_sec = 0;
    this->timeout.tv_usec = 0;
}

Socket::~Socket(){
    for (size_t i = 0; i < _servers.size(); i++)
    {
        close(_servers[i]->_ServerSocket);
        for (size_t j = 0; j < _servers[i]->_pollfds.size(); j++)
        {
            close(_servers[i]->_pollfds[j].fd);
        }
    }
    std::cout<<"Socket destroyed"<<std::endl;
}

void    Socket::setupServer(){
    std::cout<<"Setuping Servers"<<std::endl;
    for(size_t i = 0 ; i < _servers.size(); i++){
    int ServerSocket = socket(AF_INET,SOCK_STREAM,0);
    this->_servers[i]->_ServerSocket = ServerSocket;
    if (ServerSocket < 0) {
        std::cout << "Error creating socket" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    std::cout << "Socket created" << std::endl;
    int server_flag = fcntl(ServerSocket,F_GETFL,0);
    if (server_flag == -1 ) {
        std::cout << "Error getting socket flags" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    if(fcntl(ServerSocket,F_SETFL,O_NONBLOCK) == -1){  //fcntl(fd, F_SETFL, O_NONBLOCK);
        std::cout << "Error setting socket flags" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    std::cout << "Socket flags set" << std::endl;
    int opt = 1;
    if (setsockopt(ServerSocket,SOL_SOCKET,SO_REUSEADDR ,&opt,sizeof(opt))) {  //segpipe
        std::cout << "Error setting socket options" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    _servers[i]->_ServerAddress.sin_family = AF_INET;
    if(_servers[i]->configs.front()->_host == "0.0.0.0")
         _servers[i]->_ServerAddress.sin_addr.s_addr = INADDR_ANY;
    else
         _servers[i]->_ServerAddress.sin_addr.s_addr = inet_addr(_servers[i]->configs.front()->_host.c_str());
    _servers[i]->_ServerAddress.sin_port = htons(_servers[i]->configs.front()->_port);
    if (bind(ServerSocket,(struct sockaddr *)& _servers[i]->_ServerAddress,sizeof( _servers[i]->_ServerAddress)) < 0) {
        std::cout << "Error binding socket" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    std::cout << "Socket binded" << std::endl;
    if (listen(ServerSocket,SOMAXCONN) < 0) {
        std::cout << "Error listening socket" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    std::cout << "Socket listening" << std::endl;
     _servers[i]->_maxFd = ServerSocket + 1;
    std::cout << "Servers setup complete" << std::endl;
    }
    acceptConnection();
}

void    Socket::acceptConnection(){
    ParseRequest   request;//houssam add this to the class //request.parseRequest();
    size_t i = 0;
    while (true){
        // FD_SET(_ServerSocket,&_write_set);
        if(i == _servers.size())
            i = 0;
        FD_SET( _servers[i]->_ServerSocket,& _servers[i]->_read_set);
        fd_set copy_read_set = _servers[i]->_read_set;
        if(select( _servers[i]->_maxFd,&copy_read_set,0,0,&timeout) == -1){
            std::cout << "Error selecting socket" << std::endl;
            this->~Socket();
            exit(1);
        }
        if (FD_ISSET( _servers[i]->_ServerSocket,& _servers[i]->_read_set)) {
            memset(&_servers[i]->_ClientAddress,0,sizeof(_servers[i]->_ClientAddress));
            int clientSocket = accept( _servers[i]->_ServerSocket,(struct sockaddr *)& _servers[i]->_ClientAddress, & _servers[i]->_ClientAddressSize);
            if (clientSocket == -1) {
                if(errno != EWOULDBLOCK && errno != EAGAIN){
                    std::cout << "Error accepting socket" << std::endl;
                    close( _servers[i]->_ServerSocket);
                    exit(1);
                }
            }
            else{
                if(fcntl(clientSocket,F_SETFL,O_NONBLOCK) == -1){  //fcntl(fd, F_SETFL, O_NONBLOCK);
                    std::cout << "Error setting socket flags" << std::endl;
                    this->~Socket();
                    exit(1);
                }
                std::cout << "Socket accepted" << std::endl;
                 _servers[i]->_pollfds.push_back((struct pollfd){clientSocket,POLLIN,0});
                FD_SET( _servers[i]->_pollfds.back().fd,& _servers[i]->_read_set);
                 _servers[i]->_nclients++;
                 _servers[i]->_maxFd = clientSocket + 1;
                _servers[i]->_requests[clientSocket];
                // _servers[i]->_requests.insert(std::make_pair(clientSocket));
            }
        }
        for(unsigned long j = 0 ; j <  _servers[i]->_pollfds.size(); j++) {
            if (FD_ISSET( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set)) {
                char buffer[500] = {0};
                 _servers[i]->_bytesRead = recv( _servers[i]->_pollfds[j].fd,buffer,sizeof(buffer) - 1,0);
                if ( _servers[i]->_bytesRead == -1) {
                    if(errno != EWOULDBLOCK && errno != EAGAIN) {
                        std::cout << "Error reading socket" << std::endl;
                        close( _servers[i]->_ServerSocket);
                        exit(1);
                    }
                }
                if ( _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]._EOF != 0) {
                    // std::cout << "Socket read" << std::endl;
                    // request.ParseHttpRequest(buffer, _servers[i]->_bytesRead);
                    // if (buffer[0] != '\0') {
                    if (_servers[i]->_bytesRead > 1) {
                        std::string bb(buffer, _servers[i]->_bytesRead);
                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseHttpRequest(bb, _servers[i]->_bytesRead);
                    }
                    // }
                    // _servers[i]->_requests.insert(std::make_pair(_servers[i]->_pollfds[j].fd,request.ParseHttpRequest(buffer)));
                    // request.ParseHttpRequest(buffer);
                    // std::cout<<"------------>\n"<<_servers[i]->_requests[_servers[i]->_pollfds[j].fd].file<<std::endl;
                    // std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/html\n";
                    // std::ifstream file("assests/index.html");
                    // std::string str;
                    // std::string htm
                    // while (std::getline(file,str)) {
                    //     html += str;
                    // }
                    // hello += "Content-Length: " + std::to_string(html.length()) + "\n\n" + html;
                    // std::cout << buffer << std::endl;
                    // send( _servers[i]->_pollfds[j].fd , hello.c_str() , hello.length() , 0 );
                    //  std::cout << "Socket closed" << std::endl;
                    // close( _servers[i]->_pollfds[j].fd);
                    // FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set);
                    //  _servers[i]->_pollfds.erase( _servers[i]->_pollfds.begin() + j);
                    //  _servers[i]->_nclients--;
                    //  _servers[i]->_maxFd --;
                    // if( _servers[i]->_maxFd <=  _servers[i]->_ServerSocket + 1)
                    //      _servers[i]->_maxFd =  _servers[i]->_ServerSocket + 1;
                }
                else{
                     std::cout << "Socket closed" << std::endl;
                    close( _servers[i]->_pollfds[j].fd);
                    FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set);
                     _servers[i]->_pollfds.erase( _servers[i]->_pollfds.begin() + j);
                     _servers[i]->_nclients--;
                     _servers[i]->_maxFd --;
                    if( _servers[i]->_maxFd <=  _servers[i]->_ServerSocket + 1)
                         _servers[i]->_maxFd =  _servers[i]->_ServerSocket + 1;
                }
            }
        }
        i++;
    }
    
}