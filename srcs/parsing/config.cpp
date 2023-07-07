/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 19:19:39 by omanar            #+#    #+#             */
/*   Updated: 2023/07/07 15:55:56 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

Location::Location() {
	this->_url = "";
	this->_root = "";
	this->_redirect = "";
	this->_upload_path = "";
	this->_cgi_pass = "";
	this->_cgi_extension = "";
	this->_directory_listing = false;
}

Config::Config() {
	this->_port = 0;
	this->_max_body_size = 0;
	this->_server_name = "";
	this->_host = "";
	this->_locations = new std::vector<Location>;
}

Config::~Config() {
	delete this->_locations;
}

std::vector<Location>::iterator Config::getLocation(std::string url) {
	for (std::vector<Location>::iterator it = this->_locations->begin(); it != this->_locations->end(); it++) {
		if (it->_url == url)
			return (*it);
	}
	return (NULL);
}
