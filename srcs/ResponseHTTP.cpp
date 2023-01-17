<<<<<<< HEAD:srcs/socket/ResponseHTTP.cpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHTTP.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 15:38:28 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/17 18:02:28 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

=======
>>>>>>> nargouse:srcs/ResponseHTTP.cpp
#include "main.hpp"

/*
** Constructors and Destructor
*/

ResponseHTTP::ResponseHTTP() : _statusCode(), _statusPhrase(""), _body(""), _headers() {}

ResponseHTTP::ResponseHTTP( ResponseHTTP const &src ) {
    *this = src;
}

ResponseHTTP::ResponseHTTP( const RequestHTTP& request, const t_server server)
{
    defineLocation(request, server);
    generateResponse(request, server);
}

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
** Private Methods
*/

void        ResponseHTTP::generateResponse(const RequestHTTP& request, t_server server)
{
    std::string         path;
    int                 checkedPath;
    
    if (_location.root.size() == 0)
        path = server.default_serv.root;
    else
        path = _location.root;
    
    if ( path[path.size() - 1] == '/')
        path = std::string(path, 0, path.size() - 1);
    
    path += request.getURI();
    std::cerr << "Path: " << path << std::endl;
    
    checkedPath = checkPath(path);
    if ( checkedPath == 2 )
    {
        //This means that the path is a directory.
        if ( _location.index.size() > 0 )
        {
            for (std::vector<std::string>::const_iterator it = _location.index.begin(); it != _location.index.end(); it++)
            {
                if ( checkPath(path + "/" + *it) == 1 )
                {
                    path += "/" + *it;
                    break;
                }
            }
        }
        else
        {
            if ( _location.autoindex == true)
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
            return ;
        }
    }
    
    checkedPath = checkPath(path);
    if ( checkedPath == 0 )
    {
        //This means that the path is not a directory or a file.
        ResponseHTTP::buildResponse(ResponseHTTP::NOT_FOUND, ResponseHTTP::generateStatusLine(ResponseHTTP::NOT_FOUND), request);
        return ;
    }
    if ( 0 )
    {
        //Here will take place the CGI test.
    }
    this->_path = path;
    ResponseHTTP::methodDispatch(request, server);
    return ;
}

void        ResponseHTTP::buildResponse( const ResponseHTTP::StatusCode &code, const std::string &statusLine, const RequestHTTP &request)
{
    this->_statusCode = code;
    this->_statusPhrase = statusLine;
    this->_headers["Date"] = ResponseHTTP::generateDate();
    this->_headers["Server"] = "Webserv/1.0";
    this->_headers["Content-Type"] = ResponseHTTP::defineContentType(request);
    this->_headers["Content-Length"] = ResponseHTTP::defineContentLength();
    this->_headers["Connection"] = "close";
    this->_body = ResponseHTTP::generateBody();
    ResponseHTTP::responseMaker();
}

void        ResponseHTTP::responseMaker( void )
{
    std::string     response;
    
    response = this->_statusPhrase + "\r";
    response += this->getHeaders();
    response += "\r";
    response += this->_body;
    this->_response = response;
}

std::string ResponseHTTP::generateDate( void )
{
    time_t      rawtime;
    struct tm   *timeinfo;
    char        buffer[80];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
    return std::string(buffer);
}

std::string ResponseHTTP::defineContentType( const RequestHTTP &request)
{
    std::string     extension;
    std::string     contentType;
    size_t          pos;
    
    pos = request.getURI().find_last_of(".");
    if (pos == std::string::npos)
        return "text/html";
    extension = std::string(request.getURI(), pos + 1);
    if (extension == "html" || extension == "htm")
        contentType = "text/html";
    else if (extension == "jpg" || extension == "jpeg")
        contentType = "image/jpeg";
    else if (extension == "png")
        contentType = "image/png";
    else if (extension == "gif")
        contentType = "image/gif";
    else if (extension == "css")
        contentType = "text/css";
    else if (extension == "php")
        contentType = "application/php";
    else if (extension == "txt")
        contentType = "text/plain";
    else if (extension == "js")
        contentType = "application/javascript";
    else if (extension == "json")
        contentType = "application/json";
    else if (extension == "xml")
        contentType = "application/xml";
    else if (extension == "pdf")
        contentType = "application/pdf";
    else if (extension == "zip")
        contentType = "application/zip";
    else
        contentType = "text/html";
    return contentType;
}

