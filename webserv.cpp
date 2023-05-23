# include "webserv.hpp"


void	throw_error(std::string msg)
{
	std::cerr << msg << std::endl;
	std::exit(1);
}

int main () {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw_error("Error while trying to create socket.");
	else
		std::cout << "Creating socket succsefully!" << std::endl;
	sockaddr_in address;

	address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = inet_addr("10.53.30.22");

	if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0)
		throw_error("Error while trying to bind socket.");
	else
		std::cout << "Binding done well" << std::endl;

	if (listen(sockfd, 1) != 0)
		throw_error("Error while trying to listen for connections.");
	
	int new_socket;
	std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n" << std::endl;
	socklen_t size = sizeof(address);
	if ((new_socket = accept(sockfd, (struct sockaddr *)&address, &size)) < 0)
		throw_error("Error accepting!");
	else
		std::cout << "Accepting done!" << std::endl;
	
	char buffer[30000] = {0};
	int valread = read(new_socket, buffer, 1024);
	if (valread < 0) {
		std::cerr << "error read" << std::endl;
		close (sockfd);
		close (new_socket);
		std::exit(1);
	}

	std::cout << "read success ->" << buffer << std::endl;
	close(new_socket);
	return (0);
}