#ifndef SERVER_HPP
# define SERVER_HPP

# include "main.hpp"

#define MAX_EVENTS 2

typedef struct s_server t_server;
class WebServer;

void	handle_servers(std::vector<t_server> servers);
std::vector<int> init_socket(std::map<int, t_server> server_list);
void	error_handler(std::string error);
void    server_start(t_server *server_config);
void	init_epoll(int *epfd, std::vector<int> listen_sock);
void	add_epoll_handler(int *epfd, int listen_sock);
void	run_server(WebServer *_webserv);
void	reactor_loop(int epfd,std::map<int, t_server>, std::vector<int> listen_socket);
void    make_socket_non_blocking(int socket_fd);

#endif