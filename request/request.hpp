# pragma once

# include <iostream>

enum Method {GET, POST, DELETE, UNKNOWN};

typedef struct RequestLine {
	Method 		method;
	std::string url;
	std::string http_v;
};

class ParseRequest {
	private:
		RequestLine requestLine;
		std::string body;
	public:
		ParseRequest ();
		~ParseRequest ();

		void ParseHttpRequest(const std::string& request) const;

};