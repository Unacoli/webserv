#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_EVENTS 42
# define MAX_CONNECTIONS 42

# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <vector>
# include "utils.hpp"
# include "ResponseHTTP.hpp"
# include "RequestHTTP.hpp"

typedef struct s_server t_server;

class WebServer;

void	            handle_servers(std::vector<t_server> servers);
std::vector<int>    init_socket(std::map<int, t_server> server_list);
void	            error_handler(std::string error);
void                server_start(t_server server_config);
void	            init_poll(int *epfd, int listen_sock);
void	            add_epoll_handler(int *epfd, int listen_sock);
void	            run_server(WebServer *_webserv);
void	            reactor_loop(int epfd,std::map<int, t_server>, std::vector<int> listen_socket);
void                make_socket_non_blocking(int socket_fd);
t_server	        find_server(std::map<int, t_server> server_list, int fd);
bool	            is_request_complete(std::string request);
int	                is_incoming_connection( std::vector<int> listen_socket, struct epoll_event *current_event, int *conn_sock, int epfd, int i);
void	            client_disconnected(struct epoll_event *current_event, int epfd, int i);
void            	handle_client_request(struct epoll_event *current_event, int epfd, int i, std::map<int, t_server> server_list);


#endif