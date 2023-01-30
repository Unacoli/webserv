#include "ResponseHTTP.hpp"

/*
** Constructors and Destructor
*/

ResponseHTTP::ResponseHTTP() : _statusCode(OK), _statusPhrase("OK"), _headers(), _content_type(), _body(""), _path(""), _response("") {}

ResponseHTTP::ResponseHTTP(StatusCode statusCode)
{
    this->_statusCode = statusCode;
    this->_statusPhrase = statusCode;
}

ResponseHTTP::ResponseHTTP( ResponseHTTP const &src ) {
    *this = src;
}

ResponseHTTP::ResponseHTTP( const RequestHTTP& request, const t_server server)
{
    this->_default_serv = server.default_serv;
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
    this->_body = generateBody();
    this->_headers["Content-Length"] = SizeToStr(this->_body.size());
    ResponseHTTP::responseMaker();
}
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

int        ResponseHTTP::getStatusCodeInt() const {
    // We convert it to int to be able to find the good error_page in the config file.
    return static_cast<int>(this->_statusCode);
}

std::string         ResponseHTTP::getStatusPhrase() const {
    return this->_statusPhrase;
}

std::string         ResponseHTTP::getHeaders() const {
    std::string headers = "";
    for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
        headers += it->first + ": " + it->second + "\n";
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

std::string         ResponseHTTP::getResponse() const {
    return this->_response;
}

size_t              ResponseHTTP::getContentLength() const 
{
    return this->_body.size();
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
            ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
            return ;
        }
        else
        {
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
    // if ( 0 )
    // {
    //     //Here will take place the CGI test.
    // }
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
    // First we check if there is an error code.
    if (this->_statusCode != ResponseHTTP::OK)
    {   
        std::cerr << "Error code detected" << std::endl;
        return ResponseHTTP::generateErrorBody();
    }
    else if (this->_statusCode == ResponseHTTP::OK && (this->_location.autoindex == true \
                || (this->_default_serv.autoindex == true && this->_location.autoindex != false)))
    {    
        std::cerr << "Autoindex detected" << std::endl;
        return ResponseHTTP::generateAutoIndexBody();
    }
    else
    {
        std::cerr << "File detected" << std::endl;
        return ResponseHTTP::generateFileBody();
    }
}

std::string     ResponseHTTP::generateErrorBody( void )
{
    std::string     errorPage;
    // Here we will have to change the path to the error page and return generateFileBody()
    // First we check if the error page has been defined in the location block or in the server block.
    // If not we will use the default error pages.
    int error = atoi(this->_statusPhrase.substr(0, 3).c_str());
    if (this->_location.errors.find(error) != this->_location.errors.end())
        errorPage = this->_location.errors[this->_statusCode];
    else if (this->_default_serv.errors.find(error) != this->_default_serv.errors.end())
        errorPage = this->_default_serv.errors[error];
    else if (error == 404)
        errorPage = "../errors/error404.html";
    else if (error == 403)
        errorPage = "../errors/error403.html";
    else
        errorPage = "../errors/error.html";
    //Now we add the right path to the error page, we check if the location has a root or not.
    if (this->_location.root != "")
        this->_path = this->_default_serv.root + errorPage;
    std::cerr << "Error page path : " << this->_path << std::endl;
    return (ResponseHTTP::generateFileBody());
}

std::string     ResponseHTTP::generateFileBody( void )
{
    std::ifstream   file;
    std::string     body;
    std::string     line;
    // We do not apply the same method depending of the type of file.
    if (this->_headers["Content-Type"] == "text/html" || this->_headers["Content-Type"] == "text/css" || this->_headers["Content-Type"] == "text/plain")
    {
        file.open(this->_path.c_str());
        if (file.is_open())
        {
            while (getline(file, line))
                body += line;
            file.close();
        }
    }
    else if (this->_headers["Content-Type"] == "application/php")
    {
        // We will have to execute the php file and get the result.
    }
    else
    {
        // Translates the binary file into a string that can be put in the body.
        std::ifstream ifs(this->_path.c_str(), std::ios::binary | std::ios::ate);
        // If it fails we return an empty body.
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

void    ResponseHTTP::methodDispatch(RequestHTTP request)
{
     if (request.getMethod() == "GET")
         this->getMethodCheck(request);
     else if (request.getMethod() == "POST")
         this->postMethodCheck(request);
    // // else if (request.getMethod() == "DELETE")
    // //     this->deleteMethodCheck(request, server);
    else
        ResponseHTTP::buildResponse(ResponseHTTP::METHOD_NOT_ALLOWED, ResponseHTTP::generateStatusLine(ResponseHTTP::METHOD_NOT_ALLOWED), request);
}

// GET
// The getMethodCheck(request, server) function will check for every possible error that can occur with a GET request.
// To do so, it will check the t_server configuration and the std::vector<t_location> location inside of it.
// It will then change the StatusCode _statusCode accordingly.

void        ResponseHTTP::getMethodCheck(RequestHTTP request)
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
        ResponseHTTP::buildResponse(ResponseHTTP::NOT_FOUND, ResponseHTTP::generateStatusLine(ResponseHTTP::NOT_FOUND), request);
    else if (check == 3)
        ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
    else if (check == 2)
    {
        if (this->_location.autoindex == false)
            ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
        else if (this->_location.autoindex == true)
            ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        else if (this->_default_serv.autoindex == true && this->_location.autoindex != false)
            ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        else
            ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
    }
    else if (check == 1)
        ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
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
    else if ( check == 1 )
    {
        // We check if we should call a cgi script or not.
        if ( path.find(".php") != std::string::npos )
        {
            // We call the cgi script
            // We check if the script is executable
            if (access(path.c_str(), X_OK) == -1)
                ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
            // We check if the script is readable
            else if (access(path.c_str(), R_OK) == -1)
                ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
            else
            {
                // We call the cgi script
                ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
            }
        }
        // We check if we should overwrite the file or not.
        else if ( _location.client_body_append == true )
        {
            // We append the file
            file.open(path.c_str(), std::ios::out | std::ios::ate);
            if (file.is_open() == false)
                ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
            file << ResponseHTTP::handlingContentDisposition(request.getBody(), request);
            file.close();
            ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
        else if ( _location.client_body_append == -1 && _default_serv.client_body_append == true)
        {
            // We append the file
            file.open(path.c_str(), std::ios::out | std::ios::ate);
            if (file.is_open() == false)
                ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
            file << ResponseHTTP::handlingContentDisposition(request.getBody(), request);
            file.close();
            ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
        else
        {
            // We overwrite the file
            file.open(path.c_str(), std::ios::out | std::ios::trunc);
            if (file.is_open() == false)
                ResponseHTTP::buildResponse(ResponseHTTP::FORBIDDEN, ResponseHTTP::generateStatusLine(ResponseHTTP::FORBIDDEN), request);
            file << ResponseHTTP::handlingContentDisposition(request.getBody(), request);
            file.close();
            ResponseHTTP::buildResponse(ResponseHTTP::OK, ResponseHTTP::generateStatusLine(ResponseHTTP::OK), request);
        }
    }
    else
        sendError(ResponseHTTP::NOT_FOUND);
}

std::string ResponseHTTP::generateStatusLine(ResponseHTTP::StatusCode code)
{
    std::string statusLine = "";
    switch (code)
    {
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
    }
    return statusLine;
}

std::string         ResponseHTTP::defineConnection(const RequestHTTP &request)
{
    std::string connection = "";
    if (request.getHeader("Connection") != "")
    {
        if (request.getHeader("Connection") == "keep-alive")
            connection = "keep-alive\r";
        else if (request.getHeader("Connection") == "close")
            connection = "close\r";
    }
    else
        connection = "close\r";
    std::cerr << "Connection: " << connection << std::endl;
    return connection;
}

// This function will be able to handle different Content-Disposition for a POST request.
// It will return a string of what should be written in the file.
std::string       ResponseHTTP::handlingContentDisposition(std::string const &body, RequestHTTP request) const
{
    std::string bodyToWrite;
    std::string bodyCopy;
    if (request.getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
    {
        bodyCopy = body;
        while (bodyCopy.find("Content-Disposition: form-data; name=\"") != std::string::npos)
        {
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
