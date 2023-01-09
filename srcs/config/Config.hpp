/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 14:06:00 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/09 20:45:53 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "UtilsLibraries.hpp"

# include "ConfigDataStruct.hpp";

class Config
{
    public:
        Config();
        Config( Config const & src );
        ~Config();
        
        Config &    operator=( Config const & rhs );
        
        std::vector<t_server>    _server;
};

std::ostream &  operator<<( std::ostream & o, Config const & i );

#endif