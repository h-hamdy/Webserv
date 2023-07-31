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
    if(fcntl(ServerSocket,F_SETFL,O_NONBLOCK) == -1){
        std::cout << "Error setting socket flags" << std::endl;
        close(ServerSocket);
        exit(1);
    }
    std::cout << "Socket flags set" << std::endl;
    int opt = 1;
    if (setsockopt(ServerSocket,SOL_SOCKET,SO_REUSEADDR ,&opt,sizeof(opt))) {
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

std::string generateRandomString(int length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int numCharacters = characters.length();

    std::string randomString;
    for (int i = 0; i < length; ++i) {
        int randomIndex = rand() % numCharacters;
        randomString += characters[randomIndex];
    }
    std::srand(static_cast<unsigned int>(std::time(0)));
    int randomNumber = std::rand();
    return randomString + std::to_string(randomNumber);
}

std::string get_ContentType (std::string value)
{
	size_t pos = value.find_last_of("/");
	std::string type;
	std::string rand;
	if (pos != std::string::npos) {
		type = value.substr(pos + 1);
		rand = generateRandomString(10);
		return (rand + "." + type);
	}
	throw 400;
}

void    POST (Server &_servers, int j, std::string &rest, std::string bb, std::string &path)
{
    if (_servers._requests[ _servers._pollfds[j].fd].requestLine.method == "POST" && _servers._requests[ _servers._pollfds[j].fd].create_file == true)
    {
        std::string filename;
        std::map<std::string, std::string>::iterator it = _servers._requests[ _servers._pollfds[j].fd].header.find("Content-Type");
        filename = get_ContentType(it->second);

        _servers._requests[ _servers._pollfds[j].fd].filePath = path + filename;
        _servers._requests[ _servers._pollfds[j].fd].file.open(_servers._requests[ _servers._pollfds[j].fd].filePath, std::ios::binary | std::ios::app | std::ios::ate);
        _servers._requests[ _servers._pollfds[j].fd]._EOF = 1;
        _servers._requests[ _servers._pollfds[j].fd].create_file = false;
    }
    if (_servers._requests[ _servers._pollfds[j].fd].requestLine.method == "POST") {
        if (!rest.empty()) {
            _servers._requests[ _servers._pollfds[j].fd].ParseBody(rest, _servers, j);
            rest = "";
        }
        else
            _servers._requests[ _servers._pollfds[j].fd].ParseBody(bb, _servers, j);
    }
}

void    Socket::acceptConnection(){
    ParseRequest   request;
    size_t i = 0;
    while (true){
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
                if(fcntl(clientSocket,F_SETFL,O_NONBLOCK) == -1){
                    std::cout << "Error setting socket flags" << std::endl;
                    continue;
                //sigpipe
                // int set = 1;
                // setsockopt(clientSocket,SOL_SOCKET,SO_NOSIGPIPE,&set,sizeof(int));
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
                char buffer[1025] = {0};
                 _servers[i]->_bytesRead = recv( _servers[i]->_pollfds[j].fd,buffer,1024 ,0);
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
                    std::string def_root = "./root";
                    if (_servers[i]->_bytesRead > 1) {
                        try {
                            std::vector<Location>::iterator location;
                            std::string bb(buffer, _servers[i]->_bytesRead);
                            if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method.empty())
                            {
                                _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].rest.append(bb);
                                if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].rest.find("\r\n\r\n") != std::string::npos) {
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].flag = true;
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].rest = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseHttpRequest(_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].rest, _servers[i]->_bytesRead,*_servers[i], j);
                                    bb = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].rest;
                                    size_t pos = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.find("?");
                                    if (pos != std::string::npos) {
                                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].queryString = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(pos + 1);
                                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(0, pos);
                                    }
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestStatusCode();
                                    std::map<std::string, std::string>::iterator it = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.find("Host");
                                    pos = it->second.find(":");
                                     _servers[i]->_location_match = _servers[i]->matching(it->second.substr(0, pos), it->second.substr(pos + 1), _servers[i]->_requests[ _servers[i]->_pollfds[j].fd], *_servers[i], j);
                                    if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST" && _servers[i]->configs[0]->postAllowed(_servers[i]->_location_match) == false)
                                        throw 405;
                                }
                            }
                            if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST" && _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].flag == true)
                                POST (*(_servers[i]), j, _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].rest, bb, _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].path);
                        }
                        catch (int status) {
                            if (status == 201){
                                std::cout << "Upload Created successfully!" << std::endl;
                                _servers[i]->_responses[_servers[i]->_pollfds[j].fd].setStatusCode(std::to_string(status));
                                // _servers[i]->_responses[_servers[i]->_pollfds[j].fd].setResponse("<html><head> <title>201 Created</title></head><body><h1>201 Created</h1><p>The request was successful, and a new resource has been created.</p> </body></html>");
                                _servers[i]->_responses[_servers[i]->_pollfds[j].fd].close_connection = true;
                                // _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]._EOF = 0;
                            }
                            else {
                                std::cout << status << std::endl;
                                std::cout << "Throw error page" << std::endl; 
                                _servers[i]->_responses[_servers[i]->_pollfds[j].fd].setStatusCode(std::to_string(status));
                                _servers[i]->_responses[_servers[i]->_pollfds[j].fd].close_connection = true;
                                _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]._EOF = 0;
                            }
                        }
                    }
                }
                else{
                    FD_SET(_servers[i]->_pollfds[j].fd, &_servers[i]->_write_set);
                    if(FD_ISSET( _servers[i]->_pollfds[j].fd,& _servers[i]->_write_set)){
                        //Implement a method specifically designed to generate a response.
                        if(_servers[i]->_responses[ _servers[i]->_pollfds[j].fd].response_not_send == ""){
                            prepare_response(*_servers[i],j);
                            _servers[i]->_responses[ _servers[i]->_pollfds[j].fd].set_Header_Response(*_servers[i],j);
                        }
                        send_chuncked_response(*_servers[i],j);
                    }
                    if(_servers[i]->_responses[ _servers[i]->_pollfds[j].fd].close_connection == true && _servers[i]->_responses[ _servers[i]->_pollfds[j].fd].response_not_send == ""){
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