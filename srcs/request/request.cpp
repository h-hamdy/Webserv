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
	return (std::stoi(hex, NULL, 16));
}

int	ParseRequest::get_size (std::string &body, Server &server, int j)
{
		(void)server;
	(void)j;
	std::string hex;
	if (body[0] == '\r' && body[1] == '\n') {
		body = body.substr(2);
		server._bytesRead -= 2;
	}
	hex = get_hex(body);
	size = convert_hex(hex);
	if (size == 0) {
		_EOF = 0;
		if (!server._location_match->_upload_path.empty())
			throw 201;
		return 1;
	}
	body = body.substr((hex.length() + 2));
	find_size = false;
	return (0);
}

void	ParseRequest::ParseChunked (std::string _body, Server &server, int j)
{
	std::string hex;

	body += _body;
	if (find_size)
		if (get_size(body, server, j))
			return ;
	if (body.size() >= size) {
		file << body.substr(0, size);
		file.flush();
		body = body.substr(size);
		if (body[2] == '0') {
			_EOF = 0;
			if (!server._location_match->_upload_path.empty())
				throw 201;
		}
		find_size = true;
	}
}

void ParseRequest::ParseBody (const std::string& _body, Server &server, int j) {
	std::streampos fileSize = file.tellg();
	if (fileSize > server.configs[0]->_max_body_size)
		throw 413;
	std::map<std::string, std::string>::iterator it = header.find("Content-Length");
	if (it != header.end()) {
		file << _body;
		_EOF = 1;
		fileSize = file.tellg();
		if (fileSize >= std::stoi(it->second) ) {
			_EOF = 0;
			if (!server._location_match->_upload_path.empty())
				throw 201;
		}
		return ;
	}
	else
		ParseChunked(_body, server, j);
}

bool check_url(const std::string& url) {
	std::string allowd = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._~:/?#[]@!$&'()*+,;=%";
	for (size_t i = 0; i < url.length(); i++)
		if (allowd.find(url[i]) == std::string::npos)
			return (false);
	return (true);
}

void	ParseRequest::requestStatusCode () {
	std::map<std::string, std::string>::iterator it = header.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator it1 = header.find("Content-Length");
	std::map<std::string, std::string>::iterator it2 = header.find("User-Agent");
	std::map<std::string, std::string>::iterator it3 = header.find("Host");
	if (it != header.end() && it1 != header.end() && it2 != header.end() && it3 != header.end())
		throw 400;
	if (requestLine.method != "GET" && requestLine.method != "POST" && requestLine.method != "DELETE") {
		std::cout << "method = "<< requestLine.method << std::endl;
		if (requestLine.method != "PUT" && requestLine.method != "PATCH" && requestLine.method != "HEAD" && requestLine.method != "OPTIONS")
			throw 400;
		else
			throw 501;
	}
	it1 = header.find("Content-Type");
	if (it1 == header.end() && requestLine.method == "POST")
		throw 400;
	if (it1 != header.end() && it != header.end()) {
		std::string flag = "multipart/form-data; boundary=";
		std::string boudrie = it1->second.substr(0, flag.length());
		if (boudrie == flag)
			throw 400;
	}
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

std::string ParseRequest::ParseHttpRequest( std::string request, ssize_t byteRead,Server &server, int j) {
	(void)server;
	(void)j;
	std::string line;
	size_t headers = 0;
	if(byteRead == -1)
		return "";
	size_t startline = request.find("\r\n");
	if (startline != std::string::npos) {
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
	}
	if (requestLine.method != "POST") {
		_EOF = 0;
		return ("");
	}
	return (request.substr(headers));
}