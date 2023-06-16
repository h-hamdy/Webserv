#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/socket.h>
# include <iostream>
# include <string>
# include <cstdlib>
# include <unistd.h>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <stdexcept>
# include <netinet/in.h>
# include <arpa/inet.h>
# include "config.hpp"
# include "socket.hpp"
# include "web-client.hpp"

// class Tcpserver {
// 	public:
// 		// Tcpserver ();
// 		// ~Tcpserver ();
// };


class Server {
	private:
	public:
		std::vector<Config *>	configs;
		Server();
		~Server();
		void	addConfig(Config *config);
		void	printConfigs();
};

Config*	getNextConfig(std::ifstream &configFile);
std::vector<Server *>	getServers(char *file);

# endif
