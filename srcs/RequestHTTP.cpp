# include "RequestHTTP.hpp"
/*
** Constructors and destructor.
*/



RequestHTTP::RequestHTTP() :  headers_received(0), is_complete(false), _method(UNKNOWN), _uri(""), _path("") {}

RequestHTTP::RequestHTTP(const std::string& request) : is_complete(false), _method(UNKNOWN), _uri(""), _version("")
{
    this->parseRequest(request);
    this->_client_fd = -1;
    this->_cgi_info["PATH_INFO"] = "";
}

RequestHTTP::RequestHTTP(const RequestHTTP &src)
{
    *this = src;
}

RequestHTTP::~RequestHTTP()
{
    return ;
}

/*
** Operators Overload
*/
void    RequestHTTP::reinit()
{
    headers_received = 0;
    is_complete = false;
    _method = UNKNOWN;
    _uri = "";
    _path = "";
    _cgi_info.clear();
    _headers.clear();
    _body.clear();
    _version.clear();
    _client_fd = -1;
    bytes_read = 0;

}
RequestHTTP &RequestHTTP::operator=(const RequestHTTP &rhs)
{
    if (this != &rhs){
        this->is_complete = rhs.is_complete;
        this->_method = rhs._method;
        this->is_complete = rhs.is_complete;
        this->_uri = rhs._uri;
        this->_version = rhs._version;
        this->_headers = rhs._headers;
        this->_body = rhs._body;
        this->_path = rhs._path;
        this->_client_fd = rhs._client_fd;
        this->_cgi_info = rhs._cgi_info;
    }
    return *this;
}

std::ostream    &operator<<(std::ostream &o, const RequestHTTP &i)
{
    o << "Method: " << i.getMethodString() << std::endl;
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

RequestHTTP::Method RequestHTTP::getMethod() const
{
    switch (this->_method)
    {
        case GET:
            return GET;
        case POST:
            return POST;
        case DELETE:
            return DELETE;
        case PUT:
            return PUT;
        case HEAD:
            return HEAD;
        case OPTIONS:
            return OPTIONS;
        case TRACE:
            return TRACE;
        case CONNECT:
            return CONNECT;
        case PATCH:
            return PATCH;
        default:
            return UNKNOWN;
    }
}

std::string RequestHTTP::getMethodString() const
{
    switch (this->_method)
    {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case DELETE:
            return "DELETE";
        case PUT:
            return "PUT";
        case HEAD:
            return "HEAD";
        case OPTIONS:
            return "OPTIONS";
        case TRACE:
            return "TRACE";
        case CONNECT:
            return "CONNECT";
        case PATCH:
            return "PATCH";
        default:
            return "POST";
    }
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
    return "";
}

std::string RequestHTTP::getPath()
{
    unsigned long i = _path.find_first_of("?", 0);
    if (i == std::string::npos)
        return _path;
    if ((int)i == -1)
        i = _path.length();
    return _path.substr(0, i);
}

std::string RequestHTTP::getQuery()
{
    unsigned long i = _path.find_first_of("?", 0);
    if (i == std::string::npos)
        return "";
    return _path.substr(i + 1, _path.size() - i);
}

std::string RequestHTTP::getCgi_info(std::string &extension)
{
    for (std::map<std::string, std::string>::const_iterator it = this->_cgi_info.begin(); it != this->_cgi_info.end(); ++it)
    {
        if (it->first == "." + extension)
            return it->second;
    }
    return "";
}

int RequestHTTP::getClient_fd()
{
    return _client_fd;
}

std::string RequestHTTP::getPort(){
    int i = _headers["Host"].find_first_of(":", 0);

    return _headers["Host"].substr(i + 1, _headers["Host"].size() - i - 1);
}

size_t RequestHTTP::getContentLength() const{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Length");
    if (it != this->_headers.end())
        return StrToSize(it->second.c_str());
    return 0;
}

std::string RequestHTTP::getContentType() const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Type");
    std::map<std::string, std::string>::const_iterator ite = this->_headers.end();

    if (it != ite)
        return it->second;
    else 
        return "";
}


