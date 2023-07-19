# include "../includes/socket.hpp"
# include "../includes/webserv.hpp"

void    Socket::check_methods_allowed(Server &server,int j){
    std::string method =  server._requests[server._pollfds[j].fd].requestLine.method;
    std::string methods_str[3] = {"GET","POST","DELETE"};
    size_t i = 0;
    for (; i < 3; i++){
        if(method == methods_str[i]){
            if(server.configs[0]->_locations->begin()->_methods[i] == 0){
                server._pollfds[j].revents = POLLNVAL;
                server._responses[server._pollfds[j].fd].setStatusCode("405");
            }
        }
    }
    if(i == 3){
        server._pollfds[j].revents = POLLNVAL;
        server._responses[server._pollfds[j].fd].setStatusCode("405");
    }
}

void    Socket::check_methods(Server &server,int j){
    std::string method =  server._requests[server._pollfds[j].fd].requestLine.method;
    std::string methods_str[3] = {"GET","POST","DELETE"};
    size_t i = 0;
    for (; i < 3; i++){
        if(method == "GET")
            server._responses[server._pollfds[j].fd].GET(server, j);
        // else if(method == "POST")
        //     server._responses[server._pollfds[j].fd].POST(server, j);
        // else if(method == "DELETE")
        //     server._responses[server._pollfds[j].fd].DELETE(server, j);
    }
}

void    Socket::status_response(Server  &server,int j){
    std::string status = server._responses[server._pollfds[j].fd].getStatusCode();
    if(status == "200"){
        server._responses[server._pollfds[j].fd].setReasonPhrase("OK");
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
}

void    Socket::content_type(){

    std::ifstream input_file("/Users/mac/Desktop/Webserv/conf/mime.types");
    if(!input_file){
        std::cout<<"Error opening file mime.types"<<std::endl;
        throw std::exception();
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
    // //creat a iterator to print the map
    // std::map<std::string, std::string>::iterator it;
    // it = _types.begin();
    // while (it != _types.end()) {
    //     std::cout << it->first << "   " << it->second << std::endl;
    //     it++;
    // }
}

void    Socket::content_type(Server &server,int j){
    std::string path = server._requests[server._pollfds[j].fd].requestLine.url;
    std::string extension = path.substr(path.find_last_of(".") + 1);
    if(_types.find(extension) != _types.end()){
        server._responses[server._pollfds[j].fd].setContentType(_types[extension]);
    }
    else{
        server._responses[server._pollfds[j].fd].setStatusCode("415");
        server._responses[server._pollfds[j].fd].setContentType("text/plain");
    }
}

void    Socket::prepare_response(Server &server,int j){
    check_methods_allowed(server,j);
    content_type(server,j);
    status_response(server,j);
    check_methods(server,j);
}
