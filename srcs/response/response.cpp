#include "../../includes/response.hpp"
#include <dirent.h>

Response::Response() {
	_response = "";
	_protocol = "";
	_status_code = "200";
	_status_message = "";
	_content_type = "";
	_content_length = "";
	_date = "";
	_allow = "";
	_cgiHeader = "";
	_ReasonPhrase = "";
	_redirect = "";
	close_connection = false;
	sending_data = false;
	finished = false;
}

Response::~Response() {}

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

std::string Response::getRedirect() { return (_redirect); }

void Response::setRedirect(std::string redirect) { _redirect = redirect; }

void Response::setReasonPhrase(std::string ReasonPhrase) { _ReasonPhrase = ReasonPhrase; }

void Response::setResponse(std::string response) { _response = response; }

void Response::setProtocol(std::string protocol) { _protocol = protocol; }

void Response::setCgiHeader(std::string cgiHeader) { _cgiHeader = cgiHeader; }

void Response::setStatusCode(std::string status_code) { _status_code = status_code; }

void Response::setStatusMessage(std::string status_message) { _status_message = status_message; }

void Response::setContentType(std::string content_type) { _content_type = content_type; }

void Response::setContentLength(std::string content_length) { _content_length = content_length; }

void Response::setDate(std::string date) { _date = date; }

void Response::setAllow(std::string allow) { _allow = allow; }

void Response::setHeaders(std::string key, std::string value) { _headers[key] = value; }

void Response::setBody(std::string body) { _body = body; }

void Response::set_Header_Response(Server &serv, int j) {
	if(!sending_data){
		std::cout<<"sending header"<<std::endl;
		
		std::ostringstream response_stream;
		std::string extension = serv.configs[0]->_locations->begin()->_root.substr(serv.configs[0]->_locations->begin()->_root.find_last_of(".") + 1);
		response_stream << "HTTP/1.1 " + serv._responses[serv._pollfds[j].fd].getStatusCode() + " " + serv._responses[serv._pollfds[j].fd].getReasonPhrase() + "\r\n";
		response_stream << "Content-Type: "  + serv._responses[serv._pollfds[j].fd].getContentType() + "\r\n";
		if(serv._responses[serv._pollfds[j].fd].getRedirect() != "")
			response_stream << "Location: " + serv._responses[serv._pollfds[j].fd].getRedirect() + "\r\n";
		if (_cgiHeader != ""){
			response_stream << _cgiHeader + "\r\n";
			std::cout << "cgi header: " << _cgiHeader << std::endl;
		}
		response_stream << "Connection: keep-alive\r\n";
		response_stream << "Transfer-Encoding: chunked\r\n";
		response_stream << "\r\n";
		std::string response_header = response_stream.str();
		// std::cout << "res :" << response_header << std::endl;
		if(_response != ""){
			std::ostringstream chunck_stream;
			int chunck_size = (int)_response.length();
			chunck_stream << std::hex << chunck_size << "\r\n";
			std::string chunck_header = chunck_stream.str();
			std::string response = chunck_header + _response + "\r\n";
			_response = response_header + response + (this->_status_code != "200" ? "0\r\n\r\n" : "");
			// std::cout << "response: " << _response << std::endl;
		}
		else
			_response = response_header;
		sending_data = true;
	}
	else if(sending_data){
		if(_response == "" && !close_connection){
			return ;
		}
			std::ostringstream chunck_stream;
			chunck_stream << std::hex << (int)_response.length() << "\r\n";
			std::string chunck_header = chunck_stream.str();
			std::string response = chunck_header + _response + "\r\n";
			_response = response;
	}
}

