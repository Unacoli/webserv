/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 14:06:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/10 18:28:50 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "UtilsLibraries.hpp"

# include "ConfigDataStruct.hpp";

class Config
{
    public:
        std::vector<t_server>    _server;
 
        Config();
        Config( Config const & src );
        ~Config();
        
        Config &    operator=( Config const & rhs );

        void        FileOpenerChecker(std::string confpath);
        const int   FileChecker( std::string confpath );
        const int   SyntaxChecker( std::fstream *file );
        const int   MultiHandler( std::fstream *file );
        
};

std::ostream &  operator<<( std::ostream & o, Config const & i );

#endif