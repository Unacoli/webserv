/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:32:08 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/16 15:33:11 by barodrig         ###   ########.fr       */
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
# include <string>
# include <fstream>
# include <sstream>
# include <cstdio>
# include <cstdlib>
# include <exception>
# include <vector>
# include <map>
# include <set>
# include <iterator>

// C Libraries but mandatory to use to check if a file exist and is accessible before using the fstream lib.
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>

// Socket Libraries
# include <poll.h>
# include <sys/epoll.h>
# include <iostream>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cerrno>
# include <string.h>
# include <string>
# include <cstring>

# define PORT 8080
# define MAX_CONNECTIONS 1
# define TIMEOUT 5

// Client tester
# include <sys/wait.h>
int    createClient(int clientID);

// Our *.hpp files
# include "Config.hpp"
# include "ConfigDataStruct.hpp"
# include "Parsing.hpp"
# include "Base.hpp"
# include "Utils.hpp"
# include "WebServer.hpp"
# include "server.hpp"
# include "RequestHTTP.hpp"
# include "ResponseHTTP.hpp"

#endif