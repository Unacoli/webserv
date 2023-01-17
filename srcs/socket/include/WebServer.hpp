/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 10:23:42 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/16 15:34:02 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "main.hpp"

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