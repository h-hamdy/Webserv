#include "webserv.hpp"

int	main(int ac, char **av) {
	try {
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv [configuration file]");

		std::ifstream	configFile(av[1]);
		if (!configFile.is_open())
			throw std::runtime_error("Error: Failed to open configuration file: " + std::string(av[1]));

		Server *server = getNextServer(configFile);
		while (server) {
			// server->run();
			// std::cout << "--------------- Server ---------------" << std::endl;
			// server->printServerConfig();
			server = getNextServer(configFile);
		}

		configFile.close();

        Socket s;
        s.setupServer(8080,"0.0.0.0");
        s.~Socket();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}