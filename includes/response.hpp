/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 17:35:49 by omanar            #+#    #+#             */
/*   Updated: 2023/07/19 22:03:58 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <unistd.h>
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
	public:
		bool close_connection;
		bool sending_data;
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
		void setReasonPhrase(std::string ReasonPhrase);
		void setResponse(std::string response);
		void setProtocol(std::string protocol);
		void setStatusCode(std::string status_code);
		void setStatusMessage(std::string status_message);
		void setContentType(std::string content_type);
		void setContentLength(std::string content_length);
		void setDate(std::string date);
		void setAllow(std::string allow);
		void setHeaders(std::string key, std::string value);
		void setBody(std::string body);
		void setResponse(std::string protocol, std::string status_code, std::string status_message, std::string body);
		void GET(Server &server, int j);
		// void DELETE(std::string path, Config *config);
		void DELETE(std::string path);
		bool isDirectory(std::string path);
		void setEnv(std::vector<std::string> env);
};

#endif
