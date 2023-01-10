/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 14:06:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/10 20:56:34 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "UtilsLibraries.hpp"

# define SERV config.server
# define PARSE config.parsing

class Config
{
    public:
        std::vector<t_server>    server;
        t_parsing                *parsing;
        
 
        Config();
        Config( Config const & src );
        ~Config();
        
        Config &    operator=( Config const & rhs );

        void        FileOpenerChecker(std::string confpath, Config *config);
        const int   FileChecker( std::string confpath );
        const int   SyntaxChecker( std::fstream *file, Config *config );
        const int   ServerHandler( std::fstream *file, std::string first, size_t line_nb, Config *config );
        const int   MultiHandler( std::fstream *file, Config *config );
        
};

std::ostream &  operator<<( std::ostream & o, Config const & i );

#endif