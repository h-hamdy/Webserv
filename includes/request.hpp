# pragma once

# include "webserv.hpp"

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


		size_t size;
		bool find_size;
		std::string body;
		int _EOF;
		bool create_file;

		ParseRequest () {
			_EOF = 3;
			size = 1;
			find_size = true;
			create_file = true;
		}
		// ~ParseRequest ();

		std::string ParseHttpRequest(std::string request, ssize_t &bytesRead);

		void ParseStartLine (const std::string& StartLine);
		void ParseMethod (std::string& method);
		void ParseBody (const std::string& _body, ssize_t byteRead);

		void	requestStatusCode ();
		void	ParseChunked (std::string _body, ssize_t byteRead);
		std::map<std::string, std::string> parseQueryString(const std::string& queryString);
		int		get_size (std::string &body, ssize_t &byteRead);

};