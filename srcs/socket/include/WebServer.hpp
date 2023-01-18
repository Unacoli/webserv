/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clmurphy <clmurphy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 10:23:42 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/18 12:01:04 by clmurphy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "main.hpp"

class WebServer {
    private:
    public:
        WebServer();
        ~WebServer();
        int                         listen_sock;
        struct  sockaddr_in         client_addr;
        std::map<int, t_server>     server_list;             
        void    init(t_server *server_config);
        
};

#endif