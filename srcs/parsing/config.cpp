/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 19:19:39 by omanar            #+#    #+#             */
/*   Updated: 2023/07/20 17:39:37 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

Location::Location() {
	this->_url = "";
	this->_root = "";
	this->_index = "";
	this->_redirect = "";
	this->_upload_path = "";
	this->_directory_listing = false;
	this->_methods[0] = false;
	this->_methods[1] = false;
	this->_methods[2] = false;
}

Config::Config() {
	this->_port = -1;
	this->_max_body_size = -1;
	this->_server_name = "";
	this->_host = "";
	this->_locations = new std::vector<Location>;
}

Config::~Config() {
	delete this->_locations;
}

std::vector<Location>::iterator Config::getLocation(std::string const &url) {
	std::string tmp = url;
	while (!tmp.empty())
	{
		for (std::vector<Location>::iterator it = this->_locations->begin(); it != this->_locations->end(); it++)
			if (it->_url == tmp)
				return it;
		size_t pos = tmp.find_last_of('/');
		tmp = tmp.substr(0, pos);
	}
	for (std::vector<Location>::iterator it = this->_locations->begin(); it != this->_locations->end(); it++)
		if (it->_url == "/")
			return it;
	return this->_locations->end();
}

bool Config::getAllowed(std::vector<Location>::iterator location) {
	return location->_methods[0];
}

bool Config::postAllowed(std::vector<Location>::iterator location) {
	return location->_methods[1];
}

bool Config::deleteAllowed(std::vector<Location>::iterator location) {
	return location->_methods[2];
}
