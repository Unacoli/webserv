/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 20:29:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/09 21:20:12 by barodrig         ###   ########.fr       */
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
    for (t_server server: i._server)
    {
        o << "- - - - - - - SERVER CONFIGURATION " << server.names[0] << " - - - - - - -" << std::endl\
            << "- Port = " << server.listen.port << std::endl\
            << "- Host = " << server.listen.host << std::endl\
            << "- Server Name(s) = ";
            for (std::string str: server.names)
                o << str << ' ';
            o << std::endl << "- Server root = " << server.root << std::endl\
            << "- Error codes : " << std::endl;
            for (std::map<size_t, std::string>::const_iterator it = server.errors.begin();
                    it != server.errors.end(); it++)
                o << "Code = " << it->first << " > URI = " << it->second << std::endl; 
            for (t_location location: server.locations)
            {
                o << "- - - LOCATION " << location.name << " - - -" << std::endl\
                << "- Location Name = " << location.name << std::endl\
                << "- Location Root = " << location.root << std::endl\
                << "- Location index = ";
                for(std::string index: location.index)
                    o << index << " ";
                o << std::endl << "- Location HTTP Methods = ";
                for(std::string method: location.methods)
                    o << method << " ";
                o << std::endl << "- Client Body Size = " << location.client_body_size << std::endl\
                << "- Upload Path = " << location.upload_path << std::endl\
                << "- Upload Status = " << location.upload_status << std::endl\
                << "- Autodindex = " << location.autoindex << std::endl\
                << "- CGI Param = " << location.cgiparam << std::endl\
                << "- CGI Pass = " << std::endl;
                for (std::map<std::string, std::string>::const_iterator it = location.cgipass.begin();
                        it != location.cgipass.end(); it++)
                    o << it->first << ":" << it->second << std::endl;
                o << std::endl;
            }
    }
}