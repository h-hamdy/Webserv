# pragma once

# include "webserv.hpp"

struct RequestLine {
	std::string method;
	std::string url;
	std::string http_v;
};

class ParseRequest {
	private:
	public:
		RequestLine requestLine;
		std::map<std::string, std::string> header;
		std::fstream file;
		size_t size;
		bool find_size;
		std::string body;
		int _bread;
		int _EOF;
		ParseRequest () {
			_EOF = 3;
			size = 1;
			_bread = 0;
			find_size = true;
		}
		// ~ParseRequest ();

		void ParseHttpRequest(std::string request, ssize_t bytesRead);

		void ParseStartLine (const std::string& StartLine);
		void ParseMethod (std::string& method);
		void ParseBody (const std::string& _body, ssize_t byteRead);

		void	requestStatusCode ();
		void	ParseChunked (std::string _body, ssize_t byteRead);

};