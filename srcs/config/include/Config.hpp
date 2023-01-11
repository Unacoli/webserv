/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 14:06:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 16:39:36 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "main.hpp"
# include "ConfigDataStruct.hpp"
# include "Parsing.hpp"

# define SERVER 1
# define LOCATION 2

class Config
{
    public:
        std::fstream                *file;
        std::vector<t_server>       server;
        std::vector<t_server_block> server_blocks;
        
 
        Config();
        Config( Config const & src );
        ~Config();
        
        Config &    operator=( Config const & rhs );

        void        FileOpenerChecker(std::string confpath, Config *config);
        void        FileChecker( std::string confpath );
        void        SyntaxChecker( Config *config );
        void        ServerHandler( std::string first, size_t line_nb, Config *config );
        size_t      LocationHandler( std::string first, size_t line_nb, t_server_block *serv, Config *config );
        void        MultiHandler( Config *config );
        void        CheckSemiColons( Config *config )
};

std::ostream &  operator<<( std::ostream & o, Config const & i );

#endif