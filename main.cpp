#include "webserv.hpp"

int	main(int ac, char **av)
{
	try {
		signal(SIGPIPE, SIG_IGN);
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv [configuration file]");
		// std::vector<Server *> servers = getServers(av[1]);
		Socket s(av[1]);
		s.setupServer();


		// std::vector<Server *>::iterator it = servers.begin();
		// for (; it != servers.end(); it++) {
		// 	(*it)->printConfigs();
		// 	delete *it;
		// }

	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
