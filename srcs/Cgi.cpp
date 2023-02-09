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
    this->_env["CONTENT_TYPE"] = RequestHTTP._headers["Content-Type"];
    this->_env["QUERY_STRING"] = "";
    this->_env["REQUEST_METHOD"] = RequestHTTP.getMethodString();
    this->_env["SCRIPT_FILENAME"] = resp->getPath();
    if (RequestHTTP.getHeader("Content-Length") != "")
        this->_env["CONTENT_LENGTH"] = RequestHTTP.getHeader("Content-Length");
    else
        this->_env["CONTENT_LENGTH"] = SizeToStr(RequestHTTP.getBody().size());
    this->_env["REDIRECT_STATUS"] = "200";
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
    int tmp = open("/mnt/nfs/homes/barodrig/webserv/CGI.log", O_RDWR | O_CREAT | O_APPEND, 0777);
    
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
    size_t buffer_size = RequestHTTP.getContentLength();
    std::string body = RequestHTTP.getBody();

    std::cerr << "WE PRINT THE END =" << std::endl;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
    {
        std::cerr << it->first << " = " << it->second << std::endl;
    }
    // std::cerr << "BODY OF THE REQUEST = " << RequestHTTP.getBody() << std::endl;
    std::cerr << "OFF PRINTING\n\n";
    if (pipe(read_fd) < 0)
        return -1;
    signal(SIGALRM, kill_child_process);
    fcntl(read_fd[1], F_SETPIPE_SZ, buffer_size);
    if (fcntl(read_fd[1], F_GETPIPE_SZ) < 0)
        return -1;
    int ret = write(read_fd[1], body.c_str(), body.length());
    if (ret < 0)
        return -1;
    pid = fork();
    if (pid < 0)
        return -1;
    else if (pid == 0)
    {
        close(read_fd[1]);
        dup2(read_fd[0], STDIN_FILENO);
        tmp = open("/mnt/nfs/homes/barodrig/webserv/CGI.log", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (tmp < 0)
            return -1;
        dup2(tmp, STDOUT_FILENO);
        dup2(tmp, STDERR_FILENO);
        char **env = setEnv();
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
        std::cerr << "CGI RESPONSE IS " << read_Cgi() << std::endl;
        resp->setResponse(read_Cgi());
        return 0;
    }
}