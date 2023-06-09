# include "request.hpp"

void	ParseRequest::ParseStartLine (const std::string& StartLine) {
	std::istringstream iss(StartLine);

	iss >> requestLine.method >> requestLine.url >> requestLine.http_v;
}

void ParseRequest::ParseMethod (std::string& method) {
	std::istringstream iss(method);
    std::string line, key, value;
	int space, pos;
	std::cout << method << std::endl;
	std::cout << "----------------------------------" << std::endl;

    while (std::getline(iss, line)) {
		if (line == "\n" || line == "\r")
			continue;
		space = line.find(" ");
		pos = line.find("\r\n");
		key = line.substr(0, space -1);
		value =  line.substr((space + 1), pos);
		if (value[value.length() -1] == '\r')
			value.pop_back();
		header.insert(std::make_pair(key, value));
    }
}

std::string get_hex (const std::string& _body)
{
	size_t pos = _body.find("\r\n");
	std::string hex = _body.substr(0, pos);
	return (hex);
}

int	convert_hex (std::string hex)
{
	return (std::stoi(hex, nullptr, 16));
}

std::string generateRandomString(int length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int numCharacters = characters.length();

    std::string randomString;
    for (int i = 0; i < length; ++i) {
        int randomIndex = rand() % numCharacters;
        randomString += characters[randomIndex];
    }
    return randomString;
}

std::string get_ContentType (std::string value) 
{
	size_t pos = value.find_last_of("/");
	std::string type;
	std::string rand;
	if (pos != std::string::npos) {
		type = value.substr(pos + 1);
		rand = generateRandomString(10);
		return (rand + "." + type);
	}
	throw 400;
}

void	ParseRequest::ParseChunked (std::string _body)
{
	int size = 1;
	std::string hex;
	std::string filename;
	std::map<std::string, std::string>::iterator it = header.find("Content-Type");
	if (it != header.end())
		filename = get_ContentType(it->second);
	else
		filename = "file.txt";
	std::fstream file(filename, std::ios::out);
	std::string body;
	for (; size != 0; ) {
		hex = get_hex(_body);
		size = convert_hex(hex);
		_body = _body.substr((hex.length() + 2));
		body.append(_body, 0, size);
		file << body;
		_body = _body.substr(size + 2);
		body.clear();
	}
}

void ParseRequest::ParseBody (const std::string& _body) {
	if (_body.empty())
		std::cout << "No body found in the request." << std::endl;
		std::map<std::string, std::string>::iterator it = header.find("Content-Length");
		if (it != header.end()) {
			std::string filename;
		std::map<std::string, std::string>::iterator it = header.find("Content-Type");
		if (it != header.end())
			filename = get_ContentType(it->second);
		else
			filename = "file.txt";
		std::fstream file(filename, std::ios::out);
		file << _body;
		return ;
	}
	it = header.find("Transfer-Encoding");
	if (it != header.end()) {
		ParseChunked(_body);
	}
}

bool check_url(const std::string& url) {
	std::string allowd = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
	for (size_t i = 0; i < url.length(); i++)
		if (allowd.find(url[i]) == std::string::npos)
			return (false);
	return (true);
}

void	ParseRequest::requestStatusCode () {
	std::map<std::string, std::string>::iterator it = header.find("Transfer-Encoding");
	if (it != header.end() && it->second != "chunked")
		throw 501;
	if (it == header.end()) {
		it = header.find("Content-Length");
		if (it == header.end() && requestLine.method == "POST")
			throw 400;
	}
	if (requestLine.url.length() > 2048)
		throw 414;
	if (!check_url(requestLine.url))
		throw 400;
}

void ParseRequest::ParseHttpRequest( std::string request) {
	std::string line;
	size_t startline = request.find("\r\n");
	if (startline != std::string::npos) {
		line = request.substr(0, startline);
		ParseStartLine(line);
		request = request.substr(startline + 2);
	}
	size_t headers = request.find("\r\n\r\n");
	if (headers != std::string::npos) {
		line = request.substr(0, headers);
		ParseMethod(line);
		headers += 4;
	}
	requestStatusCode();
	line = request.substr(headers);
	ParseBody(line);

	// std::cout << "_______________START_LINE____________" << std::endl;
	// std::cout << "METHOD = " << requestLine.method << "       ";
	// std::cout << "URL = " << requestLine.url << "       ";
	// std::cout << "VERSION = " << requestLine.http_v << std::endl << std::endl;



	// std::cout << "_______________HEADERS____________" << std::endl << std::endl;
	// std::map<std::string, std::string>::iterator iter = header.begin();
    // for (; iter != header.end(); iter++) {
    //     std::cout << iter->first << ": " << iter->second << std::endl;
    // }

	// std::cout << "_______________BODY____________" << std::endl << std::endl;
}