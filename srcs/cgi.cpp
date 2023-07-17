# include "webserv.hpp"
# include "Response.hpp"

void CgiPost(char *args[3], Response response) {
	std::string file = upload_path + "/" + file;
	int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		throw std::runtime_error("open error");

	int status;
	pid_t pid = fork();
	if (pid == 0) {
		dup2(fd, 1);
		if (execve(args[0], args, NULL) == -1)
			throw std::runtime_error("execve error");
	}
	else if (pid > 0) {
		waitpid(pid, &status, 0);
		close(fd);
	}
	else
		throw std::runtime_error("fork error");
	
	response.setResponse("HTTP/1.1", "200", "OK", "");
}

void CgiGet(char *args[3]) {
	int fd = open("Cookie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		throw std::runtime_error("open error");
	
	int status;
	pid_t pid = fork();
	if (pid == 0) {
		dup2(fd, 1);
		if (execve(args[0], args, NULL) == -1)
			throw std::runtime_error("execve error");
	}
	else if (pid > 0) {
		waitpid(pid, &status, 0);
		close(fd);
	}
	else
		throw std::runtime_error("fork error");
	
}

void setEnv(Response response) {
	std::vector<std::string> env;

	env.push_back("SCRIPT_NAME=");
	env.push_back("SCRIPT_FILENAME=");
	env.push_back("PATH_INFO=");
	env.push_back("QUERY_STRING=");
	env.push_back("HTTP_HOST=");
	env.push_back("HTTP_PORT=");
	env.push_back("HTTP_USER_AGENT=");
	env.push_back("HTTP_COOKIE=");
	env.push_back("REDIRECT_STATUS=");
	env.push_back("REMOTE_ADDR=");
	if (post) {
		env.push_back("CONTENT_TYPE=");
		env.push_back("CONTENT_LENGTH=");
	}

	response.setEnv(env);
}

void CgiProcess() {
	char *args[3];
	Response response;

	setEnv(response);

	if (extension == ".py") {
		args[0] = strdup("/usr/bin/python3");
		args[1] = strdup(path.c_str());
		args[2] = NULL;
	}
	else if (extension == ".php") {
		args[0] = strdup("cgi/php-cgi");
		args[1] = strdup(path.c_str());
		args[2] = NULL;
	}

	if (POST)
		CgiPost(args);
	else if (GET)
		CgiGet(args, response);
}
