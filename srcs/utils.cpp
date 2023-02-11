# include "utils.hpp"

// This function will get the content type from the RequestHTTP and store it in the headers3
t_location        defineLocation(const RequestHTTP request, const t_server server)
{
    t_location    _location;
    // Check if the URI is in the server's location
    for (std::vector<t_location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++)
        if ( request.getURI() == it->path ) {
            _location = *it;
            return (_location);
        }
    // Check if the URI is in the server's location with a path, it should check all locations and return the more complete path
    for (std::vector<t_location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++)
        if ( request.getURI().find(it->path) != std::string::npos ) {
            if (it->path.length() > _location.path.length())
                _location = *it;
        }
    // If no location is found, check if the default_server t_location works with this uri
    if (server.default_serv.path.length() > _location.path.length())
        _location = server.default_serv;
    return (_location);
}

std::string getIP(int client_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char ip[16];
    getsockname(client_fd, (struct sockaddr *)&client_addr, &addr_len);
    strncpy(ip, inet_ntoa(client_addr.sin_addr), 16);
    return (ip);
}

size_t getMaxBodySize(RequestHTTP request, t_location location, t_server server) {
    //We check if there is a limit on the body size in the location.
    if (location.client_body_size != -1 && location.client_body_size != 0)
        return (location.client_body_size);
    if (location.client_body_size == 0)
        return (0);
    if (location.client_body_size == -1)
        return (std::numeric_limits<size_t>::max());
    //If there is no limit in the location, we check if there is a limit in the server.
    if (server.default_serv.client_body_size != 0)
        return (server.default_serv.client_body_size);
    if (server.default_serv.client_body_size == 0)
        return (0);
    //If there is no limit in the server, we check if there is a limit in the request.
    if (request.getContentLength() != 0)
        return (request.getContentLength());
    return (0);
}

int     checkMaxBodySize( int valread, t_server server, RequestHTTP const &request ) {
    size_t maxBodySize = getMaxBodySize(request, defineLocation(request, server), server);
    if (valread > 0 && (maxBodySize != 0 && request.getBody().length() > maxBodySize))
    {
        //std::cerr << "MAX BODY SIZE DETECTED!" << std::endl;
        return (1);
    }
    return (0);
}

std::string formatRequestURI(const std::string &uri) {
    std::string formattedURI = uri;
    //Check for leading slash and add it if it's missing.
    if (formattedURI[0] != '/')
        formattedURI = "/" + formattedURI;
    //Check for double slashes and remove them.
    for (size_t i = 0; i < formattedURI.size() - 1; i++)
        if (formattedURI[i] == '/' && formattedURI[i + 1] == '/') {
            formattedURI.erase(i, 1);
            i--;
        }
    //Check for query string and remove it.
    size_t pos = formattedURI.find('?');
    if (pos != std::string::npos)
        formattedURI.erase(pos);
    return formattedURI;
}

int     checkPath(std::string path) {
    struct stat buf;
    if (stat(path.c_str(), &buf) == 0) 
    {
        /*first we check if the extension is allowed in our web server.
         std::string extension = path.substr(path.find_last_of(".") + 1);
         if (extension != "html" && extension != "htm" && extension != "jpg" && extension != "jpeg" \
             && extension != "png" && extension != "gif" && extension != "css" && extension != "map" && extension != "js"  \
             && extension != "txt" && extension != "php" && extension != "json" && extension != "ico")
             return (0);*/
        //path exists is a file and readble
        if (S_ISREG(buf.st_mode) && (buf.st_mode & S_IRUSR))
            return (1);
        //path exists is a directory and readble
        else if (S_ISDIR(buf.st_mode) && (buf.st_mode & S_IRUSR))
            return (2);
        //path exists but is not a file or a directory
        else
            return (3);
    }
    return (0);
}

std::string readFile(std::string path) {
    std::string ret;
    std::ifstream file(path.c_str());
    if (file.is_open()) {
        std::string line;
        while (getline(file, line))
            ret += line;
        file.close();
    }
    return ret;
}

bool    isReadable(std::string path) {
    struct stat buf;
    stat(path.c_str(), &buf);
    return (buf.st_mode & S_IRUSR);
}

bool    isDirectory(std::string path) {
    struct stat buf;
    stat(path.c_str(), &buf);
    return S_ISDIR(buf.st_mode);
}

bool    fileExists(std::string path) {
    struct stat buf;
    return (stat(path.c_str(), &buf) == 0);
}

std::string trim(const std::string &str)  {
    std::string whitespaces(" \t\r\n");
    int start = str.find_first_not_of(whitespaces);
    int end = str.find_last_not_of(whitespaces);
    if(start == -1 || end == -1)
        return "";
    else
        return str.substr(start, end-start+1);
}


void    split(const std::string& s, char delim, std::vector<std::string>& parts) {
    size_t start = 0, end = 0;
    while ((end = s.find(delim, start)) != std::string::npos) 
    {
        parts.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    parts.push_back(s.substr(start));
}

std::string const                  IntToStr( int nbr ) {
    std::stringstream ss;
    ss << nbr;
    return ss.str();
}

std::string const                   SizeToStr( size_t nbr ) {
    std::ostringstream oss;
    oss << nbr;
    return oss.str();
}

size_t                              StrToSize( std::string const &str ) {
    size_t nbr = static_cast<size_t>(std::atol(str.c_str()));
    return (nbr);
}

std::vector<std::string>            LineToWords ( std::string str ) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find_first_of(" \t");
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty())
            tokens.push_back(token);
        start = str.find_first_not_of(" \t", end);
        end = str.find_first_of(" \t", start);
    }
    std::string token = str.substr(start, end);
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}

static volatile sig_atomic_t got_signal = 0;

void    ctrlc_normal(int num)
{
    (void)num;
    printf("Close Webserv\n");
    got_signal = 1;
}

static void sig_err(void)
{
    exit(0);
}

void    signal_handler(void)
{
    if (signal(SIGINT, ctrlc_normal) == SIG_ERR)
        sig_err();
}