#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_EVENTS 100
# define MAX_CONNECTIONS 100
# define SEND_BUFFER    30000

# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <vector>
# include <map>
# include "utils.hpp"
# include "ResponseHTTP.hpp"
# include "RequestHTTP.hpp"
# include "Cgi.hpp"
# include "Client.hpp"


class Cgi;

class Client;

class ResponseHTTP;

class RequestHTTP;

typedef struct s_server t_server;

class WebServer
{
    private:
        int                             max_fd;
        std::map<int, std::string>      status_info;
        void                Cgi_GET_resp(ResponseHTTP &resp, std::string &cgi_ret);
        void                Cgi_POST_resp(ResponseHTTP &resp, std::string &cgi_ret, RequestHTTP &req);
        int                 send_Cgi_resp(Cgi &cgi, RequestHTTP &req);

    public:
        WebServer();
        ~WebServer();
        fd_set              writes;
        fd_set              reads;
        std::string         getStatus_Cgi(std::string &cgi_ret);
        void	            handle_servers(std::vector<t_server> servers);
        std::vector<int>    init_socket(std::map<int, std::map<std::string, t_server> > server_list);
        void	            error_handler(std::string error);
        void                server_start(t_server server_config);
        void	            init_poll(int *epfd, std::vector<int> listen_sock);
        void	            add_epoll_handler(int *epfd, int listen_sock);
        void	            run_server(WebServer *_webserv);
        void	            reactor_loop(int epfd, std::map<int, std::map<std::string, t_server> > server_list, std::vector<int> listen_socket);
        void                make_socket_non_blocking(int socket_fd);
        t_server	        find_server(std::map<int, std::map<std::string, t_server> >, std::string host, int fd);
        bool	            is_request_complete(std::string request);
        int	                is_incoming_connection( std::vector<int> listen_socket, struct epoll_event *current_event, int *conn_sock, int epfd, int i);
        void	            client_disconnected(struct epoll_event *current_event, int epfd, int i);
        void            	handle_client_request(struct epoll_event *current_event, int epfd, int i, std::map<int, std::map<std::string, t_server> > server_list, std::map<int, Client> &clients);
        void            	send_client_response(struct epoll_event *current_event, int epfd, int i, std::map<int, std::map<std::string, t_server> > server_list, std::map<int, Client> &clients);
        void                add_fd_to_poll(int fd, fd_set *fds);
        void                run_select_poll(fd_set *reads, fd_set *writes);
        void	            read_error_handler(std::string error);
        void                turn_on_epollout(struct epoll_event *current_event, int epfd,  int i);
        void                turn_on_epollin(struct epoll_event *current_event, int epfd, int i);
        void                send_response(struct epoll_event *current_event, std::map<int, Client> &clients, int i, int epfd);

        


};

#endif