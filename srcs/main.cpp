/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:31:40 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 10:58:45 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

void     ft_launcher( std::string confpath )
{
    Config  config;
    int     pid;
    
    try
    {
        config.FileOpenerChecker(confpath, &config);
    }
    catch(const std::exception& e)
    {
        std::cerr << "webserv: " << e.what() << '\n';
    }
    std::cout << config << std::endl;
    server_start();
    return ;
}

int     main(int ac, char **av)
{
    std::string confpath;
    if (ac > 2)
    {
        std::cerr << "webserv: only one configuration file is allowed at once." << std::endl;
        return (ERROR);
    }
    if (ac == 1)
    {
        confpath = DEFAULT_PATH;
        std::cerr << "webserv: did not find any config file and chose a default one." << std::endl; 
    }
    else
    {
        if ( !std::strlen(av[1]) )
        {
            std::cerr << "webserv: config file's path can't be an empty string." << std::endl;
            return (ERROR);
        }
        confpath = av[1];
    }
    ft_launcher(confpath);
    return (SUCCESS);
}