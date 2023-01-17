/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHTTP.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 15:38:28 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/13 15:19:32 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

/*
** Constructors and Destructor
*/

ResponseHTTP::ResponseHTTP() : _statusCode(), _statusPhrase(""), _body(""), _headers() {}

ResponseHTTP::ResponseHTTP( ResponseHTTP const &src ) {
    *this = src;
}

/*ResponseHTTP::ResponseHTTP( const RequestHTTP& request, StatusCode code, const std::string& body, t_server server)
{
    
}*/

ResponseHTTP::~ResponseHTTP(){}

/*
** Operators overload
*/

ResponseHTTP &ResponseHTTP::operator=(const ResponseHTTP &rhs)
{
    if (this != &rhs)
    {
        this->_statusCode = rhs._statusCode;
        this->_statusPhrase = rhs._statusPhrase;
        this->_body = rhs._body;
        this->_headers = rhs._headers;
    }
    return *this;
}

std::ostream    &operator<<(std::ostream &o, const ResponseHTTP &i)
{
    o << "Status Code: " << i.getStatusCode() << std::endl;
    o << "Status Phrase: " << i.getStatusPhrase() << std::endl;
    o << "Body: " << i.getBody() << std::endl;
    o << "Headers: " << i.getHeaders() << std::endl;
    return o;
}

/*
** Getters
*/

ResponseHTTP::StatusCode  ResponseHTTP::getStatusCode() const {
    return this->_statusCode;
}

std::string         ResponseHTTP::getStatusPhrase() const {
    return this->_statusPhrase;
}

std::string         ResponseHTTP::getHeaders() const {
    std::string headers = "";
    for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
        headers += it->first + ": " + it->second + "\r";
    return headers;   
}

std::string         ResponseHTTP::getHeader( const std::string &name ) const {
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(name);
    if (it != this->_headers.end())
        return it->second;
    return "";
}

std::string         ResponseHTTP::getBody() const {
    return this->_body;
}

/*
** Public Methods
*/

/*std::string         ResponseHTTP::generateResponse(const RequestHTTP& request, StatusCode code, const std::string& body, t_server server)
{
}*/

/*
** Private Methods
*/



void        ResponseHTTP::methodDispatch(RequestHTTP request, t_server server)
{
    if (request.getMethod() == "GET")
        this->getMethodCheck(request, server);
    else if (request.getMethod() == "POST")
        this->postMethodCheck(request, server);
    else if (request.getMethod() == "DELETE")
        this->deleteMethodCheck(request, server);
    else
        this->_statusCode = ResponseHTTP::METHOD_NOT_ALLOWED;
}

// GET
// The getMethodCheck(request, server) function will check for every possible error that can occur with a GET request.
// To do so, it will check the t_server configuration and the std::vector<t_location> location inside of it.
// It will then change the StatusCode _statusCode accordingly.

void        ResponseHTTP::getMethodCheck(RequestHTTP request, t_server server)
{
    // Check if the request is asking for a directory
    if (request.getURI().back() == '/')
    {
        // Check if the request is asking for the root directory
        if (request.getURI() == "/")
        {
            // Check if this root directory is set in a location configuration, if not, check in the server configuration.
            for (std::vector<t_location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
            {
                if (it->path == "/")
                {
                    if ( it->index.size() != 0 || server.default_serv.index.size() != 0)
                    {
                        //Check the access to any of the index and take the path to the first one working
                        
                        this->_statusCode = ResponseHTTP::OK;
                        this->_statusPhrase = generateStatusLine(this->_statusCode)
                        //Redirect to a function that will create the body from the file.
                        this->_body =
                    }
                    else if ( it->autoindex == true )
                    {
                        this->_statusCode = ResponseHTTP::OK;
                        this->_statusPhrase = generateStatusLine(this->_statusCode);
                        this->_body = this->generateAutoindex(request, server);
                        return;
                    }
                    else
                        this->_statusCode = ResponseHTTP::FORBIDDEN;
                    return;
                }
            }
            if (server.default_serv.root == "")
                this->_statusCode = ResponseHTTP::NOT_FOUND;
            else
            {
                if (server.default_serv.index.size() != 0)
                {
                    //Check the access of every index and send in the body the first one being a readable file.
                    
                }
                else if (server.default_serv.autoindex == true)
                {
                    this->_statusCode = ResponseHTTP::OK;
                    this->_statusPhrase = generateStatusLine(this->_statusCode);
                    this->_body = this->generateAutoindex(request, server.default_serv);
                    return;
                }
                else
                    this->_statusCode = ResponseHTTP::FORBIDDEN;
            }
        }
    }
}

// This function will create an automatic directory listing from a URI found in a RequestHTTP)
std::string ResponseHTTP::generateAutoindex(RequestHTTP request, t_location location)
{
    
}

std::string ResponseHTTP::generateStatusLine(ResponseHTTP::StatusCode code)
{
    std::string statusLine = "";
    switch (code)
    {
        case ResponseHTTP::OK:
            statusLine = "HTTP/1.1 200 OK\r";
            break;
        case ResponseHTTP::CREATED:
            statusLine = "HTTP/1.1 201 Created\r";
            break;
        case ResponseHTTP::NO_CONTENT:
            statusLine = "HTTP/1.1 204 No Content\r";
            break;
        case ResponseHTTP::MULTIPLE_CHOICES:
            statusLine = "HTTP/1.1 300 Multiple Choices\r";
            break;
        case ResponseHTTP::MOVED_PERMANENTLY:
            statusLine = "HTTP/1.1 301 Moved Permanently\r";
            break;
        case ResponseHTTP::FOUND:
            statusLine = "HTTP/1.1 302 Found\r";
            break;
        case ResponseHTTP::NOT_MODIFIED:
            statusLine = "HTTP/1.1 304 Not Modified\r";
            break;
        case ResponseHTTP::BAD_REQUEST:
            statusLine = "HTTP/1.1 400 Bad Request\r";
            break;
        case ResponseHTTP::UNAUTHORIZED:
            statusLine = "HTTP/1.1 401 Unauthorized\r";
            break;
        case ResponseHTTP::FORBIDDEN:
            statusLine = "HTTP/1.1 403 Forbidden\r";
            break;
        case ResponseHTTP::NOT_FOUND:
            statusLine = "HTTP/1.1 404 Not Found\r";
            break;
        case ResponseHTTP::METHOD_NOT_ALLOWED:
            statusLine = "HTTP/1.1 405 Method Not Allowed\r";
            break;
        case ResponseHTTP::CONFLICT:
            statusLine = "HTTP/1.1 409 Conflict\r";
            break;
        case ResponseHTTP::GONE:
            statusLine = "HTTP/1.1 410 Gone\r";
            break;
        case ResponseHTTP::INTERNAL_SERVER_ERROR:
            statusLine = "HTTP/1.1 500 Internal Server Error\r";
            break;
        case ResponseHTTP::SERVICE_UNAVAILABLE:
            statusLine = "HTTP/1.1 503 Service Unavailable\r";
            break;
        case ResponseHTTP::GATEWAY_TIMEOUT:
            statusLine = "HTTP/1.1 504 Gateway Timeout\r";
            break;
    }
}
