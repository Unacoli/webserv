/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 20:29:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 20:05:19 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

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
            << "- Port = " + server->listen.port << std::endl\
            << "- Host = " + server->listen.host << std::endl\
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
    CheckSemiColons(config);
    //Call the MultiHandler() function that will call all the handlers necessary to populate the structs.
    MultiHandler(config);
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
    size_t          new_line = 0;

    while (std::getline(*(config->file), line))
    {
        line_nb++;
        if (line.empty())
            continue ;
        if (line[0] == '#')
            continue ;
        if (line.find("server") != std::string::npos)
        {
            new_line = ServerHandler(line, line_nb, config);
            line_nb = 0;
            config->file->seekg(0);
            while (line_nb < new_line)
            {
                std::getline(*(config->file), line);
                line_nb++;
            }
            continue ;
        }
    }
    return ;
}



size_t   Config::ServerHandler( std::string first, size_t line_nb, Config *config )
{
    t_server_block      serv;
    t_line              line;
    std::string         word;
    std::string         tmp;
    size_t              new_line;
    int                 braces = 0;
    line.words = LineToWords(first);
    line.line_number = line_nb;
    serv.server_lines.push_back(line);
    line.words.clear();
    if (first.find('{') != std::string::npos)
    {
        braces++;
        if (first.find_first_not_of(" \tserver{") != std::string::npos)
            throw std::runtime_error("Syntax error on line " + SizeToStr(line_nb));
    }
    else
    {
        if (std::getline(*(config->file), tmp))
        {
            line_nb++;
            line.words = LineToWords(tmp);
            line.line_number = line_nb;
            serv.server_lines.push_back(line);
            line.words.clear();
            if (tmp.find('{') != std::string::npos)
            {
                braces++;
                if (tmp.find_first_not_of(" \t{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line_nb));
            }
            else
                throw std::runtime_error("Syntax error on line " + SizeToStr(line_nb));
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
                line_nb = 0;
                config->file->seekg(0);
                while (line_nb < new_line)
                {
                    std::getline(*(config->file), tmp);
                    line_nb++;
                }
                continue ;
            }
            if (tmp.find("}") != std::string::npos)
            {
                braces++;
                if (braces == 2)
                {
                    if (tmp.find_first_not_of(" \t}") != std::string::npos)
                        throw std::runtime_error("Syntax error 1 on line " + SizeToStr(line_nb));
                    line.words = LineToWords(tmp);
                    line.line_number = line_nb;
                    serv.server_lines.push_back(line);
                    line.words.clear();
                    config->server_blocks.push_back(serv);
                    return (line_nb);
                }
                else
                    throw std::runtime_error("bSyntax error 2 on line " + SizeToStr(line_nb));
            }
            if (tmp.find("{") != std::string::npos)
                    throw std::runtime_error("Syntax error 3 on line " + SizeToStr(line_nb));
            if (!tmp.empty() || (tmp[0] != '#'))
            {
                line.words = LineToWords(tmp);
                line.line_number = line_nb;
                serv.server_lines.push_back(line);
                line.words.clear();
            }
        }
    }
    throw std::runtime_error("Syntax error 4 on line " + SizeToStr(line_nb) + ". Missing closing brace.");
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
            throw std::runtime_error("Syntax error 1 in location block on line " + SizeToStr(line_nb));
        line.line_number = line_nb;
        loc.location_lines.push_back(line);
        line.words.clear();
    }
    else
    {
        if (std::getline(*(config->file), tmp))
        {
            line_nb++;
            if (tmp.find('{') != std::string::npos && tmp.find_first_not_of(" \t{") == std::string::npos)
            {
                braces++;
                line.words = LineToWords(tmp);
                if (line.words.size() != 2)
                    throw std::runtime_error("Syntax error 2 in location block on line " + SizeToStr(line_nb - 1));
                line.line_number = line_nb;
                loc.location_lines.push_back(line);
                line.words.clear();
            }
            else
                throw std::runtime_error("Syntax error 3 in location block on line " + SizeToStr(line_nb));
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
                    line.words = LineToWords(tmp);
                    line.line_number = line_nb;
                    loc.location_lines.push_back(line);
                    line.words.clear();
                    serv->location_blocks.push_back(loc);
                    return (line_nb);
                }
                else
                    throw std::runtime_error("Syntax error 4 on line in location block " + SizeToStr(line_nb));
            }
            if (tmp.find("{") != std::string::npos)
                    throw std::runtime_error("Syntax error 5 on line in location block " + SizeToStr(line_nb));
            line.words = LineToWords(tmp);
            line.line_number = line_nb;
            loc.location_lines.push_back(line);
            line.words.clear();
        }
    }
    throw std::runtime_error("Syntax error 6 on line " + SizeToStr(line_nb) + ". Missing closing brace.");
}

