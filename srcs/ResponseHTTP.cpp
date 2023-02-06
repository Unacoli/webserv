#include "ResponseHTTP.hpp"

/*
** Constructors and Destructor
*/

ResponseHTTP::ResponseHTTP() : _statusCode(OK), _statusPhrase("OK"), _headers(), _content_type(), _body(""), _path(""), _response("") {}

ResponseHTTP::ResponseHTTP( ResponseHTTP const &src ) 
{
    *this = src;
}

ResponseHTTP::ResponseHTTP( const RequestHTTP& request, const t_server server) 
{
    this->_default_serv = server.default_serv;
    this->_statusCode = ResponseHTTP::OK;
    defineLocation(request, server);
    generateResponse(request, server);
}

ResponseHTTP::~ResponseHTTP(){}

void    ResponseHTTP::sendError(StatusCode statusCode)
{
    this->_statusCode = statusCode;
    this->_statusPhrase = generateStatusLine(statusCode);
    this->_headers["Date"] = generateDate();
    this->_headers["Content-Type"] = "text/html";
    this->_headers["Connection"] = "close";
    this->_body = generateErrorBody();
    this->_headers["Content-Length"] = SizeToStr(this->_body.size());
    this->_cgiExecutable = "";
    ResponseHTTP::responseMaker();
}

/*
** Operators overload
*/
ResponseHTTP &ResponseHTTP::operator=(const ResponseHTTP &rhs)
{
    if (this != &rhs) {
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
    //o << "Body: " << i.getBody() << std::endl;
    o << "Headers: " << i.getHeaders() << std::endl;
    return o;
}

/*
** Getters
*/
ResponseHTTP::StatusCode  ResponseHTTP::getStatusCode() const
{
    return this->_statusCode;
}

int        ResponseHTTP::getStatusCodeInt() const
{
    // We convert it to int to be able to find the good error_page in the config file.
    return static_cast<int>(this->_statusCode);
}

std::string         ResponseHTTP::getStatusPhrase() const
{
    return this->_statusPhrase;
}

std::string         ResponseHTTP::getHeaders() const
{
    std::string headers = "";

    for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
        headers += it->first + ": " + it->second + "\n";
    return headers;   
}

std::string         ResponseHTTP::getHeader( const std::string &name ) const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(name);
    if (it != this->_headers.end())
        return it->second;
    return "";
}

std::string         ResponseHTTP::getPath() const
{
    return this->_path;
}

std::string         ResponseHTTP::getBody() const
{
    return this->_body;
}

std::string         ResponseHTTP::getResponse() const
{
    return this->_response;
}

size_t              ResponseHTTP::getContentLength() const 
{
    return this->_body.size();
}

std::string         ResponseHTTP::getCgiExecutable() const 
{
    return this->_cgiExecutable;
}

/*
** Public Methods
*/
void                ResponseHTTP::appendHeader(std::string first, std::string second)
{
    _headers.insert(std::make_pair(first, second));
}

void                ResponseHTTP::appendBody( const std::string &body )
{
    this->_body += body;
}

void                ResponseHTTP::setResponse( const std::string &response )
{
    this->_response = response;
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
    path = checkRedirection(path);
    checkedPath = checkPath(path);
    if ( checkedPath == 2 )
    {
        //This means that the path is a directory.
        if ( _location.index.size() > 0 && _location.autoindex == false)
        {
            for (std::vector<std::string>::const_iterator it = _location.index.begin(); it != _location.index.end(); it++)
            {
                if ( checkPath(path + "/" + *it) == 1 )
                {
                    if ( path[path.size() - 1] == '/')
                        path += *it;
                    else
                        path += "/" + *it;
                    break;
                }
            }
        }
        else if ( _location.autoindex == true )
        {
            this->_path = path;
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
            return ;
        }
        else
        {
            sendError(FORBIDDEN);
            return ;
        }
    }
    if ( checkedPath == 0 )
    {
        //This means that the path is not a directory or a file.
        sendError(NOT_FOUND);
        return ;
    }
    this->_path = path;
    ResponseHTTP::methodDispatch(request);
    return ;
}

