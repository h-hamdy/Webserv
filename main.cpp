#include "webserv.hpp"

int	main(int ac, char **av)
{
	try {
		signal(SIGPIPE, SIG_IGN);
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv [configuration file]");
		Socket s(av[1]);
		s.setupServer();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
