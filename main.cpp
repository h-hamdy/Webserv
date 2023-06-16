#include "webserv.hpp"

int	main(int ac, char **av)
{
	try {
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv [configuration file]");
		Socket s(av[1]);
		s.setupServer();
		// std::vector<Server *> servers = getServers(av[1]);
		// std::cout << "Number of servers: " << servers.size() << std::endl;
		// s.setupServer(,"0.0.0.0");
		// s.~Socket();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
