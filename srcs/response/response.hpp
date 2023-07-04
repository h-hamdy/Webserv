/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 17:35:49 by omanar            #+#    #+#             */
/*   Updated: 2023/07/04 17:48:47 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <unistd.h>
# include "../config/config.hpp"

class Response
{
	private:
		std::string _response;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _protocol;
		std::string _status_code;
		std::string _status_message;
		std::string _content_type;
		std::string _content_length;
		std::string _date;
		std::string _allow;
	public:
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
};

#endif
