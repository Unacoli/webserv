#include "Cgi.hpp"

#define CGI_RESSOURCES_BUFFER_SIZE 100
#define CGI_READ_BUFFER_SIZE 64000

static void kill_child_process(int sig)
{
    (void) sig;
    kill(-1, SIGKILL);
}

// Constructor

Cgi::Cgi(WebServer &WebServer, RequestHTTP &RequestHTTP)
{
    this->_env["AUTH_TYPE"] = "";
    this->_env["CONTENT_TYPE"] = RequestHTTP._headers["Content-Type"];
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["PATH_INFO"] = RequestHTTP.getPath();
    this->_env["PATH_TRANSLATED"] = this->getTarget_file_path(RequestHTTP);
    this->_env["QUERY_STRING"] = RequestHTTP.getQuery();
    this->_env["REMOTE_HOST"] = RequestHTTP._headers["Host"];
    this->_env["REMOTE_ADDR"] = getIP(RequestHTTP.getClient_fd());
    this->_env["REMOTE_USER"] = "";
    this->_env["REMOTE_IDENT"] = "";
    this->_env["REQUEST_METHOD"] = RequestHTTP.getMethod();
    this->_env["REQUEST_URI"] = RequestHTTP.getPath();
    this->_env["SCRIPT_NAME"] = RequestHTTP.getPath();
    this->_env["SCRIPT_FILENAME"] = this->getTarget_file_path(RequestHTTP);
    this->_env["SERVER_NAME"] = RequestHTTP._headers["Host"];
    this->_env["SERVER_PROTOCOL"] = RequestHTTP.getHTTPVersion();
    this->_env["SERVER_PORT"] = RequestHTTP.getPort();
    this->_env["SERVER_SOFTWARE"] = "Engine_X/1.0";
    this->_env["CONTENT_LENGTH"] = "-1";
    load_file_ressources(WebServer, RequestHTTP);
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
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        envp[i] = strdup((it->first + "=" + it->second).c_str());
        ++i;
    }
    envp[i] = NULL;
    return envp;
}

// Getter

int Cgi::getPipe_write(void)
{
    return (this->pipe_write);
}

int Cgi::getPipe_read(void)
{
    return (this->pipe_read);
}

std::string Cgi::getTarget_file_path(RequestHTTP &RequestHTTP)
{
    std::string ret;
    char *pwd = getcwd(NULL, 0);
    std::string req_root = RequestHTTP.getRoot();
    std::string req_path = RequestHTTP.getPath();

    ret += pwd;
    if (req_root[0] == '.')
    {
        req_root = req_root.substr(1);
    }
    ret += req_root;
    ret += req_path.substr(RequestHTTP.getPath().size());
    free(pwd);
    return ret;
}

std::string &Cgi::getFile_ressources(void)
{
    return this->file_ressources;
}

// Execution
       
void Cgi::load_file_ressources(WebServer &WebServer, RequestHTTP &RequestHTTP)
{
    if (RequestHTTP.getMethod() == "GET")
    {
        this->ressources = open(this->_env["PATH_TRANSLATED"].c_str(), O_RDONLY);
        if (this->ressources < 0)
            return ;
        char buffer[CGI_RESSOURCES_BUFFER_SIZE + 1];
        memset(buffer, 0, CGI_RESSOURCES_BUFFER_SIZE + 1);
        int r = 1;
        while (1)
        {
            WebServer.add_fd_to_poll(this->ressources, &(WebServer.reads));
            WebServer.run_select_poll(&(WebServer.reads), &(WebServer.writes));
            if (FD_ISSET(this->ressources, &(WebServer.reads)) == 0)
            {
                close(this->ressources);
                this->ressources = -1;
                break;
            }
            r = read(this->ressources, buffer, CGI_RESSOURCES_BUFFER_SIZE);
            if (r == 0)
            {
                close (this->ressources);
                break;
            }
            else if (r == -1)
            {
                close(this->ressources);
                this->ressources = -1;
                break;
            }
            this->file_ressources += buffer;
            memset(buffer, 0, CGI_RESSOURCES_BUFFER_SIZE + 1);
        }
        FD_ZERO(&(WebServer.reads));
        this->_env["CONTENT_LENGTH"] = number_to_string(this->file_ressources.size());
    }
    if (RequestHTTP.getMethod() == "POST")
    {
        this->file_ressources = RequestHTTP._body;
        this->_env["CONTENT_LENGTH"] = number_to_string(RequestHTTP._body.size());
    }
}
       
std::string Cgi::read_Cgi(void)
{
    int rbytes = 1;
    char buffer[CGI_READ_BUFFER_SIZE + 1];
    memset(buffer, 0, CGI_READ_BUFFER_SIZE + 1);
    std::string ret;

    while (rbytes > 0)
    {
        rbytes = read(this->getPipe_read(), buffer, CGI_READ_BUFFER_SIZE);
        if (rbytes < 0)
            return NULL;
        ret += buffer;
        memset(buffer, 0, CGI_READ_BUFFER_SIZE + 1);
    }
    return ret;
}

int Cgi::write_Cgi(void)
{
    int wbyte = write(this->getPipe_write(), this->file_ressources.c_str(), this->file_ressources.size());
    if (wbyte == -1)
    {
        std::cout << "[ERROR] Cgi: write failed" << std::endl;
        alarm(30);
        waitpid(-1, NULL, 0);
        return (-1);
    }
    else
    {
        signal(SIGALRM, SIG_DFL);
        return wbyte;
    }
}

int Cgi::executeCgi(RequestHTTP &RequestHTTP)
{
    int read_fd[2];
    int write_fd[2];
    int pid;
    int ret1 = pipe(read_fd);

    if (ret1 < 0 || pipe(write_fd) < 0 || (RequestHTTP.getMethod() == "GET" && (ressources < 0)))
        return -1;
    signal(SIGALRM, kill_child_process);
    pid = fork();
    if (pid < 0)
        return -1;
    else if (pid == 0)
    {
        dup2(write_fd[0], STDIN_FILENO);
        dup2(read_fd[1], STDOUT_FILENO);
        close(write_fd[1]);
        close(read_fd[0]);
        char **env = setEnv();
        std::string extension = RequestHTTP.getPath().substr(RequestHTTP.getPath().find(".") + 1);
        char *av[3];
        av[0] = const_cast<char*>(RequestHTTP.getCgi_info(extension).c_str());
        av[1] = const_cast<char*>(RequestHTTP.getRoot().c_str());
        av[2] = NULL;
        if (env)
            ret1 = execve(av[0], av, env);
        exit(1);
    }
    else
    {
        close(write_fd[0]);
        close(read_fd[1]);
        setPipe_write(write_fd[1]);
        setPipe_read(read_fd[0]);
        return 0;
    }
}
