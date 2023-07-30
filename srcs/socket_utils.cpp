# include "../includes/socket.hpp"
# include "../includes/webserv.hpp"

void    Socket::check_methods_allowed(Server &server,int j){
    std::string method =  server._requests[server._pollfds[j].fd].requestLine.method;
    std::string methods_str[3] = {"GET","POST","DELETE"};
    size_t i = 0;
    for (; i < 3; i++){
        if(method == methods_str[i]){
            if(server.configs[0]->_locations->begin()->_methods[i] == false){
                server._pollfds[j].revents = POLLNVAL;
                server._responses[server._pollfds[j].fd].setStatusCode("405");
            }
        }
    }
}

bool fileExists(const char* directoryPath, const char* fileName, std::string &indexfile) {
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        std::cerr << "Error opening directory." << std::endl;
        return false;
    }
    bool found = false;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::strncmp(entry->d_name, fileName, 5) == 0) {
            indexfile = entry->d_name;
            found = true;
            break;
        }
    }
    closedir(dir);
    return found;
}

void HandleDir(const std::string& path, std::vector<Location>::iterator &location, std::string &filePath) {
    (void)location;
    std::string indexFile;
    std::string extention;
    if (path[path.length() - 1] != '/') {
        path + "/"; 
        // set responce
        throw 301;
    }
    else {
        if (location->_cgi_extensions.size() == 0)
            throw 4035;
        std::cout << "check index files" << std::endl;
        if (!fileExists(path.c_str(), "index", indexFile))
            throw 4036;
        size_t lastDotPos = indexFile.rfind('.');
        if (lastDotPos != std::string::npos) {
            std::vector<std::string>::iterator it;
            extention = indexFile.substr(lastDotPos);
            for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
                if ((*it)[0] == ' ')
                    (*it).erase(1);
                if (extention == *it) {
                    (void)filePath;
                    std::cout << "pass to cgi" << std::endl;
                    return ;
                }
            }
            if (it == location->_cgi_extensions.end())
                throw 404;
        }
        else
            throw 4037;
    }
}

void HandleFile(const std::string& path, std::vector<Location>::iterator &location, Server &server, int j, std::string  &filePath) {
    std::string extention;
    std::cout << "file" << std::endl;
    if (location->_cgi_extensions.size() == 0)
        throw 4030;
    size_t lastDotPos = path.rfind('.');
    if (lastDotPos != std::string::npos) {
        std::vector<std::string>::iterator it;
        extention = path.substr(lastDotPos);
        for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
            if (extention == *it) {
                std::cout << "Da5lo a7biba" << std::endl;
                CgiProcess(server, j, path, extention, filePath);
                std::cout << "rah da5l" << std::endl;
                return ;    
            }
        }
        if (it == location->_cgi_extensions.end())
            throw 4031;
    }
    else
        throw 4032;
}

void HandlePathType(const std::string& path, std::vector<Location>::iterator &location, Server &server, int j, std::string &filePath)
{
    struct stat fileStat;
    std::cout << "Path=" << path << std::endl;
    if (stat(path.c_str(), &fileStat) == 0)
    {
        if (S_ISREG(fileStat.st_mode))
            HandleFile(path, location, server, j, filePath);
        else if (S_ISDIR(fileStat.st_mode))
            HandleDir(path, location, filePath);
        else
            throw 404;
    }
    else
        throw 404;
}

void    Socket::check_methods(Server &server,int j){
    std::string method =  server._requests[server._pollfds[j].fd].requestLine.method;
        if(method == "GET")
            server._responses[server._pollfds[j].fd].GET(server, j);
        else if(method == "POST"){
        std::cout << "Location does not support upload" << std::endl;
        std::string resource = server._requests[ server._pollfds[j].fd].requestLine.url.substr(server._location_match->_url.length());
        HandlePathType(server._requests[server._pollfds[j].fd].path + resource, server._location_match, server, j,server._requests[ server._pollfds[j].fd].filePath);
    }
        else if(method == "DELETE")
            server._responses[server._pollfds[j].fd].DELETE(server, j);
}

