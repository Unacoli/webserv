/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:31:40 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/10 16:27:37 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

int     main(int ac, char **av)
{
    if (ac > 2)
    {
        std::cerr << "webserv: only one configuration file is allowed at once." << std::endl;
        return (ERROR);
    }
    if (ac == 1)
    {
        std::string confpath = DEFAULT_PATH;
        std::cerr << "webserv: did not find any config file and chosed a default one." << std::endl; 
    }
    else
    {
        if ( av[1] && std::strlen(av[1]) )
            std::string confpath(av[1]);
        else
        {
            std::cerr << "webserv: config file's path can't be an empty string." << std::endl;
            return (ERROR);
        }
    }

    return (SUCCESS);
}