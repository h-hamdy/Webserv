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
    return randomString;
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

void HandleFile(const std::string& path, std::vector<Location>::iterator &location , ParseRequest &request) {
    std::string extention;

    if (location->_cgi_extensions.size() == 0)
        throw 403;
    size_t lastDotPos = path.rfind('.');
    if (lastDotPos != std::string::npos) {
        std::vector<std::string>::iterator it;
        extention = path.substr(lastDotPos);
        for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
            if (extention == *it) {
                std::cout << "Da5lo a7biba" << std::endl;
                CgiProcess(location, path, request, extention);
                std::cout << "rah da5l" << std::endl;
            }
        }
        if (it == location->_cgi_extensions.end())
            throw 404;
    }
    else
        throw 404;

}

void HandleDir(const std::string& path, std::vector<Location>::iterator &location) {
    (void)location;
    std::string extention;
    std::cout << path << std::endl;
    if (path.length() - 1 != '/') {
        path + "/"; // add / to the path
        throw 301;
    }
    else {
        // append the file choosed in the auto indexing to the path and search and check his extention if it's valid
        // if (!location->_directory_listing)
        //     throw 403;
        // extention = path.substr(lastDotPos);
        // for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
        //     if (extention == *it)
        //         std::cout << "Pass " << path << " To cgi" << std::endl;
        // }

    }
}

bool isDirectory(std::string path) {
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return (S_ISDIR(path_stat.st_mode));
}

void DELETE(std::string path) {
	if (access(path.c_str(), F_OK) != 0)
		throw 404;
	else if (access(path.c_str(), W_OK) != 0)
		throw 403;
	else if (isDirectory(path))
		throw 403;
	else if (remove(path.c_str()) != 0)
		throw 500;
	else
		throw 200;
    
    std::cout << "File Deleted Seccessfully" << std::endl;
}

void HandlePathType(const std::string& path, std::vector<Location>::iterator &location, ParseRequest &request)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0)
    {
        if (S_ISREG(fileStat.st_mode))
            HandleFile(path, location, request);
        else if (S_ISDIR(fileStat.st_mode))
            HandleDir(path, location);
        else
            throw 404;
    }
    else
        throw 404;
}

void    POST (Server &_servers, int j, std::string rest, std::string bb)
{
    if (_servers._requests[ _servers._pollfds[j].fd].requestLine.method == "POST" && _servers._requests[ _servers._pollfds[j].fd].create_file == true)
    {
        std::string filename;
        std::map<std::string, std::string>::iterator it = _servers._requests[ _servers._pollfds[j].fd].header.find("Content-Type");
        filename = get_ContentType(it->second);
        std::string filePath;
        std::vector<Location>::iterator location = _servers._location_match;

        // must protect if location is null <<===========================================================================
            
        if (!location->_upload_path.empty()) {
            // location = _servers[i]->configs[0]->getLocation(_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url);
            filePath = location->_root + location->_upload_path + filename;
            std::cout << filePath << std::endl;
        }
        else {
            std::cout << "Location does not support upload" << std::endl;
            std::string resource = _servers._requests[ _servers._pollfds[j].fd].requestLine.url.substr(location->_url.length());
            std::cout << location->_root + resource << std::endl;
            HandlePathType(location->_root + resource, location, _servers._requests[ _servers._pollfds[j].fd]);
        }
        _servers._requests[ _servers._pollfds[j].fd].file.open(filePath, std::ios::binary | std::ios::app | std::ios::ate);
        _servers._requests[ _servers._pollfds[j].fd]._EOF = 1;
        _servers._requests[ _servers._pollfds[j].fd].create_file = false;
    }
    if (_servers._requests[ _servers._pollfds[j].fd].requestLine.method == "POST") {
        if (!rest.empty())
            _servers._requests[ _servers._pollfds[j].fd].ParseBody(rest, _servers, j);
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
                    std::string rest;
                    if (_servers[i]->_bytesRead > 1) {
                        try {
                            std::vector<Location>::iterator location;
                            std::string bb(buffer, _servers[i]->_bytesRead);
                            if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method.empty()) {
                                rest = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseHttpRequest(bb, _servers[i]->_bytesRead,*_servers[i], j);
                                size_t pos = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.find("?");
                                if (pos != std::string::npos) {
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].queryString = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(pos + 1);
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(0, pos);
                                }
                                _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestStatusCode();
                                std::map<std::string, std::string>::iterator it = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.find("Host");
                                pos = it->second.find(":");
                                std::string path;
                                _servers[i]->_location_match = _servers[i]->matching(it->second.substr(0, pos), it->second.substr(pos + 1), _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url);
                                if (_servers[i]->_location_match != _servers[i]->configs[0]->_locations->end())
                                    path =  _servers[i]->configs[0]->_locations->begin()->_root + _servers[i]->_requests[_servers[i]->_pollfds[j].fd].requestLine.url;
                                else
                                    path = _servers[i]->_location_match->_root + _servers[i]->_requests[_servers[i]->_pollfds[j].fd].requestLine.url;
                                std::cout << path << std::endl;
                                // _servers[i]->_location_match = _servers[i]->configs[0]->_locations->begin();
                                if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST" && _servers[i]->configs[0]->postAllowed(_servers[i]->_location_match) == false)
                                    throw 405;
                            }
                            if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST")
                                POST (*(_servers[i]), j, rest, bb);
                            else if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "DELETE") {
                                if (_servers[i]->configs[0]->deleteAllowed(location) == false)
                                    throw 405;
                                std::string resourc = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(location->_url.length());
                                DELETE(location->_root + resourc);
                            }
                        }
                        catch (int status) {
                            std::cout << status << std::endl;
                            if (status == 201){
                                std::cout << "Upload Created successfully!" << std::endl; 
                                _servers[i]->_responses[ _servers[i]->_pollfds[j].fd].close_connection = true;
                            }
                            else {
                                std::cout << "Throw error page" << std::endl;
                                return ;
                            }
                        }
                    }
                }
                else{
                    FD_SET(_servers[i]->_pollfds[j].fd, &_servers[i]->_write_set);
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