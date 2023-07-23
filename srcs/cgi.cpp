# include "webserv.hpp"
# include "response.hpp"
# 

void CgiPost(char *args[3], Response &response) {
	int fd[2];
	if (pipe(fd))
		throw std::runtime_error("pipe error");

	int status;
	pid_t pid = fork();
	if (pid == 0) {
		dup2(fd[1], 1);
		if (execve(args[0], args, response.getEnv()) == -1) {
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
	// response->setResponse("HTTP/1.1", "200", "OK", body);
}

void CgiGet(char *args[3], Response &response) {
	(void)response;
	int fd[2];
	if (pipe(fd))
		throw std::runtime_error("pipe error");

	int status;
	pid_t pid = fork();
	if (pid == 0) {
		dup2(fd[1], 1);
		if (execve(args[0], args, NULL) == -1) {
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
	// response.setResponse("HTTP/1.1", "200", "OK", body);
}

void setEnv(Response &response, std::string const &path, ParseRequest &request) {
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

	response.setEnv(env);
}

void CgiProcess(Server &server, int j, std::string const &path, std::string const &extension) {
	char *args[3];
	ParseRequest &req = server._requests[server._pollfds[j].fd];
	Response &res = server._responses[server._pollfds[j].fd];
	setEnv(res, path, req);

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

	if (req.requestLine.method == "POST")
		CgiPost(args, res);
	else if (req.requestLine.method == "GET")
		CgiGet(args, res);
}
