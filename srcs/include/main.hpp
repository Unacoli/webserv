/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:32:08 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 11:53:55 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
# define MAIN_HPP

// Few defines
# define SUCCESS 0
# define ERROR   1 
# define DEFAULT_PATH "./testconf/simple.conf"

// C++ Libraries
# include <iostream>
# include <cstring>
# include <fstream>
# include <sstream>
# include <cstdio>
# include <exception>
# include <vector>
# include <map>
# include <set>
# include <iterator>

// C Libraries but mandatory to use to check if a file exist and is accessible before using the fstream lib.
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>

// Our *.hpp files
# include "Config.hpp"

#endif