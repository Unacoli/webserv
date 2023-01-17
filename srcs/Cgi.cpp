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


// Getter


// Execution

std::string Cgi::executeCgi(const std::string &script)
{

}