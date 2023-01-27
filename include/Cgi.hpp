#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include <signal.h>
# include <sys/wait.h>
# include "RequestHTTP.hpp"
# include "utils.hpp"
# include "WebServer.hpp"
# include "Config.hpp"

class Config;

class WebServer;

class Cgi{
    private:
        std::map<std::string, std::string> _env;
        int ressources;
        std::string file_ressources;
        int pipe_write;
        int pipe_read;

    public:
        Cgi(WebServer &WebServer, RequestHTTP &RequestHTTP, Config config);
        Cgi(Cgi const &src);
        ~Cgi(void);
        Cgi &operator=(Cgi const &rhs);
        friend std::ostream &operator<<(std::ostream &out, Cgi &Cgi)
        {
            out << "cgi_env" << std::endl;
            for (std::map<std::string, std::string>::iterator it = Cgi._env.begin(); it != Cgi._env.end(); it++)
            {
                out << "first: " << it->first << " || second: " << it->second << std::endl;  
            }
            return out;
        }

        void setPipe_write(int fd);
        void setPipe_read(int fd);
        char **setEnv();
        int getPipe_write(void);
        int getPipe_read(void);
        std::string getTarget_file_path(RequestHTTP &RequestHTTP, std::vector<t_server> server);
        std::string &getFile_ressources(void);

        void load_file_ressources(WebServer &WebServer, RequestHTTP &RequestHTTP);
        std::string read_Cgi(void);
        int write_Cgi(void);
        int executeCgi(RequestHTTP &RequestHTTP, Config &config);
};

#endif