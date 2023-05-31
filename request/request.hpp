# pragma once

# include <iostream>
# include <map>
# include <string>
# include <sstream>
# include <cstring>

struct RequestLine {
	std::string method;
	std::string url;
	std::string http_v;
};

class ParseRequest {
	private:
		RequestLine requestLine;
		std::map<std::string, std::string> header;
		std::string body;
	public:
		// ParseRequest ();
		// ~ParseRequest ();

		void ParseHttpRequest(const std::string& request);

		void ParseStartLine (const std::string& StartLine);
		void ParseMethod (std::string& method);
		void ParseBody (const std::string& body);

};