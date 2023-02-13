#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include <signal.h>
# include <sys/wait.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/un.h>


# include "RequestHTTP.hpp"
# include "utils.hpp"
# include "WebServer.hpp"
# include "ResponseHTTP.hpp"

class WebServer;

class ResponseHTTP;
class RequestHTTP;

class Cgi{
    private:
        std::map<std::string, std::string> _env;
        int ressources;
        std::string file_ressources;
        int pipe_write;
        int pipe_read;

    public:
        Cgi(RequestHTTP RequestHTTP, ResponseHTTP *resp);
        Cgi(Cgi const &src);
        ~Cgi(void);
        Cgi &operator=(Cgi const &rhs);
        friend std::ostream &operator<<(std::ostream &out, Cgi &Cgi)
        {
            out << "cgi_env" << std::endl;
            for (std::map<std::string, std::string>::iterator it = Cgi._env.begin(); it != Cgi._env.end(); it++)
            {
                out << it->first << "=" << it->second << std::endl;  
            }
            return out;
        }

        void            setPipe_write(int fd);
        void            setPipe_read(int fd);
        char            **setEnv();

        std::string     read_Cgi(void);
        int             executeCgi(RequestHTTP &RequestHTTP, ResponseHTTP *ResponseHTTP);
};

#endif