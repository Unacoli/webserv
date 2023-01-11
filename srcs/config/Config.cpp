/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 20:29:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 15:34:06 by barodrig         ###   ########.fr       */
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
    return o;
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
    FileChecker(confpath);
    //Open the file using only functions from the fstream library.
    std::fstream    file(confpath.c_str(), std::ios::in);
    config->file = &file;
    //Check the syntax of the file and we make sure to close the file in case of an error.
    SyntaxChecker(config);
    //Call the MultiHandler() function that will call all the handlers necessary to populate the structs.
    //MultiHandler(&file, config);
    return ;
}

void    Config::FileChecker( std::string confpath )
{
    struct stat     buf;
    int             fd;

    if (access(confpath.c_str(), R_OK) == -1)
        throw std::runtime_error("File is not readable.");
    if ((fd = open(confpath.c_str(), O_RDONLY)) == -1)
        throw std::runtime_error("File error.");
    if (fstat(fd, &buf) == -1)
    {
        close(fd);
        throw std::runtime_error("File error.");
    }
    if (S_ISDIR(buf.st_mode))
    {
        close(fd);
        throw std::runtime_error("File is a directory.");
    }
    close(fd);
    return ;
}

void    Config::SyntaxChecker( Config *config )
{
    std::string     line;
    size_t          line_nb = 0;

    (void) file;
    while (std::getline(*(config->file), line))
    {
        line_nb++;
        if (line.empty())
            continue ;
        if (line[0] == '#')
            continue ;
        if (line.find("server") != std::string::npos)
        {
            ServerHandler(line, line_nb, config);
            continue ;
        }
    }
    return ;
}



void   Config::ServerHandler( std::string first, size_t line_nb, Config *config )
{
    t_server_block      serv;
    t_line              line;
    std::string         word;
    std::string         tmp;
    size_t              new_line;
    int                 braces = 0;
    if (first.find('{') != std::string::npos)
    {
        braces++;
        if (first.find_first_not_of(" \tserver{") != std::string::npos)
            throw std::runtime_error("Syntax error on line " + this->SizeToStr(line_nb));
    }
    else
    {
        if (std::getline(*(config->file), tmp))
        {
            line_nb++;
            if (tmp.find('{') != std::string::npos)
            {
                braces++;
                if (tmp.find_first_not_of(" \t{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + this->SizeToStr(line_nb));
            }
            else
                throw std::runtime_error("Syntax error on line " + this->SizeToStr(line_nb));
        }
    }
    //Now we are gonna parse each line of the server block until we find the matching closing brace.
    //Each line will be split in words that will be stored in a vector.
    //Each word stored in the std::string word then stored in the vector line.words.
    //Each line will be stored in the vector serv.server_lines.
    while (std::getline(*(config->file), tmp))
    {
        line_nb++;
        if (tmp.empty() || (tmp[0] == '#'))
            continue ;
        if (tmp.find_first_not_of(" \t") != std::string::npos)
        {
            if (tmp.find("location") != std::string::npos)
            {
                new_line = LocationHandler(tmp, line_nb, &serv, config);
                while (line_nb < new_line)
                {
                    std::getline(*(config->file), tmp);
                    line_nb++;
                }
            }
            if (tmp.find("}") != std::string::npos)
            {
                braces++;
                if (braces == 2)
                {
                    if (tmp.find_first_not_of(" \t{") != std::string::npos)
                        throw std::runtime_error("Syntax error on line " + this->SizeToStr(line_nb));
                    config->server_blocks.push_back(serv);
                    return ;
                }
                else
                    throw std::runtime_error("Syntax error on line " + this->SizeToStr(line_nb));
            }
            if (tmp.find("{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + this->SizeToStr(line_nb));
            line.words = LineToWords(tmp);
            serv.server_lines.push_back(line);
            line.words.clear();
        }
    }
}

size_t   Config::LocationHandler( std::string first, size_t line_nb, t_server_block *serv, Config *config )
{
    t_location_block    loc;
    t_line              line;
    std::string         word;
    std::string         tmp;
    int                 braces = 0;
    if (first.find('{') != std::string::npos)
    {
        braces++;
        line.words = LineToWords(first);
        if (line.words.size() != 3)
        {
            throw std::runtime_error("Syntax error in location block on line " + this->SizeToStr(line_nb));
        }
        else
        {
            loc.location_lines.push_back(line);
            line.words.clear();
        }
    }
    else
    {
        if (std::getline(*(config->file), tmp))
        {
            line_nb++;
            if (tmp.find('{') != std::string::npos && tmp.find_first_not_of(" \t{") == std::string::npos)
            {
                braces++;
                line.words = LineToWords(first);
                if (line.words.size() != 2)
                    throw std::runtime_error("Syntax error in location block on line " + this->SizeToStr(line_nb - 1));
                loc.location_lines.push_back(line);
                line.words.clear();
            }
            else
                throw std::runtime_error("Syntax error in location block on line " + this->SizeToStr(line_nb));
        }
    }
    //Now we are gonna parse each line of the server block until we find the matching closing brace.
    //Each line will be split in words that will be stored in a vector.
    //Each word stored in the std::string word then stored in the vector line.words.
    //Each line will be stored in the vector serv.server_lines.
    while (std::getline(*(config->file), tmp))
    {
        line_nb++;
        if (tmp.empty() || (tmp[0] == '#'))
            continue ;
        if (tmp.find_first_not_of(" \t") != std::string::npos)
        {
            if (tmp.find("}") != std::string::npos)
            {
                braces++;
                if (braces == 2)
                {
                    serv->location_blocks.push_back(loc);
                    return (line_nb);
                }
                else
                    throw std::runtime_error("Syntax error on line in location block " + this->SizeToStr(line_nb));
            }
            if (tmp.find("{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line in location block " + this->SizeToStr(line_nb));
            line.words = LineToWords(tmp);
            loc.location_lines.push_back(line);
            line.words.clear();
        }
    }
    return (line_nb);
}

void   Config::MultiHandler( Config *config )
{
    (void) config;
    return ;
}

std::string const Config::SizeToStr( size_t nbr )
{
    std::ostringstream oss;
    oss << nbr;
    return oss.str();
}

const std::vector<std::string>  Config::LineToWords ( const std::string& str )
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find_first_of(" \t");
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty())
            tokens.push_back(token);
        start = str.find_first_not_of(" \t", end);
        end = str.find_first_of(" \t", start);
    }
    std::string token = str.substr(start, end);
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}
