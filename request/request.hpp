# pragma once

# include <iostream>
# include <map>
# include <string>
# include <sstream>
# include <cstring>
# include <exception>
# include <fstream>

struct RequestLine {
	std::string method;
	std::string url;
	std::string http_v;
};

class ParseRequest {
	private:
		RequestLine requestLine;
		std::map<std::string, std::string> header;
		std::fstream file;
	public:
		// ParseRequest ();
		// ~ParseRequest ();

		void ParseHttpRequest(std::string request);

		void ParseStartLine (const std::string& StartLine);
		void ParseMethod (std::string& method);
		void ParseBody (const std::string& body);

		void	requestStatusCode ();
		void	ParseChunked (std::string _body);

};