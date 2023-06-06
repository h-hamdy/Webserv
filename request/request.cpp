# include "request.hpp"

void	ParseRequest::ParseStartLine (const std::string& StartLine) {
	std::istringstream iss(StartLine);

	iss >> requestLine.method >> requestLine.url >> requestLine.http_v;
}

void ParseRequest::ParseMethod (std::string& method) {
	std::istringstream iss(method);
    std::string line;

    while (std::getline(iss, line)) {
		if (line == "\n" || line == "\r")
			continue;
		size_t space = line.find(" ");
		header.insert(std::make_pair(line.substr(0, space), line.substr(space + 1)));
    }
}

void	ParseChunked (const std::string& _body, std::string &body)
{
	
}

void ParseRequest::ParseBody (const std::string& _body) {
	if (_body.empty())
		std::cout << "No body found in the request." << std::endl;
	// std::map<std::string, std::string>::iterator it = header.find("Content-Length");
	// if (it != header.end())
	else
		this->body = _body;
	// else if (it == header.end())
	// 	it = header.find("Transfer-Encoding");
	// if (it != header.end())
	// 	this->body = _body;
	// 	ParseChunked(_body, this->body);
	// here throw an exeption (error code)
}

void ParseRequest::ParseHttpRequest(const std::string& request) {
	std::string line;
	size_t startline = request.find("\r\n");
	if (startline != std::string::npos) {
		line = request.substr(0, startline);
		ParseStartLine(line);
	}
	size_t headers = request.find("\r\n\r\n");
	if (headers != std::string::npos) {
		line = request.substr(startline + 2, headers);
		std::cout << line << std::endl;
		ParseMethod(line);
	}
	line = request.substr(headers + 4);
	// std::cout << line << std::endl;
	ParseBody(line);

	// std::cout << "_______________START_LINE____________" << std::endl;
	// std::cout << "METHOD = " << requestLine.method << "       ";
	// std::cout << "URL = " << requestLine.url << "       ";
	// std::cout << "VERSION = " << requestLine.http_v << std::endl << std::endl;



	// std::cout << "_______________HEADERS____________" << std::endl << std::endl;
	// std::map<std::string, std::string>::iterator iter = header.begin();
    // for (; iter != header.end(); iter++) {
    //     std::cout << "KEY = " << iter->first << "     " << "VALUE = " << iter->second << std::endl;
    // }

	// std::cout << "_______________BODY____________" << std::endl << std::endl;
	// if (!body.empty())
	// 	std::cout << body << std::endl;
	// 	// std::cout << "Body exist" << std::endl;
	// else
	// 	std::cout << "No bodya found in the request." << std::endl;
}