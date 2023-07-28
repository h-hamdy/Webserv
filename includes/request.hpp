# pragma once

# include "webserv.hpp"

class Server;

struct RequestLine {
	std::string method;
	std::string host;
	std::string port;
	std::string url;
	std::string http_v;
};

class ParseRequest {
	private:
	public:
		RequestLine requestLine;
		std::map<std::string, std::string> header;
		std::fstream file;
		std::string queryString;


		std::string body;
		std::string path;
		std::string filePath;
		std::string tmpFile;
		std::string rest;
		int _EOF;
		int cgiFlag;
		int fd;
		int status;
		bool flag;
		bool find_size;
		bool create_file;
		size_t size;
		pid_t pid;

		ParseRequest () {
			find_size = true;
			create_file = true;
			flag = false;
			_EOF = 3;
			size = 1;
			status = 0;
			cgiFlag = 0;
			pid = 0;
			fd = -1;
		}

		std::string 						ParseHttpRequest(std::string request, ssize_t bytesRead, Server &server, int j);

		void								ParseStartLine (const std::string& StartLine);
		void								ParseMethod (std::string& method);
		void								ParseBody (const std::string& _body, Server &server, int j);

		void								requestStatusCode ();
		void								ParseChunked (std::string _body, Server &server, int j);
		std::map<std::string, std::string>	parseQueryString(const std::string& queryString);
		int									get_size (std::string &body, Server &server, int j);

};