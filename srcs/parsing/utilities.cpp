/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 14:26:21 by omanar            #+#    #+#             */
/*   Updated: 2023/07/07 19:08:13 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	isNumber(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

void	parseSingle(std::string const &line, std::string &variable) {
	std::string token;
	size_t pos = line.find(';');
	if (pos == std::string::npos)
		throw std::runtime_error("Error: missing ';'");
	std::string value = line.substr(0, pos);
	std::istringstream iss(value);
	iss >> token;
	iss >> variable;
	if (variable.empty())
		throw std::runtime_error("Error: missing value");
	if ((iss >> token))
		throw std::runtime_error("Error: syntax error: '" + token + "'");
}

void	parseMethods(std::string &line, bool *methods) {
	size_t pos = line.find(';');
	if (pos == std::string::npos)
		throw std::runtime_error("Error: missing ';'");
	line.erase(pos);
	std::string token;
	std::istringstream iss(line);
	iss >> token;
	while (iss >> token) {
		if (token == "GET")
			methods[0] = true;
		else if (token == "POST")
			methods[1] = true;
		else if (token == "DELETE")
			methods[2] = true;
		else
			throw std::runtime_error("Error: Unknown method: " + token);
	}
}

void	parseExtensions(std::string &line, std::vector<std::string> &extensions) {
	size_t pos = line.find(';');
	if (pos == std::string::npos)
		throw std::runtime_error("Error: missing ';'");
	line.erase(pos);
	std::string token;
	std::istringstream iss(line);
	iss >> token;
	while (iss >> token) {
		if (token[0] != '.')
			throw std::runtime_error("Error: extension must start with '.'");
		extensions.push_back(token);
	}
}

Location	ParseLocation(std::ifstream &configFile, std::istringstream &is) {
	std::string line;
	Location location;

	is >> location._url;
	while (getline(configFile, line)) {
		std::istringstream iss(line);
		std::string directive;
		iss >> directive;
		if (directive == "}") {
			if ((iss >> directive) && directive[0] != '#')
				throw std::runtime_error("Error: location syntax error: '" + directive + "'");
			return location;
		}
		if (directive.empty() || directive[0] == '#')
			continue;
		else if (directive == "methods")
			parseMethods(line, location._methods);
		else if (directive == "redirect")
			parseSingle(line, location._redirect);
		else if (directive == "root")
			parseSingle(line, location._root);
		else if (directive == "upload_path")
			parseSingle(line, location._upload_path);
		else if (directive == "directory_listing") {
			parseSingle(line, directive);
			if (directive == "on")
				location._directory_listing = true;
			else if (directive == "off")
				location._directory_listing = false;
			else
				throw std::runtime_error("Error: Unknown directory_listing directive: " + directive);
		}
		else if (directive == "cgi_extensions")
			parseExtensions(line, location._cgi_extensions);
		else
			throw std::runtime_error("Error: Unknown location directive: " + directive);
	}
	throw std::runtime_error("Error: Location block not closed");
}

void errorPagesLineChecker(std::string &line) {
	std::string token;
	std::istringstream iss(line);
	iss >> token;
	if (token.empty() || token[0] == '#')
		return;
	if (token == "}") {
		if ((iss >> token) && token[0] != '#')
			throw std::runtime_error("Error: error_pages syntax error: '" + token + "'");
		return;
	}
	size_t pos = line.find(';');
	if (pos == std::string::npos)
		throw std::runtime_error("Error: missing ';'");
	line = line.substr(0, pos);
	if (isNumber(token))
		return;
	throw std::runtime_error("Error: error_pages syntax error: '" + token + "'");
}

void	ParseErrorPage(std::ifstream &configFile, Config *config) {
	std::string line;
	while (getline(configFile, line)) {
		errorPagesLineChecker(line);
		std::vector<int> errors;
		std::istringstream iss(line);
		std::string token;
		int number;

		while (iss >> token) {
			if (token == "}") {
				if ((iss >> token) && token[0] != '#')
					throw std::runtime_error("Error: server syntax error: '" + token + "'");
				return;
			}
			if (token.empty() || token == "#")
				break;
			if (isNumber(token)) {
				number = atoi(token.c_str());
				if (number < 100 || number > 599)
					throw std::runtime_error("Error: error_pages: invalid status code: " + token);
				errors.push_back(number);
			}
			else {
				if (iss >> token)
					throw std::runtime_error("Error: error_pages syntax error: '" + token + "'");
				std::vector<int>::iterator it = errors.begin();
				for (; it != errors.end(); it++)
					config->_error_pages[*it] = token;
				break;
			}
		}
	}
	throw std::runtime_error("Error: error_pages block not closed");
}

void hostChecker(std::string const &host) {
	if (host[0] == '.' || host[host.length() - 1] == '.')
		throw std::runtime_error("Error: Invalid host");
	std::vector<std::string> tokens;
	std::istringstream iss(host);
	std::string token;
	while (getline(iss, token, '.')) {
		if (token.empty())
			throw std::runtime_error("Error: Invalid host");
		if (!isNumber(token) || atoi(token.c_str()) > 255)
			throw std::runtime_error("Error: Invalid host");
		tokens.push_back(token);
	}
	if (tokens.size() != 4)
		throw std::runtime_error("Error: Invalid host");
}

Config*	ParseServer(std::ifstream &configFile) {
	Config *config = new Config();
	std::string line;
	while (getline(configFile, line)) {
		std::istringstream iss(line);
		std::string directive;
		iss >> directive;

		if (directive == "}") {
			if ((iss >> directive) && directive[0] != '#')
				throw std::runtime_error("Error: server syntax error: '" + directive + "'");
			return config;
		}
		if (directive.empty() || directive[0] == '#')
			continue;
		if (directive == "server_name")
			parseSingle(line, config->_server_name);
		else if (directive == "host") {
			parseSingle(line, config->_host);
			hostChecker(config->_host);
		}
		else if (directive == "port") {
			std::string port;
			parseSingle(line, port);
			if (isNumber(port)) {
				config->_port = atoi(port.c_str());
				if (config->_port < 0 || config->_port > 65535)
					throw std::runtime_error("Error: port number out of range");
			}
			else
				throw std::runtime_error("Error: Invalid port number");
		}
		else if (directive == "max_body_size") {
			std::string size;
			iss >> size;
			parseSingle(line, size);
			if (isNumber(size))
				config->_max_body_size = atoi(size.c_str());
			else
				throw std::runtime_error("Error: Invalid client_max_body_size value");
		}
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
		else if (directive == "server") {
			iss >> directive;
			if (directive != "{" || ((iss >> directive) && directive[0] != '#'))
				throw std::runtime_error("Error: server syntax error: '" + directive + "'");
			return ParseServer(configFile);
		}
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
	}
}

Server*	findPort(std::vector<Server *> &servers, std::string host, int port) {
	for (std::vector<Server*>::iterator it = servers.begin(); it != servers.end(); ++it) {
		if ((*it)->configs[0]->_host == host && (*it)->configs[0]->_port == port)
			return *it;
	}
	return nullptr;
}

std::vector<Server *>	getServers(char *file) {
	std::ifstream	configFile(file);
	if (!configFile.is_open())
		throw std::runtime_error("Error: Failed to open configuration file: " + std::string(file));

	std::vector<Server *> servers;

	Config *config = getNextConfig(configFile);
	while (config) {
		missing(config);
		Server *found = findPort(servers, config->_host, config->_port);
		if (found != nullptr)
			found->configs.push_back(config);
		else {
			Server *server = new Server();
			server->addConfig(config);
			servers.push_back(server);
		}
		config = getNextConfig(configFile);
	}

	configFile.close();
	return servers;
}
