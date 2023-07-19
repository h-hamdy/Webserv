# include "webserv.hpp"
# include "response.hpp"

void CgiPost(char *args[3], Response *response, std::vector<Location>::iterator &location) {
	// std::string file = location->_root + "/output.html";
	// int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	(void)location;
	int fd[2];
	if (pipe(fd))
		throw std::runtime_error("pipe error");

	int status;
	pid_t pid = fork();
	if (pid == 0) {
		dup2(fd[1], 1);
		if (execve(args[0], args, response->getEnv()) == -1) {
			std::cerr << "execve error" << std::endl;
			exit(1);
		}
	}
	else if (pid > 0) {
		waitpid(pid, &status, WNOHANG);
		close(fd[1]);
	}
	else
		throw std::runtime_error("fork error");
	
	char buf[1024];
	std::string body;
	int byteRead;
	while ((byteRead = read(fd[0], buf, 1024)) > 0) {
		buf[byteRead] = '\0';
		body += buf;
	}
	response->setResponse("HTTP/1.1", "200", "OK", body);
}

// void CgiGet(char *args[3]) {
// 	// int fd = open("Cookie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 	// if (fd == -1)
// 	// 	throw std::runtime_error("open error");
	
// 	// int status;
// 	// pid_t pid = fork();
// 	// if (pid == 0) {
// 	// 	dup2(fd, 1);
// 	// 	if (execve(args[0], args, NULL) == -1)
// 	// 		throw std::runtime_error("execve error");
// 	// }
// 	// else if (pid > 0) {
// 	// 	waitpid(pid, &status, 0);
// 	// 	close(fd);
// 	// }
// 	// else
// 	// 	throw std::runtime_error("fork error");
	
// }

void setEnv(Response *response, std::string const &path, ParseRequest &request) {
	std::vector<std::string> env;

	env.push_back("SCRIPT_NAME=" + request.requestLine.url);
	env.push_back("SCRIPT_FILENAME=" + path);
	// env.push_back("PATH_INFO=");
	env.push_back("QUERY_STRING=" + request.queryString);
	env.push_back("HTTP_HOST=" + request.requestLine.host);
	env.push_back("HTTP_PORT=" + request.requestLine.port);
	env.push_back("HTTP_USER_AGENT=" + request.header["User-Agent"]);
	env.push_back("HTTP_COOKIE=");
	// env.push_back("REDIRECT_STATUS=");
	env.push_back("REMOTE_ADDR=");
	if (request.requestLine.method == "POST") {
		env.push_back("CONTENT_TYPE=" + request.header["Content-Type"]);
		env.push_back("CONTENT_LENGTH=" + request.header["Content-Length"]);
	}

	response->setEnv(env);
}

void CgiProcess(std::vector<Location>::iterator &location, std::string const &path, ParseRequest &request, std::string extension) {
	char *args[3];
	Response *response = new Response();

	setEnv(response, path, request);

	std::cout << "path in cgi " << path << std::endl;
	if (extension == ".py") {
		args[0] = strdup("/usr/bin/python3");
		args[1] = strdup(path.c_str());
		args[2] = NULL;
	}
	else if (extension == ".php") {
		args[0] = strdup("cgi-bin/php-cgi");
		args[1] = strdup(path.c_str());
		args[2] = NULL;
	}
	std::cout << "teest teeest ========" << std::endl;

	if (request.requestLine.method == "POST")
		CgiPost(args, response, location);
	// else if (method == "GET")
	// 	CgiGet(args, response);

	delete response;
}
