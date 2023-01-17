#ifndef CGI_HPP
# define CGI_HPP

#include <string>

class Cgi{
    private:
        ;
    public:
        Cgi(void);
        Cgi(Cgi const &src);
        virtual ~Cgi(void);
        Cgi &operator=(Cgi const &src);
        std::string executeCgi(const std::string &script);
};

#endif