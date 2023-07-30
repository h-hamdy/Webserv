#include "../../includes/response.hpp"
#include "../../includes/socket.hpp"
#include <filesystem>

void    Response::setErrPage(Server &serv,int j){
    std::string status = serv._responses[serv._pollfds[j].fd].getStatusCode();
    std::string status_message = serv._responses[serv._pollfds[j].fd].getReasonPhrase();
    std::string body = "<html><head><title>" + status + " " + status_message + "</title></head><body><center><h1>" + status + " " + status_message + "</h1></center><hr><center>webserv</center></body></html>";
    serv._responses[serv._pollfds[j].fd].setContentType("text/html");
    
    std::string path_err = serv.configs[0]->_error_pages[atoi(status.c_str())];
    std::cout << "path_err: " << path_err << std::endl;
    if(path_err != ""){
        std::ifstream file_err(path_err.c_str());
        if(file_err.is_open()){    
            std::string content((std::istreambuf_iterator<char>(file_err)), (std::istreambuf_iterator<char>()));
            body = content;
        }
    }
    serv._responses[serv._pollfds[j].fd].setResponse(body);
}