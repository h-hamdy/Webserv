# include "webserv.hpp"
# include "response.hpp"

void setEnv(Response &response, std::string const &path, ParseRequest &request) {
	std::vector<std::string> env;

	env.push_back("SCRIPT_NAME=" + request.requestLine.url);
	env.push_back("SCRIPT_FILENAME=" + path);
	env.push_back("PATH_INFO=" + path);
	env.push_back("QUERY_STRING=" + request.queryString);
	env.push_back("HTTP_HOST=" + request.requestLine.host);
	env.push_back("HTTP_PORT=" + request.requestLine.port);
	env.push_back("HTTP_USER_AGENT=" + request.header["User-Agent"]);
	env.push_back("HTTP_COOKIE=" + request.header["Cookie"]);
	env.push_back("REDIRECT_STATUS=200");
	env.push_back("REQUEST_METHOD=" + request.requestLine.method);
	env.push_back("REMOTE_ADDR=" + request.requestLine.host);
	if (request.requestLine.method == "POST") {
		env.push_back("CONTENT_TYPE=" + request.header["Content-Type"]);
		env.push_back("CONTENT_LENGTH=" + request.header["Content-Length"]);
	}

	response.setEnv(env);
}

void Cgi(char *args[3], Response &response, ParseRequest &req, std::string filePath) {
	int file;
	if (req.requestLine.method == "POST")
		file = open(filePath.c_str(), O_RDONLY);
	else
		file = 0;
	int fd = open("tmp", O_RDWR | O_CREAT | O_TRUNC, 0666);
		if (fd == -1)
			throw std::runtime_error("open error");
	req.pid = fork();
	if (req.pid == 0) {
		dup2(file, 0);
		dup2(fd, 1);
		close(file);
		close(fd);
		if (execve(args[0], args, response.getEnv()) == -1) {
			std::cerr << "execve error" << std::endl;
			exit(1);
		}
		exit(0);
	}
	else if (req.pid == -1)
		throw std::runtime_error("fork error");
	close(fd);
}

std::string parseCgiBody(std::string const &body) {
	size_t pos = body.find("\r\n\r\n");
	if (pos != std::string::npos)
		return body.substr(pos + 4);
	return body;
}

void CgiProcess(Server &server, int j, std::string const &path, std::string const &extension, std::string filePath) {
	char *args[3];
	ParseRequest &req = server._requests[server._pollfds[j].fd];
	Response &res = server._responses[server._pollfds[j].fd];
	if (req.cgiFlag == 2) {
		res.setResponse("");
		res.close_connection = true;
		return;
	}
	try
	{
		if (!req.cgiFlag) {
			setEnv(res, path, req);
			if (extension == ".py") {
				args[0] = strdup("cgi-bin/python3");
				args[1] = strdup(path.c_str());
				args[2] = NULL;
			}
			else if (extension == ".php") {
				args[0] = strdup("cgi-bin/php-cgi");
				args[1] = strdup(path.c_str());
				args[2] = NULL;
			}


			Cgi(args, res, req, filePath);
			free(args[0]);
			free(args[1]);
			req.cgiFlag = 1;
		}
		else {
			// res.setResponse("");
			std::cout << "path in cgi " << path << " flag " << req.cgiFlag << std::endl;
			int check = waitpid(req.pid, &req.status, WNOHANG);
			if (check == 0){
				res.setResponse("");
				return ;
			}
			if (WIFEXITED(req.status)) {
				std::cout << "wlh ta dkhaal" << std::endl;
				std::string body;
				int byteRead;
				char buf[1024];
				int fd = open("tmp", O_RDONLY);
				if (fd == -1)
					throw std::runtime_error("open error");
				while ((byteRead = read(fd, buf, 1024)) > 0) {
					buf[byteRead] = '\0';
					body += buf;
				}
				std::cout << "body ====================== " << body << std::endl;
				res.setResponse(parseCgiBody(body));
				// exit(0);
				close(fd);
				unlink("tmp");
				remove("tmp");
				req.cgiFlag = 2;
			}
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		res.setStatusCode("500");
	}
}
