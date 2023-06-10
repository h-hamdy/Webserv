/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 14:26:21 by omanar            #+#    #+#             */
/*   Updated: 2023/06/10 19:05:12 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	isNumber(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

void	parseValue(std::istringstream &iss, std::string &variable) {
	std::string token;
	iss >> token;
	token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
	variable = token;
}

Location	ParseLocation(std::ifstream &configFile, std::istringstream &is) {
	std::string line;
	Location location;

	is >> location._url;
	while (getline(configFile, line)) {
		if (line == "    }")
			return location;
		std::istringstream iss(line);
		std::string directive;
		iss >> directive;
		if (directive.empty() || directive[0] == '#')
			continue;
		else if (directive == "methods") {
			std::string method;
			while (iss >> method) {
				method.erase(std::remove(method.begin(), method.end(), ';'), method.end());
				location._methods.push_back(method);
			}
		}
		else if (directive == "redirect")
			parseValue(iss, location._redirect);
		else if (directive == "root")
			parseValue(iss, location._root);
		else if (directive == "upload_path")
			parseValue(iss, location._upload_path);
		else if (directive == "directory_listing") {
			iss >> directive;
			directive.erase(std::remove(directive.begin(), directive.end(), ';'), directive.end());
			if (directive == "on")
				location._directory_listing = true;
			else if (directive == "off")
				location._directory_listing = false;
			else
				throw std::runtime_error("Error: Unknown directory_listing directive: " + directive);
		}
		else if (directive == "cgi_pass")
			parseValue(iss, location._cgi_pass);
		else if (directive == "cgi_extension")
			parseValue(iss, location._cgi_extension);
		else
			throw std::runtime_error("Error: Unknown location directive: " + directive);
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
				token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
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
		std::string directive;
		iss >> directive;

		if (directive.empty() || directive[0] == '#')
			continue;
		if (directive == "server_name")
			parseValue(iss, config->_server_name);
		else if (directive == "host")
			parseValue(iss, config->_host);
		else if (directive == "port")
			parseValue(iss, config->_port);
		else if (directive == "max_body_size")
			parseValue(iss, config->_max_body_size);
		else if (directive == "error_pages")
			ParseErrorPage(configFile, config);
		else if (directive == "location")
			config->_locations->push_back(ParseLocation(configFile, iss));
		else
			throw std::runtime_error("Error: Unknown directive: " + directive);
	}
	throw std::runtime_error("Error: server block not closed");
}

Config*	getNextConfig(std::ifstream &configFile) {
	std::string line;

	while (getline(configFile, line)) {
		std::istringstream iss(line);
		std::string directive;
		iss >> directive;
		if (directive.empty() || directive[0] == '#')
			continue;
		else if (directive == "server")
			return ParseServer(configFile);
		else
			throw std::runtime_error("Error: Unknown directive: " + directive);
	}
	return NULL;
}

void	missing(Config *config) {
	if (config->_server_name.empty())
		throw std::runtime_error("Error: Missing server_name directive");
	if (config->_host.empty())
		throw std::runtime_error("Error: Missing host directive");
	if (config->_port.empty())
		throw std::runtime_error("Error: Missing port directive");
	if (config->_max_body_size.empty())
		throw std::runtime_error("Error: Missing max_body_size directive");
	std::vector<Location>::iterator it = config->_locations->begin();
	for (; it != config->_locations->end(); it++) {
		if (it->_url.empty())
			throw std::runtime_error("Error: Missing location url directive");
		if (it->_root.empty())
			throw std::runtime_error("Error: Missing root directive");
		if (it->_redirect.empty())
			throw std::runtime_error("Error: Missing redirect directive");
		if (it->_upload_path.empty())
			throw std::runtime_error("Error: Missing upload_path directive");
		if (it->_cgi_pass.empty() && it->_cgi_extension.empty())
			throw std::runtime_error("Error: Missing cgi_pass or cgi_extension directive");
		if (it->_methods.empty())
			throw std::runtime_error("Error: Missing methods directive");
	}
}

Server*	getNextServer(std::ifstream &configFile) {
	Config *config = getNextConfig(configFile);
	if (!config)
		return NULL;
	missing(config);
	return new Server(config);
}

std::vector<Server *>	getServers(char *file) {
	std::ifstream	configFile(file);
	if (!configFile.is_open())
		throw std::runtime_error("Error: Failed to open configuration file: " + std::string(file));

	Server *server;
	std::vector<Server *> servers;
	while ((server = getNextServer(configFile)))
		servers.push_back(server);
	configFile.close();
	return servers;
}
