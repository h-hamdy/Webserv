# include "../../includes/webserv.hpp"

void	ParseRequest::ParseStartLine (const std::string& StartLine) {
	std::istringstream iss(StartLine);

	iss >> requestLine.method >> requestLine.url >> requestLine.http_v;
}

void ParseRequest::ParseMethod (std::string& method) {
	std::istringstream iss(method);
    std::string line, key, value;
	int space, pos;

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

void	ParseRequest::ParseChunked (std::string _body, ssize_t byteRead)
{
	std::string hex;
	std::string body;
	if (_bread > 0) {
		body.append(_body, 0, _bread);
		file << body;
		file.flush();
		_body = _body.substr(0, _bread);
		body.clear();
		_bread -= byteRead;
		return ;
	}
	if (_bread == 0) {
		hex = get_hex(_body);
		size = convert_hex(hex);
	}
	for (; byteRead > 0; ) {
		_body = _body.substr((hex.length() + 2));
		byteRead -= (hex.length() + 2);
		if (size >= byteRead) {
			_bread = size - byteRead;
			body = _body.substr(0, byteRead);
			file << body;
			file.flush();
			_body = _body.substr(byteRead);
			byteRead = 0;
		}
		else {
			body.append(_body, 0, size);
			file << body;
			_body = _body.substr(size + 2);
			byteRead -= size;
		}
		body.clear();
	}
	if (size == 0)
		_EOF = 0;
}

void ParseRequest::ParseBody (const std::string& _body, ssize_t byteRead) {
	if (_EOF == 3) {
		std::string filename;
		std::map<std::string, std::string>::iterator it = header.find("Content-Type");
		if (it != header.end())
			filename = get_ContentType(it->second);
		else
			filename = "file.txt";
		this->file.open(filename, std::ios::binary | std::ios::app | std::ios::ate);
		_EOF = 1;
	}
	std::map<std::string, std::string>::iterator it = header.find("Content-Length");
	if (it != header.end()) {
		file << _body;
		std::streampos fileSize = file.tellg();
		_EOF = 1;
		if (std::stoi(it->second) == fileSize) {
			_EOF = 0;
		}
		return ;
	}
	else
		ParseChunked(_body, byteRead);
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

void ParseRequest::ParseHttpRequest( std::string request, ssize_t byteRead) {
	std::string line;
	size_t headers = 0;
	if(byteRead == -1)
		return ;
	size_t startline = request.find("\r\n");
	if (startline != std::string::npos && requestLine.method.empty()) {
		line = request.substr(0, startline);
		ParseStartLine(line);
		request = request.substr(startline + 2);
		byteRead -= (startline + 2);
		headers = request.find("\r\n\r\n");
		if (headers != std::string::npos) {
			line = request.substr(0, headers);
			ParseMethod(line);
			headers += 4;
			byteRead -= headers;
		}
		try {
			requestStatusCode();
		}
		catch (int status) {
			std::cout << status << std::endl;
		}

	}
	if (requestLine.method == "POST") {
		line = request.substr(headers);
		ParseBody(line, byteRead);
	}
	else
		_EOF = 0;
}