std::string     ResponseHTTP::defineContentLength( void )
{
    std::ifstream   file;
    std::string     contentLength;
    int             length;
    
    file.open(this->_path.c_str());
    if (file.is_open())
    {
        file.seekg(0, file.end);
        length = file.tellg();
        file.seekg(0, file.beg);
        file.close();
        contentLength = IntToStr(length);
    }
    else
        contentLength = "0";
    return contentLength;
}

std::string     ResponseHTTP::generateBody( void )
{
    // First we check if there is an error code.
    if (this->_statusCode != ResponseHTTP::OK)
        return ResponseHTTP::generateErrorBody();
    else if (this->_statusCode == ResponseHTTP::OK && this->_location.autoindex == true)
        return ResponseHTTP::generateAutoIndexBody();
    else
        return ResponseHTTP::generateFileBody();
}

std::string     ResponseHTTP::generateErrorBody( void )
{
    std::string     body;
    
    body = "<html><head><title>" + this->_statusPhrase + "</title></head><body><h1>" + this->_statusPhrase + "</h1></body></html>";
    return body;
}

std::string     ResponseHTTP::generateFileBody( void )
{
    std::ifstream   file;
    std::string     body;
    std::string     line;
    
    file.open(this->_path.c_str());
    if (file.is_open())
    {
        while (getline(file, line))
            body += line;
        file.close();
    }
    return body;
}

std::string     ResponseHTTP::generateAutoIndexBody( void )
{
    std::string     body;
    DIR             *dir;
    struct dirent   *ent;
    std::string     path;
    
    body = "<html><head><title>Index of " + this->_path + "</title></head><body><h1>Index of " + this->_path + "</h1><ul>";
    if ((dir = opendir(this->_path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] != '.')
            {
                path = this->_path + ent->d_name;
                if ( checkPath(path) == 1 )
                    body += "<li><a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "/</a></li>";
                else
                    body += "<li><a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a></li>";
            }
        }
        closedir(dir);
    }
    body += "</ul></body></html>";
    return body;
}

// This function will get the content type from the RequestHTTP and store it in the headers3
void        ResponseHTTP::defineLocation(const RequestHTTP request, const t_server server)
{
    // Check if the URI is in the server's location
    for (std::vector<t_location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++)
    {
        if ( request.getURI() == it->path )
        {
            this->_location = *it;
            return;
        }
    }
    // Check if the URI is in the server's location with a path, it should check all locations and return the more complete path
    for (std::vector<t_location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++)
    {
        if ( request.getURI().find(it->path) != std::string::npos )
        {
            if (it->path.length() > this->_location.path.length())
                this->_location = *it;
        }
    }
    // If no location is found, check if the default_server t_location works with this uri
    if (server.default_serv.path.length() > this->_location.path.length())
        this->_location = server.default_serv;
    // If no location works and the default_server t_location doesn't work, return a 404
    if (this->_location.path == "")
        this->_statusCode = ResponseHTTP::NOT_FOUND;
}

void    ResponseHTTP::methodDispatch(RequestHTTP request, t_server server)
{
    if (request.getMethod() == "GET")
        this->getMethodCheck(request, server);
    else if (request.getMethod() == "POST")
        this->postMethodCheck(request, server);
    else if (request.getMethod() == "DELETE")
        this->deleteMethodCheck(request, server);
    else
        ResponseHTTP::buildResponse(ResponseHTTP::METHOD_NOT_ALLOWED, ResponseHTTP::generateStatusLine(ResponseHTTP::METHOD_NOT_ALLOWED), request);
}

// GET
// The getMethodCheck(request, server) function will check for every possible error that can occur with a GET request.
// To do so, it will check the t_server configuration and the std::vector<t_location> location inside of it.
// It will then change the StatusCode _statusCode accordingly.

void        ResponseHTTP::getMethodCheck(RequestHTTP request, t_server server)
{
    //
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