void	Response::GET(Server &serv,int j){
	std::string body = "";
	std::string response = "";
	std::string path =  serv._requests[serv._pollfds[j].fd].path;
	struct stat path_stat;

	if(stat(path.c_str(), &path_stat) == 0){
		if((path_stat.st_mode & S_IFDIR) && serv._location_match->_index != ""){
			std::cout << "path: ======= " << path << std::endl;
			path += "/" + serv._location_match->_index;
		}
		else if((path_stat.st_mode & S_IFDIR) && serv._location_match->_directory_listing){
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir(path.c_str())) != NULL) {
				body += "<html><head><title>Index of " + path + "</title></head><body>\n";
				body += "<h1>Index of " + path + "</h1>\n";
				body += "<ul>\n";
				while ((ent = readdir(dir)) != NULL) {
					std::string filename = ent->d_name;
					if (filename != "." && filename != "..") {
						std::string filepath = serv._requests[serv._pollfds[j].fd].requestLine.url +  (serv._requests[serv._pollfds[j].fd].requestLine.url[serv._requests[serv._pollfds[j].fd].requestLine.url.length() - 1] == '/' ?  "" : "/");
						std::cout << "file_path :" << filepath << std::endl;
						body += "<li><a href=\"" + filepath + filename + "\">" + filename + "</a></li>\n";
					}
				}
				body += "</ul>\n";
				body += "</body></html>\n";
				closedir(dir);
				setResponse(body);
				if(sending_data){
					setResponse("");
					close_connection = true;
				}
				return;
			}
			else{
				serv._responses[serv._pollfds[j].fd].setStatusCode("404");
				close_connection = true;
				return ;
			}
		}
		else if((path_stat.st_mode & S_IFDIR) && !serv.configs[0]->_locations->begin()->_directory_listing){
			serv._responses[serv._pollfds[j].fd].setStatusCode("403");
			close_connection = true;
			return ;
		}

	}
	std::ifstream file(path.c_str(), std::ios::binary);
	std::vector<Location>::iterator location = serv._location_match;
	if (location->_cgi_extensions.size() == 0)
        setStatusCode("403");
    size_t lastDotPos = path.rfind('.');
    std::string extention = path.substr(lastDotPos);
	if(!file.is_open()){
		serv._responses[serv._pollfds[j].fd].setStatusCode("404");
		close_connection = true;
		return ;
	}
    else if (lastDotPos != std::string::npos && (extention == ".py"|| extention == ".php" )) {
        std::vector<std::string>::iterator it;
        for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
            if (extention == *it) {
				// _response = "";
                CgiProcess(serv, j, path, extention, "");
				return ;
            }
        }
        if (it == location->_cgi_extensions.end()){
            setStatusCode("403");
			close_connection = true;
		}
    }
	else if (file.is_open() ){
		char chunk_buffer[4096];
		// std::cout<<"sending file"<<std::endl;
		file.seekg(pospause);
		file.read(chunk_buffer, sizeof(chunk_buffer));
		pospause = file.tellg();
		int chunk_size = file.gcount();
		if (chunk_size > 0) {
			response = std::string(chunk_buffer, chunk_size);
			file.close();
		}
		else{
			std::cout<<"sending end"<<std::endl;
			close_connection = true;
		}
	}
	setResponse(response);
}
bool Response::isDirectory(std::string path) {
	struct stat path_stat;
	stat(path.c_str(), &path_stat);
	return (S_ISDIR(path_stat.st_mode));
}

void Response::DELETE(Server &serv, int j) {
	std::string path = serv._requests[serv._pollfds[j].fd].path;
	// std::cout << "path: ======= " << path << std::endl;
	if(sending_data){
		close_connection = true;
		return ;
	}
	if (access(path.c_str(), F_OK) != 0) {
		setStatusCode("404");
	}
	else if (access(path.c_str(), W_OK) != 0) {
		setStatusCode("403");

	}
	else if (isDirectory(path)) {
		setStatusCode("403");
	}
	else if (remove(path.c_str()) != 0) {
		setStatusCode("500");
	}
	else {
		std::cout << "test5" << std::endl;
		setResponse("<html><body><h1>File deleted.</h1></body></html>");
	}
	// std::cout << "File deleted successfully." << std::endl;
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

void Response::freeEnv() {
	for (size_t i = 0; _env[i]; i++)
		free(_env[i]);
	delete [] _env;
}

bool Response::fileExists(const char* directoryPath, const char* fileName, std::string &indexfile) {
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        std::cerr << "Error opening directory." << std::endl;
        return false;
    }
    bool found = false;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::strncmp(entry->d_name, fileName, 5) == 0) {
            indexfile = entry->d_name;
            found = true;
            break;
        }
    }
    closedir(dir);
    return found;
}