void    Socket::status_response(Server  &server,int j){
    std::string status = server._responses[server._pollfds[j].fd].getStatusCode();
    if(status == "200"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("OK");
        return ;
    }
    else if(status == "201"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Created");
    }
    else if(status == "202"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Accepted");
    }
    else if(status == "204"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("No Content");
    }
    else if(status == "301"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Moved Permanently");
    }
    else if(status == "302"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Found");
    }
    else if(status == "304"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Not Modified");
    }
    else if(status == "400"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Bad Request");
    }
    else if(status == "401"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Unauthorized");
    }
    else if(status == "403"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Forbidden");
    }
    else if(status == "404"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Not Found");
    }
    else if(status == "405"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Method Not Allowed");
    }
    else if(status == "413"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Payload Too Large");
    }
    else if(status == "415"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Unsupported Media Type");
    }
    else if(status == "500"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Internal Server Error");
    }
    else if(status == "501"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("Not Implemented");
    }
    else if(status == "505"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("HTTP Version Not Supported");
    }
    server._responses[server._pollfds[j].fd].setErrPage(server, j);
}

void    Socket::content_type(){

    std::ifstream input_file("./conf/mime.types");
    if(!input_file){
        std::cout<<"Error opening file mime.types"<<std::endl;
        // throw std::exception();
    }
    std::stringstream f;
    f << input_file.rdbuf();
    std::string input = f.str();
    std::istringstream iss(input);
    std::string line;

    while (std::getline(iss, line)) {
        std::istringstream lineIss(line);
        std::string fileType;
        std::string extensions;

        lineIss >> fileType;

        while (lineIss >> extensions) {
            if(extensions == "{" || extensions == "}")
                continue;
            extensions.erase(std::remove(extensions.begin(), extensions.end(), ';'), extensions.end());
            _types[extensions] = fileType;
        }
    }
    std::cout << "Mime types loaded" << std::endl;
}

int   Socket::content_type(Server &server,int j){
    std::string path = server._requests[server._pollfds[j].fd].requestLine.url;
    std::string extension = path.substr(path.find_last_of(".") + 1);
    if(_types.find(extension) != _types.end()){
        server._responses[server._pollfds[j].fd].setContentType(_types[extension]);
        return 1;
    }
    else{
        // server._responses[server._pollfds[j].fd].setStatusCode("415");
        server._responses[server._pollfds[j].fd].setContentType("text/html");
        return 0;
    }
}

void    Socket::prepare_response(Server &server,int j){
    check_methods_allowed(server,j);
    content_type(server,j);
    if(server._responses[server._pollfds[j].fd].getStatusCode() == "200")
        check_methods(server,j);
    status_response(server,j);
}



void    send_chuncked_response(Server &server,int j){
	//response_not_send can be dont have /r/n and its valid
	// server._responses[server._pollfds[j].fd].set_Header_Response(server,j);
	std::string response = "";
	if(server._responses[server._pollfds[j].fd].response_not_send == "")
	    response = server._responses[server._pollfds[j].fd].getResponse();
	else
		response = server._responses[server._pollfds[j].fd].response_not_send;
    // std::cout<<"response to send: "<<response<<std::endl;
    if(response == "" && !server._responses[server._pollfds[j].fd].close_connection){
        return ;
    }
    int ret = send(server._pollfds[j].fd, response.c_str(), response.length(), 0);
    if(ret < 1 ){
        server._responses[server._pollfds[j].fd].response_not_send = response;
    }
    else if (ret < (int)response.length())
        server._responses[server._pollfds[j].fd].response_not_send = response.substr(ret);
	else
		server._responses[server._pollfds[j].fd].response_not_send = "";
    server._responses[server._pollfds[j].fd].setResponse("");
}