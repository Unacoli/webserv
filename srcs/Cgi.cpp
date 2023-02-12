#include "Cgi.hpp"

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
    return ;
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
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);
    int r = 1;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return "";
    std::string path = std::string(cwd) + "/CGI.log";
    const char *path_recv = path.c_str();
    int tmp = open(path_recv, O_RDWR | O_CREAT | O_APPEND, 0777);
    if (tmp < 0)
        throw std::runtime_error("Error: open CGI.log");
    while (1)
    {
        r = read(tmp, buffer, BUFFER_SIZE);
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
        memset(buffer, 0, BUFFER_SIZE + 1);
    }
    return ret;
}

int Cgi::executeCgi(RequestHTTP &RequestHTTP, ResponseHTTP *resp)
{
    int tmp;
    int tmp_send;
    int pid;
    std::string body = RequestHTTP.getBody();
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return -1;
    std::string tmp_path_send = std::string(cwd) + "/CGI_send.log";
    const char *path_send = tmp_path_send.c_str();
    std::string tmp_path_recv = std::string(cwd) + "/CGI.log";
    const char *path_recv = tmp_path_recv.c_str();

    signal(SIGALRM, kill_child_process);
    tmp_send = open(path_send, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (tmp_send < 0)
        throw std::runtime_error("Error: open CGI_send.log");
    int ret = 0;
    int i = 0;
    while (body.length() - i > 0)
    {
        ret = write(tmp_send, body.c_str() + i, body.size() - i);
        if (ret < 0)
            return -1;
        if (ret == 0)
            break;
        i += ret;
    }
    close(tmp_send);
    pid = fork();
    if (pid < 0)
        return -1;
    else if (pid == 0)
    {; 
        tmp_send = open( path_send, O_RDONLY);
        if (tmp_send < 0)
            throw std::runtime_error("Error: open CGI_send.log");
        dup2(tmp_send, STDIN_FILENO);
        close(tmp_send);
        tmp = open(path_recv, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (tmp < 0)
            throw std::runtime_error("Error: open CGI.log");
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
        close(STDIN_FILENO);
        exit(EXIT_FAILURE);
    }
    else
    {
        waitpid(pid, NULL, 0);
        resp->setResponse(read_Cgi());
        return 0;
    }
}