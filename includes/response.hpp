/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 17:35:49 by omanar            #+#    #+#             */
/*   Updated: 2023/07/27 18:42:01 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <unistd.h>
# include <sys/stat.h>
# include "config.hpp"
# include "socket.hpp"

class Server;

class Response
{
	private:
		char		**_env;
		std::string _response;
		std::map<std::string, std::string> _headers;
		std::streampos pospause;
		std::string _body;
		std::string _protocol;
		std::string _status_code;
		std::string _status_message;
		std::string _content_type;
		std::string _content_length;
		std::string _date;
		std::string _allow;
		std::string _ReasonPhrase;
		std::string _cgiHeader;
		std::string _redirect;
	public:
		bool close_connection;
		bool sending_data;
		bool finished;
		std::string response_not_send;			
		Response();
		~Response();
		Response(Response const &src);
		Response &operator=(Response const &src);
		std::string getResponse();
		std::string getProtocol();
		std::string getStatusCode();
		std::string getStatusMessage();
		std::string getContentType();
		std::string getContentLength();
		std::string getDate();
		std::string getAllow();
		std::string getReasonPhrase();
		std::string getRedirect();
		void setRedirect(std::string redirect);
		void setReasonPhrase(std::string ReasonPhrase);
		void setResponse(std::string response);
		void setCgiHeader(std::string cgiHeader);
		void setProtocol(std::string protocol);
		void setStatusCode(std::string status_code);
		void setStatusMessage(std::string status_message);
		void setContentType(std::string content_type);
		void setContentLength(std::string content_length);
		void setDate(std::string date);
		void setAllow(std::string allow);
		void setHeaders(std::string key, std::string value);
		void setBody(std::string body);
		void set_Header_Response(Server &serv, int j);
		void GET(Server &server, int j);
		void DELETE(Server &server, int j);
		bool isDirectory(std::string path);
		void setEnv(std::vector<std::string> env);
		char ** getEnv();

		void HandlePathType(const std::string& path, std::vector<Location>::iterator &location, Server &server, int j, std::string &filePath);
		void HandleFile(const std::string& path, std::vector<Location>::iterator &location, Server &server, int j, std::string  &filePath);
		void HandleDir(const std::string& path, std::vector<Location>::iterator &location, std::string &filePath);
		bool fileExists(const char* directoryPath, const char* fileName, std::string &indexfile);
};

#endif