void Response::HandleDir(const std::string& path, std::vector<Location>::iterator &location, std::string &filePath, Server& server, int j) {
    (void)location;
    std::string indexFile;
    std::string extention;
	std::cout << "dir" << std::endl;
    if (path[path.length() - 1] != '/') {
		server._responses[server._pollfds[j].fd].setRedirect(path + "/");
		server._responses[server._pollfds[j].fd].setStatusCode("301");
		close_connection = true;
    }
    else {
        if (location->_cgi_extensions.size() == 0) {
			server._responses[server._pollfds[j].fd].setStatusCode("403");
			close_connection = true;
			return ;
		}
        std::cout << "check index files" << std::endl;
        if (!fileExists(path.c_str(), "index", indexFile)) {
			server._responses[server._pollfds[j].fd].setStatusCode("403");
			close_connection = true;
			return ;
		}
        size_t lastDotPos = indexFile.rfind('.');
        if (lastDotPos != std::string::npos) {
            std::vector<std::string>::iterator it;
            extention = indexFile.substr(lastDotPos);
            for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
                if ((*it)[0] == ' ')
                    (*it).erase(1);
                if (extention == *it) {
                    (void)filePath;
                    std::cout << "pass to cgi" << std::endl;
					CgiProcess(server, j, path + indexFile, extention, filePath);
                    return ;
                }
            }
            if (it == location->_cgi_extensions.end()) {
				server._responses[server._pollfds[j].fd].setStatusCode("403");
				close_connection = true;
			}
        }
        else {
			server._responses[server._pollfds[j].fd].setStatusCode("403");
			close_connection = true;
		}
    }
}

void Response::HandleFile(const std::string& path, std::vector<Location>::iterator &location, Server &server, int j, std::string  &filePath) {
    std::string extention;
    std::cout << "file" << std::endl;
    if (location->_cgi_extensions.size() == 0) {
		server._responses[server._pollfds[j].fd].setStatusCode("403");
		close_connection = true;
		return ;
	}
    size_t lastDotPos = path.rfind('.');
    if (lastDotPos != std::string::npos) {
        std::vector<std::string>::iterator it;
        extention = path.substr(lastDotPos);
        for (it = location->_cgi_extensions.begin(); it != location->_cgi_extensions.end(); it++) {
            if (extention == *it) {
                std::cout << "Da5lo a7biba" << std::endl;
                CgiProcess(server, j, path, extention, filePath);
                std::cout << "rah da5l" << std::endl;
                return ;    
            }
        }
        if (it == location->_cgi_extensions.end()) {
			server._responses[server._pollfds[j].fd].setStatusCode("403");
			close_connection = true;
		}
    }
    else {
		server._responses[server._pollfds[j].fd].setStatusCode("403");
		close_connection = true;
	}
}

void Response::HandlePathType(const std::string& path, std::vector<Location>::iterator &location, Server &server, int j, std::string &filePath)
{
    struct stat fileStat;
    std::cout << "Path=" << path << std::endl;
    if (stat(path.c_str(), &fileStat) == 0)
    {
        if (S_ISREG(fileStat.st_mode))
            HandleFile(path, location, server, j, filePath);
        else if (S_ISDIR(fileStat.st_mode))
            HandleDir(path, location, filePath, server, j);
        else {
			server._responses[server._pollfds[j].fd].setStatusCode("404");
			close_connection = true;
		}
    }
    else {
		server._responses[server._pollfds[j].fd].setStatusCode("404");
		close_connection = true;
	}
}

void    Socket::check_methods(Server &server,int j){
    std::string method =  server._requests[server._pollfds[j].fd].requestLine.method;
        if(method == "GET")
            server._responses[server._pollfds[j].fd].GET(server, j);
        else if(method == "POST" && server._location_match->_upload_path.empty()) {
        std::cout << "Location does not support upload" << std::endl;
        std::string resource = server._requests[ server._pollfds[j].fd].requestLine.url.substr(server._location_match->_url.length());
        server._responses[server._pollfds[j].fd].HandlePathType(server._requests[server._pollfds[j].fd].path + resource, server._location_match, server, j,server._requests[ server._pollfds[j].fd].filePath);
    }
        else if(method == "DELETE")
            server._responses[server._pollfds[j].fd].DELETE(server, j);
}
