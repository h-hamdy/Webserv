#include "webserv.hpp"

int	main(int ac, char **av)
{
	try {
		Socket s;
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv [configuration file]");
		std::vector<Server *> servers = getServers(av[1]);
		
		std::vector<Server *>::iterator it = servers.begin();
		while (it != servers.end()) {
			
			(*it)->printServerConfig();
			delete *it++;
		}

		// s.setupServer(,"0.0.0.0");
		// s.~Socket();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
