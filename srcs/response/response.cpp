#include "../../includes/response.hpp"

Response::Response() {
	_response = "";
	_protocol = "";
	_status_code = "";
	_status_message = "";
	_content_type = "";
	_content_length = "";
	_date = "";
	_allow = "";
	close_connection = false;
	sending_data = false;
}

Response::~Response() {
	// delete [] _env;
}

Response::Response(Response const &src) {
	*this = src;
}

Response &Response::operator=(Response const &src) {
	if (this != &src)
	{
		_response = src._response;
		_protocol = src._protocol;
		_status_code = src._status_code;
		_status_message = src._status_message;
		_content_type = src._content_type;
		_content_length = src._content_length;
		_date = src._date;
		_allow = src._allow;
	}
	return (*this);
}

std::string Response::getResponse() { return (_response); }

std::string Response::getProtocol() { return (_protocol); }

std::string Response::getStatusCode() { return (_status_code); }

std::string Response::getStatusMessage() { return (_status_message); }

std::string Response::getContentType() { return (_content_type); }

std::string Response::getContentLength() { return (_content_length); }

std::string Response::getDate() { return (_date); }

std::string Response::getAllow() { return (_allow); }

std::string Response::getReasonPhrase() { return (_ReasonPhrase) ;}

void Response::setReasonPhrase(std::string ReasonPhrase) { _ReasonPhrase = ReasonPhrase; }

void Response::setResponse(std::string response) { _response = response; }

void Response::setProtocol(std::string protocol) { _protocol = protocol; }

void Response::setStatusCode(std::string status_code) { _status_code = status_code; }

void Response::setStatusMessage(std::string status_message) { _status_message = status_message; }

void Response::setContentType(std::string content_type) { _content_type = content_type; }

void Response::setContentLength(std::string content_length) { _content_length = content_length; }

void Response::setDate(std::string date) { _date = date; }

void Response::setAllow(std::string allow) { _allow = allow; }

void Response::setHeaders(std::string key, std::string value) { _headers[key] = value; }

void Response::setBody(std::string body) { _body = body; }

void Response::setResponse(std::string protocol, std::string status_code, std::string status_message, std::string body) {
	_response = protocol + " " + status_code + " " + status_message + "\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
	_response += "Date: " + getDate() + "\r\n";
	_response += "Allow: " + getAllow() + "\r\n";
	_response += "\r\n";
	_response += body;
}

void	Response::GET(Server &serv,int j){
	std::string body;
	std::string response;
	response = "";
	response += response_not_send;
	response_not_send = "";
	std::string path =  serv.configs[0]->_locations->begin()->_root + serv._requests[serv._pollfds[j].fd].requestLine.url;
	// std::string path = serv._location_match->_root + serv._requests[serv._pollfds[j].fd].requestLine.url;
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
    	send(serv._pollfds[j].fd, response.c_str(), response.length(), 0);
		close_connection = true;
		return ;
	}
	if(!sending_data){
		std::cout<<"sending header"<<std::endl;
		std::ostringstream response_stream;
		std::string extension = path.substr(path.find_last_of(".") + 1);
		response_stream << "HTTP/1.1 200 OK\r\n";
		response_stream << "HTTP/1.1 "+ serv._responses[serv._pollfds[j].fd].getStatusCode() +" "+serv._responses[serv._pollfds[j].fd].getReasonPhrase()+"\r\n";
		response_stream << "Content-Type: "  + serv._responses[serv._pollfds[j].fd].getContentType() + "\r\n";
		response_stream << "Connection: keep-alive\r\n";
		response_stream << "Transfer-Encoding: chunked\r\n";
		response_stream << "\r\n";
		response = response_stream.str();

		// send(serv._pollfds[j].fd, response_header.c_str(), response_header.length(), 0);
		sending_data = true;
		// return ;
	}
	else if (sending_data && response == ""){
		std::cout<<"sending data"<<std::endl;
		// char chunk_buffer[4096];
		char chunk_buffer[65536];	
		file.seekg(pospause);
		file.read(chunk_buffer, sizeof(chunk_buffer));
		pospause = file.tellg();
		int chunk_size = file.gcount();
		if (chunk_size > 0) {
			std::ostringstream chunk_stream;
			chunk_stream << std::hex << chunk_size << "\r\n";
			std::string chunk_header = chunk_stream.str();
			response = chunk_header + std::string(chunk_buffer, chunk_size) + "\r\n";
			// send(serv._pollfds[j].fd, chunk_header.c_str(), chunk_header.length(), 0);
			// send(serv._pollfds[j].fd, chunk_buffer, chunk_size, 0);
			// send(serv._pollfds[j].fd, "\r\n", 2, 0);
			file.close();
		}
		else{
			std::cout<<"sending end"<<std::endl;
			response = "0\r\n\r\n";
			sending_data = false;
			close_connection = true;
		}
	}
	int ret = send(serv._pollfds[j].fd, response.c_str(), response.length(), 0);
	if (ret < 1){
		std::cout << "error send" << std::endl;
		response_not_send = response;
	}
	else if(ret < (int)response.length()){
		std::cout << "the complete response was not sent" << std::endl;
		response_not_send = response.substr(ret);
		// close_connection = false;
	}
}
bool Response::isDirectory(std::string path) {
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return (S_ISDIR(path_stat.st_mode));
}

void Response::DELETE(std::string path) {
	if (access(path.c_str(), F_OK) != 0)
		setResponse("HTTP/1.1", "404", "Not Found", "DELETE");
	else if (access(path.c_str(), W_OK) != 0)
		setResponse("HTTP/1.1", "403", "Forbidden", "DELETE");
	else if (isDirectory(path))
		setResponse("HTTP/1.1", "403", "Forbidden", "DELETE");
	else if (remove(path.c_str()) != 0)
		setResponse("HTTP/1.1", "500", "Internal Server Error", "DELETE");
	else
		setResponse("HTTP/1.1", "200", "OK", "<html><body><h1>File deleted.</h1></body></html>");
}

void Response::setEnv(std::vector<std::string> env) {
	std::cout << "env size: " << env.size() << std::endl;
	this->_env = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
		this->_env[i] = strdup(env[i].c_str());
	this->_env[env.size()] = NULL;
}

char** Response::Response::getEnv() {
	 return (_env);
}