void    Config::CheckSemiColons( Config *config )
{
    for ( std::vector<t_server_block>::const_iterator server = config->server_blocks.begin();
            server != config->server_blocks.end(); server++ )
    {
        for ( std::vector<t_line>::const_iterator line = server->server_lines.begin();
                line != server->server_lines.end(); line++ )
        {
            if (line->words[line->words.size() - 1].find(';') == std::string::npos \
                    && line->words[line->words.size() - 1].find_first_of("{}") == std::string::npos)
                throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) +  " : missing ';' at the end of the line.");
            if (line->words[line->words.size() - 1].find_first_of("{}") != std::string::npos \
                     && line->words[line->words.size() - 1].find(";") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : extra ';' at the end of the line.");
        }
        for ( std::vector<t_location_block>::const_iterator location = server->location_blocks.begin();
                location != server->location_blocks.end(); location++ )
        {
            for ( std::vector<t_line>::const_iterator line = location->location_lines.begin();
                    line != location->location_lines.end(); line++ )
            {
                if (line->words[line->words.size() - 1].find(';') == std::string::npos \
                        && line->words[line->words.size() - 1].find_first_of("{}") == std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number)+ " : missing ';' at the end of the line.");
                if (line->words[line->words.size() - 1].find_first_of("{}") != std::string::npos \
                        && line->words[line->words.size() - 1].find(";") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : extra ';' at the end of the line.");
            }
        }
    }
}