void        ResponseHTTP::buildResponse( const ResponseHTTP::StatusCode &code, const std::string &statusLine, const RequestHTTP &request)
{
    this->_statusCode = code;
    this->_statusPhrase = statusLine;
    this->_headers["Date"] = ResponseHTTP::generateDate();
    this->_headers["Server"] = "Webserv/1.0";
    this->_headers["Content-Type"] = ResponseHTTP::defineContentType(request);
    this->_headers["Connection"] = ResponseHTTP::defineConnection(request);
    this->_body = ResponseHTTP::generateBody();
    this->_headers["Content-Length"] = ResponseHTTP::defineContentLength();
    ResponseHTTP::responseMaker();
}

void        ResponseHTTP::responseMaker( void ) 
{
    std::string     response;

    response = "HTTP/1.1 " + this->_statusPhrase;
    response += this->getHeaders();
    response += "\r\n";
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
    else if (extension == "ico")
        contentType = "image/x-icon";
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
    return (IntToStr(this->_body.length()));
}

std::string     ResponseHTTP::generateBody( void ) 
{   
    if (this->_location.autoindex == true || (this->_default_serv.autoindex == true && this->_location.autoindex != false))
        return ResponseHTTP::generateAutoIndexBody();
    else
        return ResponseHTTP::generateFileBody();
}

std::string     ResponseHTTP::generateErrorBody( void ) 
{
    std::string     errorPage;
    int error = atoi(this->_statusPhrase.substr(0, 3).c_str());
    
    switch (error)
    {
        case 300:
            errorPage = "errors/error300.html";
            break;
        case 301:
            errorPage = "errors/error301.html";
            break;
        case 302:
            errorPage = "errors/error302.html";
            break;
        case 303:
            errorPage = "errors/error303.html";
            break;
        case 304:
            errorPage = "errors/error304.html";
            break;
        case 400:
            errorPage = "errors/error400.html";
            break;
        case 401:
            errorPage = "errors/error401.html";
            break;
        case 403:
            errorPage = "errors/error403.html";
            break;
        case 404:
            errorPage = "errors/error404.html";
            break;
        case 405:
            errorPage = "errors/error405.html";
            break;
        case 409:
            errorPage = "errors/error409.html";
            break;
        case 410:
            errorPage = "errors/error410.html";
            break;
        case 413:
            errorPage = "errors/error413.html";
            break;
        case 500:
            errorPage = "errors/error500.html";
            break;
        case 501:
            errorPage = "errors/error501.html";
            break;
        case 503:
            errorPage = "errors/error503.html";
            break;
        case 504:
            errorPage = "errors/error504.html";
            break;
        default:
            errorPage = "errors/error.html";
            break;
    }

    if (this->_location.root != "")
        this->_path = this->_default_serv.root + errorPage;
    this->_headers["Connection"] = "close";
    this->_headers["Content-Type"] = "text/html";
    return (ResponseHTTP::generateFileBody());
}

