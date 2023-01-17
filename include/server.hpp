#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_EVENTS 42
# define MAX_CONNECTIONS 1

# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <vector>
# include "configDataStruct.hpp"
# include "utils.hpp"
# include "WebServer.hpp"
# include "RequestHTTP.hpp"

typedef struct s_server t_server;

class WebServer;

void	handle_servers(std::vector<t_server> servers);
void	error_handler(std::string error);
void    server_start(t_server *server_config);
void	init_poll(int *epfd, int listen_sock);
void	add_epoll_handler(int *epfd, int listen_sock);
void	run_server(WebServer *_webserv);
void	reactor_loop(int epfd, WebServer *_webserv);
void    make_socket_non_blocking(int socket_fd);

#endif