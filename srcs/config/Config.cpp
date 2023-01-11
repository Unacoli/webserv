/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 20:29:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 11:09:36 by barodrig         ###   ########.fr       */
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
    this->server = src.server;         //We'll see if we need to enter further in details for deep copy no idea for now
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
		this->server = rhs.server;
	return *this;
}

std::ostream &                operator<<( std::ostream & o, Config const & i )
{
    for ( std::vector<t_server>::const_iterator server = i.server.begin();
            server != i.server.end(); server++ )
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
**  to populate or tserver and t_location structs with the information we need by parsing the file.
**  If any of the steps fail, an exception will be thrown.
*/
void    Config::FileOpenerChecker( std::string confpath, Config *config )
{
    //Check if the file is valid and accessible.
    if (FileChecker(confpath) == ERROR)
        return ;
    //Open the file using only functions from the fstream library.
    std::fstream    file(confpath, std::ios::in);
    //Check the syntax of the file and we make sure to close the file in case of an error.
    if (SyntaxChecker(&file, config) == ERROR)
        file.close();
    //Call the MultiHandler() function that will call all the handlers necessary to populate the structs.
    if (MultiHandler(&file, config) == ERROR)
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

const int    Config::SyntaxChecker( std::fstream *file, Config *config )
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
        if (line.find_first_not_of(" \t"))
        {
            if (line.find("server"))
                ServerHandler(file, line, line_nb, config);
            continue ;
        }
        
    }
    return (SUCCESS);
}

void   Config::ServerHandler( std::fstream *file, std::string first, size_t line_nb, Config *config )
{
    t_server_block      serv;
    t_line              line;
    std::string         word;
    std::string         tmp;
    int                 braces = 0;
    if (first.find('{') != std::string::npos)
    {
        braces++;
        if (first.find_first_not_of(" \tserver{") != std::string::npos)
            throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
    }
    else
    {
        if (std::getline(*file, tmp))
        {
            if (tmp.find('{') != std::string::npos)
            {
                braces++;
                if (tmp.find_first_not_of(" \t{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
            }
            else
                throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
        }
    }
    //Now we are gonna parse each line of the server block until we find the matching closing brace.
    //Each line will be split in words that will be stored in a vector.
    //Each word stored in the std::string word then stored in the vector line.words.
    //Each line will be stored in the vector serv.server_lines.
    while (std::getline(*file, tmp))
    {
        line_nb++;
        if (tmp.empty() || (tmp[0] == '#'))
            continue ;
        if (tmp.find_first_not_of(" \t"))
        {
            if (tmp.find("location"))
                LocationHandler(file, tmp, line_nb, &serv);
            if (tmp.find("}"))
            {
                braces++;
                if (braces == 2)
                {
                    config->server_blocks.push_back(serv);
                    return ;
                }
                else
                    throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
            }
            if (tmp.find("{"))
                    throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
            std::string token = std::strtok(const_cast< char *>(tmp.c_str()), " \t");
            while (!token.empty())
            {
                line.words.push_back(token);
                token = std::strtok(NULL, " \t");
            }
            serv.server_lines.push_back(line);
            continue ;
        }
    }
}

void   Config::LocationHandler( std::fstream *file, std::string first, size_t line_nb, t_server_block *serv )
{
    t_location_block    loc;
    t_line              line;
    std::string         word;
    std::string         tmp;
    int                 braces = 0;
    if (first.find('{') != std::string::npos)
    {
        braces++;
        if (first.find_first_not_of(" \tlocation{") != std::string::npos)
            throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
    }
    else
    {
        if (std::getline(*file, tmp))
        {
            if (tmp.find('{') != std::string::npos)
            {
                braces++;
                if (tmp.find_first_not_of(" \t{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
            }
            else
                throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
        }
    }
    //Now we are gonna parse each line of the server block until we find the matching closing brace.
    //Each line will be split in words that will be stored in a vector.
    //Each word stored in the std::string word then stored in the vector line.words.
    //Each line will be stored in the vector serv.server_lines.
    while (std::getline(*file, tmp))
    {
        line_nb++;
        if (tmp.empty() || (tmp[0] == '#'))
            continue ;
        if (tmp.find_first_not_of(" \t"))
        {
            if (tmp.find("}"))
            {
                braces++;
                if (braces == 2)
                {
                    serv->location_blocks.push_back(loc);
                    return ;
                }
                else
                    throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
            }
            if (tmp.find("{"))
                    throw std::runtime_error("Syntax error on line " + std::to_string(line_nb));
            std::string token = std::strtok(const_cast< char *>(tmp.c_str()), " \t");
            while (!token.empty())
            {
                line.words.push_back(token);
                token = std::strtok(NULL, " \t");
            }
            loc.location_lines.push_back(line);
            continue ;
        }
    }
}

const int   Config::MultiHandler( std::fstream *file, Config *config )
{
    return (SUCCESS);
}
