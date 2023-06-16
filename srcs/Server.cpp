/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 14:23:17 by omanar            #+#    #+#             */
/*   Updated: 2023/06/10 19:05:53 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Server::Server(Config *config) {
	this->config = config;
	_ServerSocket = 0;
    _ClientSocket = 0;
    _maxFd = 0;
    _nclients = 0;
    _bytesRead = 0;
    FD_ZERO(&_read_set);
    FD_ZERO(&_write_set);
    _ClientAddressSize = sizeof(_ClientAddress);
    memset(&_ServerAddress,0,sizeof(_ServerAddress));
    memset(&_ClientAddress,0,sizeof(_ClientAddress));
}

Server::~Server() {
	delete this->config;
	for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++) {
		close(it->fd);
	}
}

void Server::printServerConfig() {
	std::cout << "--------------- Server ---------------" << std::endl;
    std::cout << "  Server Name: " << this->config->_server_name << std::endl;
	std::cout << "  Host: " << this->config->_host << std::endl;
	std::cout << "  Port: " << this->config->_port << std::endl;
	std::cout << "  Max Body Size: " << this->config->_max_body_size << std::endl;

	std::map<int, std::string>::iterator mit = this->config->errorPages.begin();
	while (mit != this->config->errorPages.end()) {
		std::cout << "  Error " << mit->first << " : " << mit->second << std::endl;
		mit++;
	}

	std::vector<Location>::iterator it = this->config->_locations->begin();
	while (it != this->config->_locations->end()) {
		std::cout << "  Location: " << it->_url << " {" << std::endl;
		std::cout << "      Root: " << it->_root << std::endl;
		std::cout << "      Upload Path: " << it->_upload_path << std::endl;
		std::cout << "      Directory Listing: " << it->_directory_listing << std::endl;
		std::cout << "      CGI Pass: " << it->_cgi_pass << std::endl;
		std::cout << "      CGI Extension: " << it->_cgi_extension << std::endl;
		std::cout << "      Methods: ";
		std::vector<std::string>::iterator it2 = it->_methods.begin();
		while (it2 != it->_methods.end()) {
			std::cout << *it2 << " ";
			it2++;
		}
		std::cout << std::endl << "  }" << std::endl;
		it++;
	}
	std::cout << "--------------------------------------" << std::endl;
}
