/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigDataStruct.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 14:35:56 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 15:53:13 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIGDATASTRUCT_HPP
# define CONFIGDATASTRUCT_HPP

# include "main.hpp"

typedef struct s_listen {
    unsigned int    port;
    unsigned int    host;
}               t_listen;

typedef struct s_location{
    std::string                         name;               // Location directive name.
    std::string                         root;               // Define HTTP redirection path for this root.
    std::vector<std::string>            index;              // List of file to be checked for index. They should be read in the specified order as Nginx does if the request is a directory.
    std::vector<std::string>            methods;            // HTTP methods allowed in the server (GET, POST, ...)
    
    size_t                              client_body_size;   // Default is 1M in Nginx. It defines the files upload size limit for a client body request.
    
    std::string                         upload_path;        // Path to upload files if upload_status is on and the HTTP method post is enabled.
    bool                                upload_status;      // Does this server location enabled upload (ex : "upload_status on").

    bool                                autoindex;          // Enable or disable the directory listing output.
    
    std::string                         cgiparam;           //
    std::map<std::string, std::string>  cgipass;            // Map the IP and port of the CGI background server.
}               t_location;

typedef struct s_server{
    t_listen                            listen;             // Ports and hosts of the server
    std::vector<std::string>            names;              // Define virtual server names, the first one is the primary server name as in Nginx.
    std::string                         root;               // Default HTTP redirection path of the server.
    std::map<size_t, std::string>       errors;             // Map error codes with the uri of its page.
    std::vector<t_location>             locations;
}               t_server;

#endif