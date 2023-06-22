# pragma once

# include "webserv.hpp"

struct RequestLine {
	std::string method;
	std::string url;
	std::string http_v;
};

class ParseRequest {
	private:
		RequestLine requestLine;
		std::map<std::string, std::string> header;
		int _EOF;
		int	_bread;
		std::fstream file;
	public:
		ParseRequest () {
			_EOF = 3;
		}
		// ~ParseRequest ();

		void ParseHttpRequest(std::string request, ssize_t bytesRead);

		void ParseStartLine (const std::string& StartLine);
		void ParseMethod (std::string& method);
		void ParseBody (const std::string& body, ssize_t byteRead);

		void	requestStatusCode ();
		void	ParseChunked (std::string _body, ssize_t byteRead, std::fstream &file);

};