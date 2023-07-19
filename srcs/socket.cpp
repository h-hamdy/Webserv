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
    content_type();
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
    FD_SET( _servers[i]->_ServerSocket,& _servers[i]->_read_set);
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
        fd_set copy_read_set = _servers[i]->_read_set;
        fd_set copy_write_set = _servers[i]->_write_set;
        if(select( _servers[i]->_maxFd,&copy_read_set,&copy_write_set,0,&timeout) == -1){
            std::cout << "Error selecting socket" << std::endl;
            continue;
            // this->~Socket();
            // exit(1);
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
                    continue;
                    // this->~Socket();
                    // exit(1);
                }
                std::cout << "Socket accepted" << std::endl;
                 _servers[i]->_pollfds.push_back((struct pollfd){clientSocket,POLLIN,0});
                FD_SET( _servers[i]->_pollfds.back().fd,& _servers[i]->_read_set);
                 _servers[i]->_nclients++;
                 _servers[i]->_maxFd = clientSocket + 1;
                _servers[i]->_requests[clientSocket];
                _servers[i]->_responses[clientSocket];
            }
        }
        for(unsigned long j = 0 ; j <  _servers[i]->_pollfds.size(); j++) {
            if (FD_ISSET( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set)) {
                char buffer[2048] = {0};
                 _servers[i]->_bytesRead = recv( _servers[i]->_pollfds[j].fd,buffer,sizeof(buffer),0);
                if ( _servers[i]->_bytesRead < 1) {
                    if(errno != EWOULDBLOCK && errno != EAGAIN) {
                        std::cout << "Error reading socket" << std::endl;
                        close( _servers[i]->_pollfds[j].fd);
                        FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set);
                        FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_write_set);
                        _servers[i]->_requests.erase( _servers[i]->_pollfds[j].fd);
                        _servers[i]->_responses.erase( _servers[i]->_pollfds[j].fd);
                        _servers[i]->_pollfds.erase( _servers[i]->_pollfds.begin() + j);
                        _servers[i]->_nclients--;
                        continue;
                        
                    }
                }
                if ( _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]._EOF != 0) {
                    // request.ParseHttpRequest(buffer, _servers[i]->_bytesRead);
                    // if (buffer[0] != '\0') {
                    if (_servers[i]->_bytesRead > 1) {
                        std::string bb(buffer, _servers[i]->_bytesRead);
                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseHttpRequest(bb, _servers[i]->_bytesRead,*_servers[i],j);
                    }
                }
                else{
                    // Read request from the client
                    // Parse the request and extract relevant information

                    // Process the request
                    // Perform necessary business logic or data processing
                    // Check if the request requires CGI execution
                    // bool isCGI = /* Logic to determine if CGI execution is needed */

                    // if (isCGI) {
                        // Execute CGI script
                        // Pass request information to the CGI script (e.g., environment variables, request data)
                        // Capture the output generated by the CGI script
                        // Set appropriate status code, headers, and response body based on the CGI script output
                    // } else {
                        // Process the request
                        // Perform necessary business logic or data processing

                        // Construct the response
                        // Set appropriate status code, headers, and response body
                    // }

                    // Construct the response
                    // Set appropriate status code, headers, and response body

                    // Send the response to the client
                    // Format the response message and send it over the client socket

                    // Close the client socket

                    // Handle errors and exceptions
                    // FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set);
                    // FD_SET( _servers[i]->_pollfds[j].fd,& _servers[i]->_write_set);
                    if(FD_ISSET( _servers[i]->_pollfds[j].fd,& _servers[i]->_write_set)){
                       //I'm planning to move these lines into a separate function.
                        // check_methods(*_servers[i],j);
                        // content_type(*_servers[i],j);
                        // status_response(*_servers[i],j);
                        //Implement a method specifically designed to generate a response.
                        prepare_response(*_servers[i],j);
                        // _servers[i]->_responses[ _servers[i]->_pollfds[j].fd].GET(*_servers[i],j);
                        //print url 
                        // std::cout<<"url: "<<_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url<<std::endl;
                    }
                    if(_servers[i]->_responses[ _servers[i]->_pollfds[j].fd].close_connection == true){
                        std::cout << "Socket closed" << std::endl;
                        close( _servers[i]->_pollfds[j].fd);
                        FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set);
                        FD_CLR( _servers[i]->_pollfds[j].fd,& _servers[i]->_write_set);
                        _servers[i]->_requests.erase( _servers[i]->_pollfds[j].fd);
                        _servers[i]->_responses.erase( _servers[i]->_pollfds[j].fd);
                        _servers[i]->_pollfds.erase( _servers[i]->_pollfds.begin() + j);
                        _servers[i]->_nclients--;
                    }
                }
            }
        }
        i++;
    }
    
}