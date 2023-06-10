/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 14:26:21 by omanar            #+#    #+#             */
/*   Updated: 2023/06/10 16:16:35 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	isNumber(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

Location	ParseLocation(std::ifstream &configFile, std::istringstream &is) {
	std::string line;
	Location location;

	is >> location._url;
	while (getline(configFile, line)) {
		if (line == "    }")
			return location;
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;
		if (keyword.empty() || keyword[0] == '#')
			continue;
		else if (keyword == "methods") {
			std::string method;
			while (iss >> method)
				location._methods.push_back(method);
		}
		else if (keyword == "root")
			iss >> location._root;
		else if (keyword == "autoindex")
			iss >> location._autoindex;
		else if (keyword == "index") {
			std::string index;
			while (iss >> index)
				location._index.push_back(index);
		}
		else if (keyword == "upload_path")
			iss >> location._upload_path;
		else if (keyword == "cgi_pass")
			iss >> location._cgi_pass;
		else if (keyword == "cgi_extension")
			iss >> location._cgi_extension;
		else if (keyword == "directory_listing")
			iss >> location._directory_listing;
		else
			throw std::runtime_error("Error: Unknown location keyword: " + keyword);
	}
	throw std::runtime_error("Error: Location block not closed");
}

void	ParseErrorPage(std::ifstream &configFile, Config *config) {
	std::string line;
	while (getline(configFile, line)) {
		if (line == "    }")
			return ;
		std::vector<std::pair<int, std::string> > errors;
		std::istringstream iss(line);
		std::string token;

		while (iss >> token) {
			if (token.empty() || token == "#")
				break;
			if (isNumber(token))
				errors.push_back(std::make_pair(atoi(token.c_str()), ""));
			else {
				std::vector<std::pair<int, std::string> >::iterator it = errors.begin();
				for (; it != errors.end(); it++)
					config->errorPages[it->first] = token;
				break;
			}
		}
	}
	throw std::runtime_error("Error: error_pages block not closed");
}

Config*	ParseServer(std::ifstream &configFile) {
	Config *config = new Config();
	std::string line;
	while (getline(configFile, line)) {
		if (line == "}")
			return config;
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;

		if (keyword.empty() || keyword[0] == '#')
			continue;
		if (keyword == "server_name")
			iss >> config->_server_name;
		else if (keyword == "host")
			iss >> config->_host;
		else if (keyword == "port")
			iss >> config->_port;
		else if (keyword == "max_body_size")
			iss >> config->_max_body_size;
		else if (keyword == "error_pages")
			ParseErrorPage(configFile, config);
		else if (keyword == "location")
			config->_locations->push_back(ParseLocation(configFile, iss));
		else
			throw std::runtime_error("Error: Unknown keyword: " + keyword);
	}
	throw std::runtime_error("Error: server block not closed");
}

Config*	getNextConfig(std::ifstream &configFile) {
	std::string line;

	while (getline(configFile, line)) {
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;
		if (keyword.empty() || keyword[0] == '#')
			continue;
		else if (keyword == "server")
			return ParseServer(configFile);
		else
			throw std::runtime_error("Error: Unknown keyword: " + keyword);
	}
	return NULL;
}

Server*	getNextServer(std::ifstream &configFile) {
	Config *config = getNextConfig(configFile);
	if (!config)
		return NULL;
	return new Server(config);
}
