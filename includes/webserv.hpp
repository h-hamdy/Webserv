#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/socket.h>
# include <iostream>
# include <string.h>
# include <cstdlib>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fstream>
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
		Config *config;
	public:
		Server(Config *config);
		~Server();
		void printServerConfig();
};

Server*	getNextServer(std::ifstream &configFile);
Config*	getNextConfig(std::ifstream &configFile);

# endif