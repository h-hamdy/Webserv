# include "../includes/webserv.hpp"

#include <sys/stat.h>
#include <filesystem>

bool pathExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool createDirectory(const std::string& path) {
    int result = mkdir(path.c_str(), 0777);
    return (result == 0);
}

void check_upload_path(std::string pathToCheck) {

    if (pathExists(pathToCheck))
        std::cout << "Path exists!" << std::endl;
    else {
        std::cout << "Path does not exist!" << std::endl;
        if (createDirectory(pathToCheck))
            std::cout << "Directory created successfully!" << std::endl;
        else {
            std::cout << pathToCheck << std::endl;
            std::cout << "Failed to create directory!" << std::endl;
            throw 404;
        }
    }
}

std::vector<Location>::iterator	Server::matching (const std::string &host, const std::string &port, ParseRequest &req, Server &server, int j) {
    (void)j;
    (void)server;
    std::vector<Config *>::iterator it = configs.begin();
    std::vector<Location>::iterator location;
    if (host == (*it)->_host || host == (*it)->_server_name) {
        int Post = std::atoi(port.c_str());
        if (Post == (*it)->_port) {
            // std::cout << "Server matched" << std::endl;
            location = (*it)->getLocation(req.requestLine.url);
            if (location == (*it)->_locations->end())
                throw 404;
            if (!location->_redirect.empty()) {
                // std::cout << "seting responce for redirection" << std::endl;
                // server._responses[server._pollfds[j].fd].setResponse(location->_redirect);
                // server._responses[server._pollfds[j].fd].setStatusCode("301");
                // if(server._responses[server._pollfds[j].fd].sending_data){
                //     server._responses[server._pollfds[j].fd].setResponse("");
                //     server._responses[server._pollfds[j].fd].close_connection = true;
                // }
            }
            if (location->_upload_path.empty()) {
                std::string resource = req.requestLine.url.substr(location->_url.length());
                req.path = "./root" + location->_root;
                if (req.requestLine.method == "GET")
                    req.path += resource;
                return location;
            }
            else {
                if (location->_upload_path[0] != '/' || location->_upload_path[location->_upload_path.size() - 1] != '/') {
                    std::cout << "Failed to create directory!" << std::endl;
                    throw 404;
                }
                if (req.requestLine.method == "POST")
                    req.path = "./root" + location->_root + location->_upload_path;
                else {
                    std::string resource = req.requestLine.url.substr(location->_url.length());
                    req.path = "./root" + location->_root + resource;
                }
                if (req.requestLine.method == "POST")
                    check_upload_path(req.path);
            }
        }
    }
    else
        throw 400;
    return location;
}