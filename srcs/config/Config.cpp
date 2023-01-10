/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 20:29:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/10 18:57:19 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Config::Config()
{
}

Config::Config( const Config & src )
{
    this->_server = src._server;         //We'll see if we need to enter further in details for deep copy no idea for now
    *this = src;
}

/*
** ------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Config &				Config::operator=( Config const & rhs )
{
	if ( this != &rhs )
		this->_server = rhs._server;
	return *this;
}

std::ostream &                operator<<( std::ostream & o, Config const & i )
{
    for ( std::vector<t_server>::const_iterator server = i._server.begin();
            server != i._server.end(); server++ )
    {
        o << "- - - - - - - SERVER CONFIGURATION " << server->names[0] << " - - - - - - -" << std::endl\
            << "- Port = " << server->listen.port << std::endl\
            << "- Host = " << server->listen.host << std::endl\
            << "- Server Name(s) = ";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            for ( std::vector<std::string>::const_iterator str = server->names.begin();
                    str != server->names.end(); str++ )
                o << *str << ' ';
            o << std::endl << "- Server root = " << server->root << std::endl\
            << "- Error codes : " << std::endl;
            for (std::map<size_t, std::string>::const_iterator it = server->errors.begin();
                    it != server->errors.end(); it++)
                o << "Code = " << it->first << " > URI = " << it->second << std::endl; 
            for ( std::vector<t_location>::const_iterator location = server->locations.begin();
                    location != server->locations.end(); location++ )
            {
                o << "- - - LOCATION " << location->name << " - - -" << std::endl\
                << "- Location Name = " << location->name << std::endl\
                << "- Location Root = " << location->root << std::endl\
                << "- Location index = ";
                for ( std::vector<std::string>::const_iterator index = location->index.begin();
                        index != location->index.end(); index++ )
                    o << *index << " ";
                o << std::endl << "- Location HTTP Methods = ";
                for ( std::vector<std::string>::const_iterator method = location->methods.begin();
                        method != location->methods.end(); method++ )
                    o << *method << " ";
                o << std::endl << "- Client Body Size = " << location->client_body_size << std::endl\
                << "- Upload Path = " << location->upload_path << std::endl\
                << "- Upload Status = " << location->upload_status << std::endl\
                << "- Autodindex = " << location->autoindex << std::endl\
                << "- CGI Param = " << location->cgiparam << std::endl\
                << "- CGI Pass = " << std::endl;
                for (std::map<std::string, std::string>::const_iterator it = location->cgipass.begin();
                        it != location->cgipass.end(); it++)
                    o << it->first << ":" << it->second << std::endl;
                o << std::endl;
            }
    }
}

/*
** --------------------------------- METHODS ---------------------------------
*/

/*
**  FileOpenerChecker() function will first redirect the path of the conf file to a FileChecker() function
**  who's purpose is to check if the file is valid and accessible or not. If it is valid, FileOpenerChecker()
**  will itself open the file stream.
**  The file will be opened using <cstdio> library.
**  The next step will be to redirect the opened file to the SyntaxChecker() function that will check the conformity
**  of the config file syntax with our rules.
**  Finally, it will redirect the opened file to the MultiHandler() function that will call all the handlers necessary
**  to populate or t_server and t_location structs with the information we need by parsing the file.
**  If any of the steps fail, an exception will be thrown.
*/
void    Config::FileOpenerChecker( std::string confpath )
{
    //Check if the file is valid and accessible.
    if (FileChecker(confpath) == ERROR)
        return ;
    //Open the file using only functions from the fstream library.
    std::fstream    file(confpath, std::ios::in);
    //Check the syntax of the file and we make sure to close the file in case of an error.
    if (SyntaxChecker(&file) == ERROR)
        file.close();
    //Call the MultiHandler() function that will call all the handlers necessary to populate the structs.
    if (MultiHandler(&file) == ERROR)
    {
        file.close();
        throw std::runtime_error("MultiHandler() failed");
    }
    return ;
}

const int    Config::FileChecker( std::string confpath )
{
    struct stat     buf;
    int             fd;

    //Check if the file is valid and accessible.
    if ((fd = open(confpath.c_str(), O_RDONLY)) == -1)
        return (ERROR);
    if (fstat(fd, &buf) == -1)
        return (ERROR);
    if (S_ISDIR(buf.st_mode))
        return (ERROR);
    if (access(confpath.c_str(), R_OK) == -1)
        return (ERROR);
    return (SUCCESS);
}

// This syntax checker will 
const int    Config::SyntaxChecker( std::fstream *file )
{
    std::string     line;
    size_t          line_nb = 0;

    while (std::getline(*file, line))
    {
        line_nb++;
        if (line.empty())
            continue ;
        if (line[0] == '#')
            continue ;
        
    }
    return (SUCCESS);
}

const int   Config::MultiHandler( std::fstream *file )
{
    return (SUCCESS);
}
