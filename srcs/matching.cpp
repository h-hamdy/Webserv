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
            std::cout << "Failed to create directory!" << std::endl;
            throw 404;
        }
    }
}

bool	Server::matching (const std::string &host, const std::string &port, std::string url) {
    std::vector<Config *>::iterator it = configs.begin();
    if (host == (*it)->_host || host == (*it)->_server_name) {
        int Post = std::atoi(port.c_str());
        if (Post == (*it)->_port) {
            std::cout << "Server matched" << std::endl;
            std::vector<Location>::iterator location = (*it)->getLocation(url);
            if (location == (*it)->_locations->end())
                throw 404;
            if (location->_upload_path.empty())
                return false;
            else
                check_upload_path(location->_upload_path);
            std::cout << "upload path : " << location->_upload_path << std::endl;
            return true;
        }
    }
    else
        throw 400;
    return true;
}