<<<<<<< HEAD:srcs/socket/RequestHTTP.cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHTTP.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 14:43:14 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/17 17:14:00 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "main.hpp"
=======
# include "RequestHTTP.hpp"
>>>>>>> nargouse:srcs/RequestHTTP.cpp

/*
** Constructors and destructor.
*/

RequestHTTP::RequestHTTP() : _method(UNKNOWN), _uri(""), _version(""), _body("") {}

RequestHTTP::RequestHTTP(const std::string& request) : _method(UNKNOWN), _uri(""), _version(""), _body("") 
{
    this->parseRequest(request);
}

RequestHTTP::RequestHTTP(const RequestHTTP &src) 
{
    *this = src;
}

RequestHTTP::~RequestHTTP() {}

/*
** Operators Overload
*/

RequestHTTP &RequestHTTP::operator=(const RequestHTTP &rhs) 
{
    if (this != &rhs) {
        this->_method = rhs._method;
        this->_uri = rhs._uri;
        this->_version = rhs._version;
        this->_body = rhs._body;
        this->_headers = rhs._headers;
    }
    return *this;
}

std::ostream    &operator<<(std::ostream &o, const RequestHTTP &i) 
{
    o << "Method: " << i.getMethod() << std::endl;
    o << "URI: " << i.getURI() << std::endl;
    o << "HTTP Version: " << i.getHTTPVersion() << std::endl;
    o << "Headers: " << std::endl;
    o << i.getHeaders() << std::endl;
    o << "Body: " << i.getBody() << std::endl;
    
    return o;
}

/*
** Getters
*/

std::string RequestHTTP::getMethod() const 
{
    if (this->_method == GET)
        return "GET";
    else if (this->_method == POST)
        return "POST";
    else if (this->_method == DELETE)
        return "DELETE";
    else
        return "UNKNOWN";
}

std::string RequestHTTP::getURI() const 
{
    return this->_uri;
}

std::string RequestHTTP::getHTTPVersion() const 
{
    return this->_version;
}

std::string RequestHTTP::getBody() const 
{
    return this->_body;
}

std::string RequestHTTP::getHeaders() const 
{
    std::string headers;
    
    for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
        headers += it->first + ": " + it->second + "\n";
        
    return headers;
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
    if (headers.empty()) {
        throw std::runtime_error("headers vector is empty");
    }
    //Now we are going to parse the std::vector<std::string> &headers and put the key and value in the map.
    for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it) 
    {
        std::string header = *it;
        size_t pos = header.find(':');
        if (pos == std::string::npos) 
            return ;
        std::string key = header.substr(0, pos);
        std::string value = header.substr(pos + 1);
        key = trim(key);
        value = trim(value);
        if (key.empty() || value.empty())
            return ;
        _headers[key] = value;
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
    _uri = RequestHTTP::formatRequestURI(requestLine[1]);
    _version = requestLine[2];
    
    std::vector<std::string> headerLines;
    for (size_t i = 1; i < lines.size(); i++) 
    {
        if (lines[i].empty())
            break;
        headerLines.push_back(lines[i]);
    }
    parseHeaders(headerLines);

    for (size_t i = headerLines.size() + 2; i < lines.size(); i++) {
        _body += lines[i];
    }
}

std::string RequestHTTP::formatRequestURI(const std::string &uri)
{
    std::string formattedURI = uri;
    //Check for leading slash and add it if it's missing.
    if (formattedURI[0] != '/')
        formattedURI = "/" + formattedURI;
    //Check for double slashes and remove them.
    for (size_t i = 0; i < formattedURI.size() - 1; i++)
    {
        if (formattedURI[i] == '/' && formattedURI[i + 1] == '/') {
            formattedURI.erase(i, 1);
            i--;
        }
    }
    //Check for query string and remove it.
    size_t pos = formattedURI.find('?');
    if (pos != std::string::npos)
        formattedURI.erase(pos);
    return formattedURI;
}