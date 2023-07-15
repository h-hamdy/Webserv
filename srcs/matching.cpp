# include "../includes/webserv.hpp"

void	Server::matching (const std::string &host, const std::string &port) {
    (void)port;
    std::vector<Config *>::iterator it = configs.begin();
    for (; it != configs.end(); it++) {
        if (host == (*it)->_host || host == (*it)->_server_name) {
            int Post = std::atoi(port.c_str());
            if (Post == (*it)->_port) {
                std::cout << "Server matched" << std::endl;
                // call match location function
                return ;
            }
        }
    }
    if (it == configs.end())
        throw 400;
}