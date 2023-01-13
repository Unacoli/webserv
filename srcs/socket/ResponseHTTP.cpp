/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHTTP.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 15:38:28 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/13 10:43:47 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "main.hpp"

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
            for (std::vector<t_location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
            {
                if (request.getURI().find(it->name) != std::string::npos)
                {
                    if (!it->root.empty())
                    {
                        if (isDirectory(it->root))
                        {
                            if (it->index.empty() && server.index.empty())
                                this->_statusCode = ResponseHTTP::NOT_FOUND;
                            else
                            {
                                if (!it->index.empty())
                                {
                                    for (std::vector<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); it2++)
                                    {
                                        if (isReadable(it->root + *it2))
                                        {
                                            this->_statusCode = ResponseHTTP::OK;
                                            this->_body = readFile(it->root + *it2);
                                        }
                                        else
                                            this->_statusCode = ResponseHTTP::NOT_FOUND;
                                    }
                                }
                                else if (it->index.empty() && !server.index.empty())
                                {
                                    for (std::vector<std::string>::iterator it2 = server.index.begin(); it2 != server.index.end(); it2++)
                                    {
                                        if (isReadable(it->root + *it2))
                                        {
                                            this->_statusCode = ResponseHTTP::OK;
                                            this->_body = readFile(it->root + *it2);
                                        }
                                        else
                                            this->_statusCode = ResponseHTTP::NOT_FOUND;
                                    }
                                }
                                else
                                    this->_statusCode = ResponseHTTP::NOT_FOUND;
                            }
                        }
                        else
                            this->_statusCode = ResponseHTTP::NOT_FOUND;
                    }
                    else if (it->root.empty() && !server.root.empty())
                    {
                        if (isDirectory(server.root))
                        {
                            if (it->index.empty() && server.index.empty())
                                this->_statusCode = ResponseHTTP::NOT_FOUND;
                            else
                            {
                                if (!it->index.empty())
                                {
                                    for (std::vector<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); it2++)
                                    {
                                        if (isReadable(server.root + *it2))
                                        {
                                            this->_statusCode = ResponseHTTP::OK;
                                            this->_body = readFile(server.root + *it2);
                                        }
                                        else
                                            this->_statusCode = ResponseHTTP::NOT_FOUND;
                                    }
                                }
                                else
                                    this->_statusCode = ResponseHTTP::NOT_FOUND;
                            }
                        }
                        else
                            this->_statusCode = ResponseHTTP::NOT_FOUND;
                    }
                    else
                        this->_statusCode = ResponseHTTP::NOT_FOUND;
                }
                else
                    this->_statusCode = ResponseHTTP::NOT_FOUND;
            }        
        }
        else if (request.getURI() != "/")
        {
            // Check if the request is asking for a directory in the root directory of a location
            if (is
            // Check if the request is asking for a directory in a location
            // Check if the request is asking for a directory in the root directory
            else if (!server.root.empty() && isDirectory(server.root))
            {
                if (isDirectory(server.root + request.getURI()))
                {
                    if (server.index.empty())
                        this->_statusCode = ResponseHTTP::NOT_FOUND;
                    else
                    {
                        for (std::vector<std::string>::iterator it = server.index.begin(); it != server.index.end(); it++)
                        {
                            if (isReadable(server.root + request.getURI() + *it))
                            {
                                this->_statusCode = ResponseHTTP::OK;
                                this->_body = readFile(server.root + request.getURI() + *it);
                            }
                            else
                                this->_statusCode = ResponseHTTP::NOT_FOUND;
                        }
                    }
                }
                else
                    this->_statusCode = ResponseHTTP::NOT_FOUND;
            }
        }
        else
            this->_statusCode = ResponseHTTP::NOT_FOUND;
    }
    else
    {
        // Check if the request is asking for a file
        // Check if the request is asking for a file in the root directory
        // Check if the request is asking for a file in a location
        // Check if the request is asking for a file in the root directory of a location
    }
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