bool   RequestHTTP::isComplete()
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find("Content-Length");
    std::map<std::string, std::string>::const_iterator ite = this->_headers.end();


    /* we find the content length*/
    if (is_complete == true)
        return true ;
    if (it != ite){
        size_t contentLength = atoi(this ->_headers.find("Content-Length")->second.c_str());
        if (getContentType()  == "multipart/form-data")
            contentLength--;
        if (contentLength <= (this->_body.size() * sizeof(std::string::value_type)) || contentLength == (this->_body.size() * sizeof(std::string::value_type)) - 1)
        {
            this->is_complete = true;
            return true;
        }
        else
        {
    
            return false;
        }
    }
    else if (headers_received == true)
    {
        this->is_complete = true;
        return true;    
    }
    else
        return false;
}

/*
**  Public Methods
*/
void    RequestHTTP::appendBody(const std::string& body){
    this->_body += body;
}



/*
**  Private Methods
*/

bool    RequestHTTP::parseHeaders( std::vector<std::string> &headers ){
    if (headers.empty())
        return false;
    //Now we are going to parse the std::vector<std::string> &headers and put the key and value in the map.
    for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string header = *it;
        size_t pos = header.find(':');
        if (pos == std::string::npos) 
            return false;
        std::string key = header.substr(0, pos);
        std::string value = header.substr(pos + 1);
        key = trim(key);
        value = trim(value);
        if (key.empty() || value.empty())
            return false;
        _headers[key] = value;
    }
    return true;
}

void    RequestHTTP::parseRequest(const std::string &request)
{
    if (request.empty() || request == "\r\n")
    {
        _uri = "BAD_REQUEST";
        is_complete = 1;
        return ;
    }
    std::vector<std::string> lines;
    split(request, '\n', lines);
    std::vector<std::string> requestLine;
    split(lines[0], ' ', requestLine);
    if (requestLine.size() != 3)
    {
        _uri = "BAD_REQUEST";
        is_complete = 1;
        return ;
    }
    if (requestLine[0] == "GET")
        _method = GET;
    else if (requestLine[0] == "POST")
        _method = POST;
    else if (requestLine[0] == "DELETE")
        _method = DELETE;
    else if (requestLine[0] == "PUT")
        _method = PUT;
    else if (requestLine[0] == "HEAD")
        _method = HEAD;
    else if (requestLine[0] == "OPTIONS")
        _method = OPTIONS;
    else if (requestLine[0] == "TRACE")
        _method = TRACE;
    else if (requestLine[0] == "CONNECT")
        _method = CONNECT;
    else if (requestLine[0] == "PATCH")
        _method = PATCH;
    else
        _method = UNKNOWN;

    _uri = formatRequestURI(requestLine[1]);
    _version = requestLine[2];
    if (_version != "HTTP/1.1\r" && _version != "HTTP/1.1")
    {
        _uri = "BAD_VERSION";
        is_complete = 1;
        return ;
    }
    std::vector<std::string> headerLines;
    for (size_t i = 1; i < lines.size(); i++) {
        if (lines[i] == "\r")
        {
            headers_received = 1;
            break ;
        }
        if (lines[i].empty() )
            break;
        headerLines.push_back(lines[i]);
    }
    if (parseHeaders(headerLines) == false)
    {
        _uri = "BAD_REQUEST";
        is_complete = 1;
        return ;
    }
    size_t npos;
    for (size_t i = headerLines.size() + 2; i < lines.size(); i++)
    {
        if ((npos = lines[i].find("\r")) != std::string::npos)
        {
            lines[i].replace(npos, 1, "\r\n");
        }
        _body += lines[i];
    }
    if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked")
        _body = unchunk(_body);
}

std::string RequestHTTP::unchunk(std::string &chunked)
{
    std::string unchunked;
    size_t pos = 0;
    size_t size = 0;
    size_t npos = 0;
    while ((npos = chunked.find("\r\n", pos)) != std::string::npos)
    {
        std::istringstream ss(chunked.substr(pos, npos - pos));
        ss >> std::hex >> size;
        if (size == 0)
            break;
        unchunked += chunked.substr(npos + 2, size);
        pos = npos + 2 + size + 2;
    }
    return unchunked;
}