std::string     ResponseHTTP::generateFileBody( void ) 
{
    std::ifstream   file;
    std::string     body;
    std::string     line;

    if (this->_headers["Content-Type"] == "text/html" || this->_headers["Content-Type"] == "text/css" \
        || this->_headers["Content-Type"] == "text/plain")
    {
        file.open(this->_path.c_str());
        if (file.is_open()) {
            while (getline(file, line))
                body += line;
            file.close();
        }
    }
    else 
    {
        std::ifstream ifs(this->_path.c_str(), std::ios::binary | std::ios::ate);
        if (ifs.fail()) 
            return body;
        std::ifstream::pos_type pos = ifs.tellg();
        std::vector<char>  result(pos);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], pos);
        body = std::string(result.begin(), result.end());
        ifs.close();
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
    if ((dir = opendir(this->_path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
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

void        ResponseHTTP::defineLocation(const RequestHTTP request, const t_server server) {
    // Check if the URI is in the server's location
    for (std::vector<t_location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++)
        if ( request.getURI() == it->path ) {
            this->_location = *it;
            return;
        }
    // Check if the URI is in the server's location with a path, it should check all locations and return the more complete path
    for (std::vector<t_location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++)
        if ( request.getURI().find(it->path) != std::string::npos )
            if (it->path.length() > this->_location.path.length())
                this->_location = *it;
    // If no location is found, check if the default_server t_location works with this uri
    if (server.default_serv.path.length() > this->_location.path.length())
        this->_location = server.default_serv;
    // If no location works and the default_server t_location doesn't work, return a 404
    if (this->_location.path == "")
        this->_statusCode = ResponseHTTP::NOT_FOUND;
}

void    ResponseHTTP::methodDispatch(RequestHTTP request) {
    RequestHTTP::Method method = request.getMethod();

    switch (method) {
        case RequestHTTP::GET:
            ResponseHTTP::getMethodCheck(request);
            break;
        case RequestHTTP::POST:
            ResponseHTTP::postMethodCheck(request);
            break;
        case RequestHTTP::DELETE:
            ResponseHTTP::deleteMethodCheck(request);
            break;
        case RequestHTTP::PUT:
            sendError(ResponseHTTP::NOT_IMPLEMENTED);
            break;
        case RequestHTTP::HEAD:
            sendError(ResponseHTTP::NOT_IMPLEMENTED);
            break;
        case RequestHTTP::OPTIONS:
            sendError(ResponseHTTP::NOT_IMPLEMENTED);
            break;
        case RequestHTTP::TRACE:
            sendError(ResponseHTTP::NOT_IMPLEMENTED);
            break;
        case RequestHTTP::CONNECT:
            sendError(ResponseHTTP::NOT_IMPLEMENTED);
            break;
        default:
            sendError(ResponseHTTP::NOT_IMPLEMENTED);
            break;
    }
}

// GET
// The getMethodCheck(request, server) function will check for every possible error that can occur with a GET request.
// To do so, it will check the t_server configuration and the std::vector<t_location> location inside of it.
// It will then change the StatusCode _statusCode accordingly.

void        ResponseHTTP::getMethodCheck(const RequestHTTP &request)
{
    // Try to get the path given in this->_path
    // Then we call buildResponse with the appropriate status code
    // If the path is not found, we return a 404
    // If the path is a directory, we return a 403
    // If the path is a file, we return a 200
    // If the path is a directory and autoindex is on, we return a 200
    std::fstream    file;
    std::string     path;
    int             check;

    path = this->_path;
    check = checkPath(path);
    if (check == 0)
        sendError(NOT_FOUND);
    else if (check == 3)
        sendError(FORBIDDEN);
    else if (check == 2) {
        if (this->_location.autoindex == false)
            sendError(FORBIDDEN);
        else if (this->_location.autoindex == true)
        {
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
        else if (this->_default_serv.autoindex == true && this->_location.autoindex != false)
        {
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
        else
            sendError(FORBIDDEN);
    }
    else {
        //we check if we need to call a cgi script or not
        if (path.find(".php") != std::string::npos) {
            //We check if this extension is allowed in the map
            for (std::map<std::string, std::string>::const_iterator it = this->_location.cgi.begin(); it != this->_location.cgi.end(); it++){
                if (it->first.find("php") != std::string::npos){
                    this->_cgiExecutable = it->second;
                    break;
                }
            }
            //we check if the cgi script is executable
            if (access(path.c_str(), X_OK) == -1 || this->_cgiExecutable == "" || access(this->_cgiExecutable.c_str(), X_OK) == -1)
                sendError(FORBIDDEN);
            else
            {
                Cgi cgi(request, this);
                std::string cgiResponse = getResponse();
                std::string body = cgiResponse.substr(cgiResponse.find("\r\n\r\n") + 4);
                std::string headers = "HTTP/1.1 " + ResponseHTTP::generateStatusLine(ResponseHTTP::OK) + "\r\n" \
                + "Connexion: close\r\n" + "Content-Length: "+ SizeToStr(body.length()) + "\r\n";
                cgiResponse = headers + cgiResponse;
                setResponse(cgiResponse);
            }
        }
        else
        {
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
    }
}

// POST
// The postMethodCheck(request, server) function will check for every possible error that can occur with a POST request.
// To do so, it will check the std::vector<t_location> _location and the t_location _default_serv.
// It will then change the StatusCode _statusCode accordingly.
// If the path is not found, we return a 404
void        ResponseHTTP::postMethodCheck(RequestHTTP request) 
{
    std::fstream    file;
    std::string     path;
    int             check;

    path = this->_path;
    check = checkPath(path);
    if (check == 0)
        sendError(ResponseHTTP::NOT_FOUND);
    else if ( check == 1 ){
        // We check if we should call a cgi script or not.
        if ( path.find(".php") != std::string::npos ){
            for (std::map<std::string, std::string>::const_iterator it = this->_location.cgi.begin(); it != this->_location.cgi.end(); it++){
                if (it->first.find("php") != std::string::npos){
                    this->_cgiExecutable = it->second;
                    break;
                }
            }
            //we check if the cgi script is executable
            if (access(path.c_str(), X_OK) == -1 || this->_cgiExecutable == "" || access(this->_cgiExecutable.c_str(), X_OK) == -1)
                sendError(FORBIDDEN);
            else
            {
                Cgi cgi(request, this);
                std::string cgiResponse = getResponse();
                std::string body = cgiResponse.substr(cgiResponse.find("\r\n\r\n") + 4);
                std::string headers = "HTTP/1.1 " + ResponseHTTP::generateStatusLine(ResponseHTTP::OK) + "\r\n" \
                + "Connexion: keep\r\n" + "Content-Length: "+ SizeToStr(body.length()) + "\r\n";
                cgiResponse = headers + cgiResponse;
                setResponse(cgiResponse);
            }
        }
        // We check if we should overwrite the file or not.
        else if ( _location.client_body_append == true ){
            // We append the file
            file.open(path.c_str(), std::ios::out | std::ios::app);
            if (file.is_open() == false || access(path.c_str(), W_OK ) == -1)
                sendError(FORBIDDEN);
            file << ResponseHTTP::handlingContentDisposition(request.getBody(), request);
            file << std::endl;
            file.close();
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
        else if ( _location.client_body_append == -1 && _default_serv.client_body_append == true){
            // We append the file
            file.open(path.c_str(), std::ios::out | std::ios::app);
            if (file.is_open() == false || access(path.c_str(), W_OK ) == -1)
                sendError(FORBIDDEN);
            file << ResponseHTTP::handlingContentDisposition(request.getBody(), request);
            file << std::endl;
            file.close();
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
        else{
            // We overwrite the file
            file.open(path.c_str(), std::ios::out | std::ios::trunc);
            if (file.is_open() == false || access(path.c_str(), W_OK ) == -1)
                sendError(FORBIDDEN);
            file << ResponseHTTP::handlingContentDisposition(request.getBody(), request);
            file.close();
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
    }
    else
        sendError(ResponseHTTP::NOT_FOUND);
}

// DELETE
// The deleteMethodCheck(request, server) function will check for every possible error that can occur with a DELETE request.
// To do so, it will check the std::vector<t_location> _location and the t_location _default_serv.
// It will then change the StatusCode _statusCode accordingly. If it is a success, typically a 204, we will delete the file.
// If the path is not found, we return a 404
void        ResponseHTTP::deleteMethodCheck(const RequestHTTP request)
{
    std::fstream    file;
    std::string     path;
    int             check;

    path = this->_path;
    check = checkPath(path);
    if (check == 0)
        sendError(ResponseHTTP::NOT_FOUND);
    //we call the CGI if the file extension is .php
    else if (check == 1 && path.find(".php") != std::string::npos){
        // We check if the script is executable and readable
        if (access(path.c_str(), X_OK ) == -1)
            sendError(ResponseHTTP::FORBIDDEN);
        else
        {
            if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
    }
    else if (check == 1) {
        // We check if the file is writable
        if (access(path.c_str(), W_OK) == -1)
            sendError(ResponseHTTP::FORBIDDEN);
        else {
            // We delete the file
            if (remove(path.c_str()) != 0)
                sendError(ResponseHTTP::FORBIDDEN);
            else
            {
                if ( _statusCode != ResponseHTTP::OK )
                ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
            else
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
            }
        }
    }
    else if (check == 2) {
        // We check if the directory is writable
        if (access(path.c_str(), W_OK) == -1)
            sendError(ResponseHTTP::FORBIDDEN);
        else {
            // We delete the directory
            if (remove(path.c_str()) != 0)
                sendError(ResponseHTTP::FORBIDDEN);
            else
            {
                if ( _statusCode != ResponseHTTP::OK )
                    ResponseHTTP::buildResponse(_statusCode, ResponseHTTP::generateStatusLine(_statusCode), request);
                else
                    ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
            }
        }
    }
    else
        sendError(ResponseHTTP::NOT_FOUND);
}

std::string ResponseHTTP::generateStatusLine(ResponseHTTP::StatusCode code)
{
    std::string statusLine = "";

    switch (code) {
        case ResponseHTTP::UNDEFINED:
            statusLine = "Undefined\r";
            break;
        case ResponseHTTP::OK:
            statusLine = "200 OK\r";
            break;
        case ResponseHTTP::CREATED:
            statusLine = "201 Created\r";
            break;
        case ResponseHTTP::NO_CONTENT:
            statusLine = "204 No Content\r";
            break;
        case ResponseHTTP::MULTIPLE_CHOICES:
            statusLine = "300 Multiple Choices\r";
            break;
        case ResponseHTTP::MOVED_PERMANENTLY:
            statusLine = "301 Moved Permanently\r";
            break;
        case ResponseHTTP::FOUND:
            statusLine = "302 Found\r";
            break;
        case ResponseHTTP::SEE_OTHER:
            statusLine = "303 See Other\r";
            break;
        case ResponseHTTP::NOT_MODIFIED:
            statusLine = "304 Not Modified\r";
            break;
        case ResponseHTTP::TEMPORARY_REDIRECT:
            statusLine = "307 Temporary Redirect\r";
            break;
        case ResponseHTTP::PERMANENT_REDIRECT:
            statusLine = "308 Permanent Redirect\r";
            break;
        case ResponseHTTP::BAD_REQUEST:
            statusLine = "400 Bad Request\r";
            break;
        case ResponseHTTP::UNAUTHORIZED:
            statusLine = "401 Unauthorized\r";
            break;
        case ResponseHTTP::FORBIDDEN:
            statusLine = "403 Forbidden\r";
            break;
        case ResponseHTTP::NOT_FOUND:
            statusLine = "404 Not Found\r";
            break;
        case ResponseHTTP::METHOD_NOT_ALLOWED:
            statusLine = "405 Method Not Allowed\r";
            break;
        case ResponseHTTP::CONFLICT:
            statusLine = "409 Conflict\r";
            break;
        case ResponseHTTP::GONE:
            statusLine = "410 Gone\r";
            break;
        case ResponseHTTP::REQUEST_ENTITY_TOO_LARGE:
            statusLine = "413 Request Entity Too Large\r";
            break;
        case ResponseHTTP::INTERNAL_SERVER_ERROR:
            statusLine = "500 Internal Server Error\r";
            break;
        case ResponseHTTP::NOT_IMPLEMENTED:
            statusLine = "501 Not Implemented\r";
            break;
        case ResponseHTTP::SERVICE_UNAVAILABLE:
            statusLine = "503 Service Unavailable\r";
            break;
        case ResponseHTTP::GATEWAY_TIMEOUT:
            statusLine = "504 Gateway Timeout\r";
            break;
        case ResponseHTTP::HTTP_VERSION_NOT_SUPPORTED:
            statusLine = "505 HTTP Version Not Supported\r";
            break;
    }
    return statusLine;
}

void                ResponseHTTP::setStatusCode( int code )
{
    switch (code) {
        case 0:
            this->_statusCode = UNDEFINED;
            break;
        case 200:
            this->_statusCode = OK;
            break;
        case 201:
            this->_statusCode = CREATED;
            break;
        case 204:
            this->_statusCode = NO_CONTENT;
            break;
        case 300:
            this->_statusCode = MULTIPLE_CHOICES;
            break;
        case 301:
            this->_statusCode = MOVED_PERMANENTLY;
            break;
        case 302:
            this->_statusCode = FOUND;
            break;
        case 304:
            this->_statusCode = NOT_MODIFIED;
            break;
        case 307:
            this->_statusCode = TEMPORARY_REDIRECT;
            break;
        case 308:
            this->_statusCode = PERMANENT_REDIRECT;
            break;
        case 400:
            this->_statusCode = BAD_REQUEST;
            break;
        case 401:
            this->_statusCode = UNAUTHORIZED;
            break;
        case 403:
            this->_statusCode = FORBIDDEN;
            break;
        case 404:
            this->_statusCode = NOT_FOUND;
            break;
        case 405:
            this->_statusCode = METHOD_NOT_ALLOWED;
            break;
        case 409:
            this->_statusCode = CONFLICT;
            break;
        case 410:
            this->_statusCode = GONE;
            break;
        case 413:
            this->_statusCode = REQUEST_ENTITY_TOO_LARGE;
            break;
        case 500:
            this->_statusCode = INTERNAL_SERVER_ERROR;
            break;
        case 501:
            this->_statusCode = NOT_IMPLEMENTED;
            break;
        case 503:
            this->_statusCode = SERVICE_UNAVAILABLE;
            break;
        case 504:
            this->_statusCode = GATEWAY_TIMEOUT;
            break;
        case 505:
            this->_statusCode = HTTP_VERSION_NOT_SUPPORTED;
            break;
        default:
            this->_statusCode = INTERNAL_SERVER_ERROR;
            break;
    }
}

std::string         ResponseHTTP::defineConnection(const RequestHTTP &request) 
{
    std::string connection = "";

    if (request.getHeader("Connection") != "") {
        if (request.getHeader("Connection") == "keep-alive")
            connection = "keep-alive\r";
        else if (request.getHeader("Connection") == "close")
            connection = "close\r";
        else if (request.getHeader("Connection") == "upgrade")
            connection = "upgrade\r";
        else if (request.getHeader("Connection") == "keep-alive, upgrade")
            connection = "keep-alive, upgrade\r";
        else
            connection = "close\r";
    }
    else
        connection = "close\r";
    return connection;
}

// This function will be able to handle different Content-Disposition for a POST request.
// It will return a string of what should be written in the file.
std::string       ResponseHTTP::handlingContentDisposition(std::string const &body, RequestHTTP request) const
{
    std::string bodyToWrite;
    std::string bodyCopy;
    
    if (request.getHeader("Content-Type").find("multipart/form-data") != std::string::npos) {
        bodyCopy = body;
        while (bodyCopy.find("Content-Disposition: form-data; name=\"") != std::string::npos) {
            std::string name = body.substr(body.find("Content-Disposition: form-data; name=\"") + 38, body.find("\"", body.find("Content-Disposition: form-data; name=\"") + 38) - body.find("Content-Disposition: form-data; name=\"") - 38);
            std::string value = body.substr(body.find("\r") + 2, body.find("\r", body.find("\r") + 2) - body.find("\r") - 2);
            bodyToWrite += name + "=" + value + "\r";
            bodyCopy = body.substr(body.find("\r", body.find("\r") + 2) + 2);
        }
    }
    else
        bodyToWrite = body;
    return bodyToWrite;
}

std::string                ResponseHTTP::checkRedirection( std::string const &path )
{
    if ( _location.redirects.empty() != true )
    {
        std::string redirection = _location.redirects[0];
        std::string code = redirection.substr(0, redirection.find(" "));
        std::string new_path;

        redirection = redirection.substr(redirection.find(" ") + 1);
        new_path = _location.root + redirection.substr(redirection.find(_location.path) + 1);
        setStatusCode(atoi(code.c_str()));
        return (new_path);
    }
    return (path);
}