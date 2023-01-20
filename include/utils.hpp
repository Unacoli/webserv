#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <cstdlib>
# include <sys/stat.h>
# include <iostream>
# include "RequestHTTP.hpp"

/*
** File System Operation
*/
bool                            isDirectory(std::string path);
bool                            fileExists(std::string path);
bool                            isReadable(std::string path);
std::string                     readFile(std::string path); 
int                             checkPath(std::string path);   
std::string                     formatRequestURI(const std::string &uri);

/*
** Request Operation
*/
const size_t                    checkMaxBodySize( int valread, t_server server, RequestHTTP const &request );
const t_location                defineLocation(const RequestHTTP request, const t_server server);

/*
** Variable Operation
*/
std::string const               IntToStr( int nbr );
std::string const               SizeToStr( size_t nbr );
std::vector<std::string>        LineToWords ( std::string str );
size_t                          StrToSize( std::string const &str );
void                            split(const std::string& s, char delim, std::vector<std::string>& parts);
std::string                     trim(const std::string &str);

#endif