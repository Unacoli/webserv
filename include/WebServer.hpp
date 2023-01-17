#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "server.hpp"

class WebServer {
    private:
        t_server            *server_config;
    public:
        WebServer();
        ~WebServer();
        int                  listen_sock;
        struct  sockaddr_in  client_addr;
        void    init(t_server *server_config);
        
};

#endif