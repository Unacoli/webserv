#ifndef MAIN_HPP
# define MAIN_HPP

// Few defines
# define SUCCESS 0
# define ERROR   1

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
# include <dirent.h>

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

# define PORT 8080
# define TIMEOUT 5

// Client tester
# include <sys/wait.h>

// Our *.hpp files
# include "Config.hpp"
# include "WebServer.hpp"

#endif