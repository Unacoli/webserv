#include "Cgi.hpp"

// Constructor

Cgi::Cgi()
{
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
        ;
    }
    return (*this);
}

// Setter

void    Cgi::set_pipe_write(int fd)
{
    this->pipe_write = fd;
}

void    Cgi::set_pipe_read(int fd)
{
    this->pipe_read = fd;
}

// Getter

int Cgi::get_pipe_write(void)
{
    return (this->pipe_write);
}

int Cgi::get_pipe_read(void)
{
    return (this->pipe_read);
}

// Execution

std::string Cgi::executeCgi(const std::string &script)
{
    return (script);
}