#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include "ResponseHTTP.hpp"

class ResponseHTTP;

class Cgi{
    private:
        std::map<std::string, std::string> _env;
    //    int ressources;
        std::string file_ressources;
        int pipe_write;
        int pipe_read;
    public:
        Cgi(void);
        Cgi(Cgi const &src);
        virtual ~Cgi(void);
        Cgi &operator=(Cgi const &rhs);
        std::string executeCgi(const std::string &script);
        void set_pipe_write(int fd);
        void set_pipe_read(int fd);
        int get_pipe_write(void);
        int get_pipe_read(void);
};

#endif