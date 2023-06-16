/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 14:23:17 by omanar            #+#    #+#             */
/*   Updated: 2023/06/16 15:12:26 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Server::Server() {}

Server::~Server() {
	for (std::vector<Config *>::iterator it = this->configs.begin(); it != this->configs.end(); ++it)
		delete *it;
}

void	Server::addConfig(Config *config) {
	this->configs.push_back(config);
}

void	Server::printConfigs() {
	std::cout << "<---------------- Server ---------------->" << std::endl;
	for (std::vector<Config *>::iterator it = this->configs.begin(); it != this->configs.end(); ++it) {
		std::cout << "Config {" << std::endl;
		std::cout << "   port: " << (*it)->_port << std::endl;
		std::cout << "   max_body_size: " << (*it)->_max_body_size << std::endl;
		std::cout << "   server_name: " << (*it)->_server_name << std::endl;
		std::cout << "   host: " << (*it)->_host << std::endl;
		std::cout << "   error_page: " << std::endl;
		for (std::map<int, std::string>::iterator it2 = (*it)->_error_pages.begin(); it2 != (*it)->_error_pages.end(); ++it2) {
			std::cout << "      " << it2->first << ": " << it2->second << std::endl;
		}
		std::cout << "   locations:" << std::endl;
		for (std::vector<Location>::iterator it2 = (*it)->_locations->begin(); it2 != (*it)->_locations->end(); ++it2) {
			std::cout << "   Location {" << std::endl;
			std::cout << "      url: " << it2->_url << std::endl;
			std::cout << "      root: " << it2->_root << std::endl;
			std::cout << "      redirect: " << it2->_redirect << std::endl;
			std::cout << "      upload_path: " << it2->_upload_path << std::endl;
			std::cout << "      cgi_pass: " << it2->_cgi_pass << std::endl;
			std::cout << "      cgi_extension: " << it2->_cgi_extension << std::endl;
			std::cout << "      directory_listing: " << it2->_directory_listing << std::endl;
			std::cout << "      methods: ";
			for (std::vector<std::string>::iterator it3 = it2->_methods.begin(); it3 != it2->_methods.end(); ++it3) {
				std::cout << *it3 << " ";
			}
			std::cout << "\n   }" << std::endl;
		}
		std::cout << "}" << std::endl;
	}
	std::cout << "<---------------- End Server ---------------->" << std::endl;
}
