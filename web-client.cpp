# include "web-client.hpp"

void	throw_error(std::string msg)
{
	std::cerr << msg << std::endl;
	std::exit(1);
}

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in address;
	std::string req = "GET / HTTP/1.1\r\n\r\n";

	if (sockfd < 0)
		throw_error("Error while trying to open socket!");
	else
		std::cout << "Creating socket succsefully!" << std::endl;

	address.sin_family = AF_INET;
    address.sin_port = htons(8080);

	socklen_t size = sizeof(address);
	if (connect(sockfd, (struct sockaddr *)&address, size) != 0)
		throw_error("Cannot connect to the server!");
	else
		std::cout << "Connection done!" << std::endl;
	
	if (write(sockfd, req.c_str(), req.length()) != req.length())
		throw_error("Write error!");
	
	char buffer[30000] = {0};
	int n = 0;
	for (; n == read(sockfd, buffer, 1024) > 0;)
		std::cout << buffer;
	if (n < 0)
		throw_error("Read error!");	
}

