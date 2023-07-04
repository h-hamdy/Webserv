/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/04 17:35:51 by omanar            #+#    #+#             */
/*   Updated: 2023/07/04 17:52:49 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

Response::Response() {
	_response = "";
	_protocol = "";
	_status_code = "";
	_status_message = "";
	_content_type = "";
	_content_length = "";
	_date = "";
	_allow = "";
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
