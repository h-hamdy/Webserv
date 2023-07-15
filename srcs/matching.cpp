# include "../includes/webserv.hpp"

void	Server::matching (const std::string &host, const std::string &port, std::string url) {
    std::vector<Config *>::iterator it = configs.begin();
    if (host == (*it)->_host || host == (*it)->_server_name) {
        int Post = std::atoi(port.c_str());
        if (Post == (*it)->_port) {
            std::cout << "Server matched" << std::endl;
            std::vector<Location>::iterator location = (*it)->getLocation(url);
            if (location == (*it)->_locations->end())
                throw 404;
            std::cout << "Location matched : " << location->_url << std::endl;
            return ;
        }
    }
    else
        throw 400;
}