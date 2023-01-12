/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHTTP.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 14:43:14 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 15:11:08 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "RequestHTTP.hpp"

/*
** Constructors and destructor.
*/

RequestHTTP::RequestHTTP() : _method(UNKNOWN), _uri(""), _version(""), _body("") {}

RequestHTTP::RequestHTTP(const RequestHTTP &src) {
    *this = src;
}

RequestHTTP::~RequestHTTP() {}

/*
** Operators Overload
*/

RequestHTTP &RequestHTTP::operator=(const RequestHTTP &rhs) {
    if (this != &rhs) {
        this->_method = rhs._method;
        this->_uri = rhs._uri;
        this->_version = rhs._version;
        this->_body = rhs._body;
        this->_headers = rhs._headers;
    }
    return *this;
}

std::ostream    &operator<<(std::ostream &o, const RequestHTTP &i) {
    o << "Method: " << i.getMethod() << std::endl;
    o << "URI: " << i.getURI() << std::endl;
    o << "HTTP Version: " << i.getHTTPVersion() << std::endl;
    o << "Body: " << i.getBody() << std::endl;
    o << "Headers: " << std::endl;
    
    return o;
}

/*
** Getters
*/

RequestHTTP::Method RequestHTTP::getMethod() const {
    return this->_method;
}

std::string RequestHTTP::getURI() const {
    return this->_uri;
}

std::string RequestHTTP::getHTTPVersion() const {
    return this->_version;
}

std::string RequestHTTP::getBody() const {
    return this->_body;
}

std::string RequestHTTP::getHeaders() const {
    std::string headers;
    for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) 
        headers += it->first + ": " + it->second + "\r";
    return  headers;
}

std::string	RequestHTTP::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
    if (it != this->_headers.end())
        return it->second;
    return NULL;
}

/*
**  Private Methods
*/

void    RequestHTTP::parseHeaders( std::vector<std::string> &headers )
{
    for (int i = 0; i < headers.size(); i++) 
    {
        std::vector<std::string> header;
        split(headers[i], ':', header);
        if (header.size() != 2) {
            throw std::runtime_error("Invalid header");
        }
        _headers[header[0]] = header[1];
    }
}

void    RequestHTTP::parseRequest(const std::string &request)
{
    std::vector<std::string> lines;
    split(request, '\n', lines);
    
    std::vector<std::string> requestLine;
    split(lines[0], ' ', requestLine);
    if (requestLine.size() != 3) {
        throw std::runtime_error("Invalid request line");
    }
    if (requestLine[0] == "GET") {
        _method = GET;
    } 
    else if (requestLine[0] == "POST") {
        _method = POST;
    }
    else if (requestLine[0] == "DELETE") {
        _method = DELETE;
    } 
    else {
        _method = UNKNOWN;
    }
    _uri = requestLine[1];
    _version = requestLine[2];
    
    std::vector<std::string> headerLines;
    for (int i = 1; i < lines.size(); i++) 
    {
        if (lines[i].empty())
            break;
        headerLines.push_back(lines[i]);
    }
    parseHeaders(headerLines);
    
    for (int i = headerLines.size() + 2; i < lines.size(); i++) {
        _body += lines[i];
    }
}