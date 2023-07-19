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

void    Socket::acceptConnection(){
    ParseRequest   request;
    size_t i = 0;
    while (true){
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
                if(fcntl(clientSocket,F_SETFL,O_NONBLOCK) == -1){
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
            }
        }
        for(unsigned long j = 0 ; j <  _servers[i]->_pollfds.size(); j++) {
            if (FD_ISSET( _servers[i]->_pollfds[j].fd,& _servers[i]->_read_set)) {
                char buffer[1000] = {0};
                 _servers[i]->_bytesRead = recv( _servers[i]->_pollfds[j].fd,buffer,sizeof(buffer) - 1,0);
                if ( _servers[i]->_bytesRead == -1) {
                    if(errno != EWOULDBLOCK && errno != EAGAIN) {
                        std::cout << "Error reading socket" << std::endl;
                        close( _servers[i]->_ServerSocket);
                        exit(1);
                    }
                }
                if ( _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]._EOF != 0) {
                    std::string rest;
                    if (_servers[i]->_bytesRead > 1) {
                        try {
                            std::vector<Location>::iterator location;
                            std::string bb(buffer, _servers[i]->_bytesRead);
                            if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method.empty()) {
                                    rest = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseHttpRequest(bb, _servers[i]->_bytesRead);
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.host = _servers[i]->configs[0]->_host;
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.port = std::to_string(_servers[i]->configs[0]->_port);
                                    size_t pos = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.find("?");
                                    if (pos != std::string::npos) {
                                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].queryString = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(pos + 1);
                                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(0, pos);
                                    }
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestStatusCode();
                                    std::vector<Location>::iterator _location = _servers[i]->configs[0]->getLocation(_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url);
                                    if (_location == _servers[i]->configs[0]->_locations->end()) {
                                        _location = _servers[i]->configs[0]->getLocation("/");
                                    }
                                    else {
                                        if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST" && _servers[i]->configs[0]->postAllowed(_location) == false)
                                            throw 405;
                                        std::map<std::string, std::string>::iterator it = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.find("Host");
                                        if (it != _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.end()) {
                                            size_t pos = it->second.find(":");
                                            location = _servers[i]->matching(it->second.substr(0, pos), it->second.substr(pos + 1), _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url);
                                        }
                                    }
                                }
                                if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST" && _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].create_file == true) {
                                    std::string filename;
                                    std::map<std::string, std::string>::iterator it = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.find("Content-Type");
                                    filename = get_ContentType(it->second);
                                    std::string filePath;
                                    if (!location->_upload_path.empty()) {
                                        std::vector<Location>::iterator location = _servers[i]->configs[0]->getLocation(_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url);
                                        filePath = location->_root + location->_upload_path + filename;
                                    }
                                    else {
                                        std::cout << "Location does not support upload" << std::endl;
                                        std::string resource = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(location->_url.length());
                                        if (resource.empty())
                                            throw 404;
                                        HandlePathType(location->_root + resource, location, _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]);
                                        // std::string pathToCgi = "/Users/hhamdy/Desktop/Webserv/cgi/";
                                        // filePath = pathToCgi + filename;
                                    }
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].file.open(filePath, std::ios::binary | std::ios::app | std::ios::ate);
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd]._EOF = 1;
                                    _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].create_file = false;
                                }
                                if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "POST") {
                                    if (!rest.empty())
                                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseBody(rest, _servers[i]->_bytesRead);
                                    else
                                        _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].ParseBody(bb, _servers[i]->_bytesRead);
                                }
                                else if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "DELETE") {
                                    if (_servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.method == "DELETE" && _servers[i]->configs[0]->deleteAllowed(location) == false)
                                        throw 405;
                                    std::string resourc = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url.substr(location->_url.length());
                                    DELETE(location->_root + resourc);
                                }
                            }
                            catch (int status) {
                                std::cout << status << std::endl;
                                if (status == 201)
                                    std::cout << "Upload Created successfully!" << std::endl;
                                else
                                    return ;
                            }
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
                    // std::map<std::string, std::string>::iterator it = _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.find("Host");
                    // try {
                    //     if (it != _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].header.end()) {
                    //         size_t pos = it->second.find(":");
                    //         _servers[i]->matching(it->second.substr(0, pos), it->second.substr(pos + 1), _servers[i]->_requests[ _servers[i]->_pollfds[j].fd].requestLine.url);
                    //     }
                    // }
                    // catch (int status) {
                    //     std::cout << status << std::endl;
                    // }
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