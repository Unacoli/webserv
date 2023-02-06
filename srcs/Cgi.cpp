#include "Cgi.hpp"

#define CGI_RESSOURCES_BUFFER_SIZE 100
#define CGI_READ_BUFFER_SIZE 64000

static void kill_child_process(int sig)
{
    (void) sig;
    kill(-1, SIGKILL);
}

// Constructor

Cgi::Cgi(RequestHTTP RequestHTTP, ResponseHTTP *resp)
{
    //this->_env["AUTH_TYPE"] = "";
    this->_env["CONTENT_TYPE"] = RequestHTTP._headers["Content-Type"];
    //this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    // this->_env["PATH_INFO"] = findPathInfo(resp.getPath());
    //this->_env["PATH_TRANSLATED"] = resp->getPath();
    this->_env["QUERY_STRING"] = "";
    // this->_env["REMOTE_HOST"] = RequestHTTP._headers["Host"];
    // this->_env["REMOTE_ADDR"] = getIP(RequestHTTP.getClient_fd());
    // this->_env["REMOTE_USER"] = "";
    // this->_env["REMOTE_IDENT"] = "";
    this->_env["REQUEST_METHOD"] = RequestHTTP.getMethodString();
    // this->_env["REQUEST_URI"] = resp->getPath();
    // SCRIPT NAME is gonna be the relative path to the requested php script within the web document root
    // We must look at the current directory and the path of the requested file to get the relative path
    // this->_env["SCRIPT_NAME"] = getTarget_file_path(resp);
    this->_env["SCRIPT_FILENAME"] = resp->getPath();
    // this->_env["SERVER_NAME"] = RequestHTTP._headers["Host"];
    // this->_env["SERVER_PROTOCOL"] = RequestHTTP.getHTTPVersion();
    // this->_env["SERVER_PORT"] = RequestHTTP.getPort();
    // this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
    if (RequestHTTP.getHeader("Content-Length") != "")
        this->_env["CONTENT_LENGTH"] = RequestHTTP.getHeader("Content-Length");
    else
        this->_env["CONTENT_LENGTH"] = RequestHTTP.getBody().size();
    this->_env["REDIRECT_STATUS"] = "200";
    //load_file_ressources(RequestHTTP);
    executeCgi(RequestHTTP, resp);
}

// Copy constructor

Cgi::Cgi(Cgi const &src)
{
    *this = src;
}

// Destructor

Cgi::~Cgi()
{   
}

// Surcharged operator

Cgi &Cgi::operator=(Cgi const &rhs)
{
    if (this != &rhs)
    {
        this->ressources = rhs.ressources;
        this->file_ressources = rhs.file_ressources;
        this->pipe_write = rhs.pipe_write;
        this->pipe_read = rhs.pipe_read;
    }
    return (*this);
}

// Setter

void    Cgi::setPipe_write(int fd)
{
    this->pipe_write = fd;
}

void    Cgi::setPipe_read(int fd)
{
    this->pipe_read = fd;
}

char **Cgi::setEnv()
{
    char **envp = new char*[sizeof(char *) * (_env.size() + 1)];
    if (!envp)
        return NULL;
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
    {
        std::string tmp = it->first + "=" + it->second;
        envp[i] = strdup(tmp.c_str());
        i++;
    }
    envp[i] = NULL;
    return envp;
}

//Read from the pipe and return a string
std::string     Cgi::read_Cgi(void)
{
    std::string ret;
    char buffer[CGI_RESSOURCES_BUFFER_SIZE + 1];
    memset(buffer, 0, CGI_RESSOURCES_BUFFER_SIZE + 1);
    int r = 1;
    int tmp = open("/tmp/CGI.log", O_RDWR | O_CREAT | O_APPEND, 0777);
    
    if (tmp < 0)
        return "";
    while (1)
    {
        r = read(tmp, buffer, CGI_RESSOURCES_BUFFER_SIZE);
        if (r == 0)
        {
            close (tmp);
            break;
        }
        else if (r == -1)
        {
            close(tmp);
            break;
        }
        ret += buffer;
        memset(buffer, 0, CGI_RESSOURCES_BUFFER_SIZE + 1);
    }
    return ret;
}

int Cgi::executeCgi(RequestHTTP &RequestHTTP, ResponseHTTP *resp)
{
    int read_fd[2];
    int tmp;
    int pid;

    std::cerr << "WE PRINT THE END =\n";
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
    {
        std::cerr << it->first << " = " << it->second << std::endl;
    }
    std::cerr << "BODY OF THE REQUEST = " << RequestHTTP.getBody() << std::endl;
    std::cerr << "OF PRINTING\n\n";
    if (pipe(read_fd) < 0)
        return -1;
    signal(SIGALRM, kill_child_process);
    fcntl(read_fd[0], F_SETPIPE_SZ, RequestHTTP.getBody().size() + CGI_RESSOURCES_BUFFER_SIZE);
    write(read_fd[1], RequestHTTP.getBody().c_str(), RequestHTTP.getBody().size());
    pid = fork();
    if (pid < 0)
        return -1;
    else if (pid == 0)
    {
        close(read_fd[1]);
        dup2(read_fd[0], STDIN_FILENO);
        tmp = open("/tmp/CGI.log", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (tmp < 0)
            return -1;
        dup2(tmp, STDOUT_FILENO);
        dup2(tmp, STDERR_FILENO);
        char **env = setEnv();
        std::string extension = RequestHTTP.getPath().substr(RequestHTTP.getPath().find(".") + 1);
        char *av[] = {
            (char*)(strdup(resp->getCgiExecutable().c_str())),
            (char *)"-f",
            (char*)(strdup(resp->getPath().c_str())),
            NULL
        };
        if (env)
            execve(av[0], av, env);
        close(tmp);
        close(read_fd[0]);
        close(STDIN_FILENO);
        exit(EXIT_FAILURE);
    }
    else
    {
        close(read_fd[0]);
        close(read_fd[1]);
        waitpid(pid, NULL, 0);
        resp->setResponse(read_Cgi());
        return 0;
    }
}