void    Config::MultiHandler( Config *config )
{
    //Now we are gonna parse every line of our server_blocks and location_blocks.
    //We are gonna check the format of each line at the same time and fill the t_server structure and its vector of t_location with the information we'll gather.
    //If we encounter an unvalid directive, format, etc... We'll throw an exception.

     for ( std::vector<t_server_block>::const_iterator server = config->server_blocks.begin();
            server != config->server_blocks.end(); server++ )
    {
        t_server serv;
        for ( std::vector<t_line>::const_iterator line = server->server_lines.begin();
                line != server->server_lines.end(); line++ )
        {
            if (line->words[0] == "listen")
            {
                t_listen listen;
                if (line->words.size() == 2)
                {
                    listen.port = line->words[1];
                    listen.host = "*";
                    serv.listen = listen;
                }
                else if (line->words.size() == 3)
                {
                    listen.port = line->words[2];
                    listen.host = line->words[1];
                    serv.listen = listen;
                }
                else
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for listen directive.");
            }
            else if (line->words[0] == "server_name")
            {
                for (size_t i = 1; i < line->words.size(); i++)
                    serv.names.push_back(line->words[i]);
            }
            else if (line->words[0] == "error")
            {
                if (line->words.size() != 3)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for error_page directive.");
                serv.errors.insert(std::pair<size_t, std::string>(StrToSize(line->words[1]), line->words[2]));
            }
            else if (line->words[0] == "root")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for root directive.");
                serv.root = line->words[1];
            }
            else if (line->words[0] == "index")
            {
                for (size_t i = 1; i < line->words.size(); i++)
                    serv.index.push_back(line->words[i]);
            }
            else if (line->words[0] == "client_max_body_size")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for client_max_body_size directive.");
                serv.client_body_size = line->words[1];
            }
            else if (line->words[0] == "autoindex")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for autoindex directive.");
                if (line->words[1] != "on" && line->words[1] != "off"\
                        && line->words[1] != "on;" && line->words[1] != "off;")
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for autoindex directive.");
                if (line->words[1] == "on" || line->words[1] == "on;")
                    serv.autoindex = true;
                else
                    serv.autoindex = false;
            }
            else if (line->words[0] == "cgiparam")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgiparam directive.");
                serv.cgiparam = line->words[1];
            }
            else if (line->words[0] == "cgipass")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgipass directive.");
                if (line->words[1].find(":") == std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for cgipass directive.");
                serv.cgipass.insert(std::pair<std::string, std::string>(line->words[1].substr(0, line->words[1].find(":")), line->words[1].substr(line->words[1].find(":") + 1)));
            }
            else if (line->words[0] == "server" || line->words[0] == "}")
                continue ;
            else
                throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid directive.");
        }
        for ( std::vector<t_location_block>::const_iterator location = server->location_blocks.begin();
                location != server->location_blocks.end(); location++ )
        {
            t_location loc;
            for ( std::vector<t_line>::const_iterator line = location->location_lines.begin();
                    line != location->location_lines.end(); line++ )
            {
                if (line->words[0] == "location")
                {
                    if (line->words.size() != 3)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for location directive.");
                    if ( line->words[1][0] != '/' && line->words[1] != "=" && line->words[1] != "~" && line->words[1] != "~*")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid operator for location directive.");
                    loc.name = line->words[2];
                }
                else if (line->words[0] == "root")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for root directive.");
                    loc.root = line->words[1];
                }
                else if (line->words[0] == "index")
                {
                    for (size_t i = 1; i < line->words.size(); i++)
                        loc.index.push_back(line->words[i]);
                }
                else if (line->words[0] == "method")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for method directive.");
                    loc.methods.push_back(line->words[1]);
                }
                else if (line->words[0] == "client_body_size")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for client_body_size directive.");
                    loc.client_body_size = line->words[1];
                }
                else if (line->words[0] == "upload_status")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for upload_status directive.");
                    if (line->words[1] != "on" && line->words[1] != "off"\
                            && line->words[1] != "on;" && line->words[1] != "off;")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for upload_status directive.");
                    if (line->words[1] == "on" || line->words[1] == "on;")
                        loc.upload_status = true;
                    else
                        loc.upload_status = false;
                }
                else if (line->words[0] == "upload_path")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for upload_path directive.");
                    loc.upload_path = line->words[1];
                }
                else if (line->words[0] == "autoindex")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for autoindex directive.");
                    if (line->words[1] != "on" && line->words[1] != "off"\
                            && line->words[1] != "on;" && line->words[1] != "off;")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for autoindex directive.");
                    if (line->words[1] == "on" || line->words[1] == "on;")
                        loc.autoindex = true;
                    else
                        loc.autoindex = false;
                }
                else if (line->words[0] == "cgiparam")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgiparam directive.");
                    loc.cgiparam = line->words[1];
                }
                else if (line->words[0] == "cgipass")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgipass directive.");
                    if (line->words[1].find(":") == std::string::npos)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for cgipass directive.");
                    loc.cgipass.insert(std::pair<std::string, std::string>(line->words[1].substr(0, line->words[1].find(":")), line->words[1].substr(line->words[1].find(":") + 1)));
                }
                else if (line->words[0] == "}")
                    continue ;
                else
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid directive.");
            }
            serv.locations.push_back(loc);
        }
        config->server.push_back(serv);
    }
    
    return ;
}
