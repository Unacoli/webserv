/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/13 11:14:29 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/13 12:38:25 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE_HPP
# define BASE_HPP

# include "main.hpp"

//We'll implement a class Base which will be the parent class of the server and the location clases and will contain the common attributes and methods.
class Base{
    protected:
        std::vector<std::string>            server_name;
        t_listen                            listen;
        std::string                         root;
        
        std::vector<std::string>            index;
        std::vector<std::string>            methods;
        std::map<size_t, std::string>       error_page;
        
        std::string                         client_body_size;
        bool                                autoindex;
        
        std::string                         upload_path;
        bool                                upload_status;
        
        std::string                         cgipass;
        std::string                         cgi_extension;
        
        

        void                                setServerName(std::vector<std::string> names);
        void                                setListen(t_listen  listen);
        void                                setRoot(std::string root);
        void                                setIndex(std::vector<std::string> index);
        void                                setMethods(std::vector<std::string> methods);
        void                                setErrorPage(std::map<size_t, std::string> errors);
        void                                setClientBodySize(std::string client_body_size);
        void                                setAutoindex(bool autoindex);
        void                                setUploadPath(std::string upload_path);
        void                                setUploadStatus(bool upload_status);
        void                                setCgipass(std::string cgipass);
        void                                setCgiExtension(std::vector<std::string> extensions);
    
    public:
        Base();
        Base(const Base &src);
        virtual ~Base();
        Base &operator=(const Base &src);
        
        std::string     getServerName() const;
        std::string     getListen() const;
        std::string     getRoot() const;
        std::string     getIndex() const;
        std::string     getMethods() const;
        std::string     getErrorPage(size_t error) const;
        std::string     getErrorPages() const;
        std::string     getClientBodySize() const;
        bool            getAutoindex() const;
        std::string     getUploadPath() const;
        bool            getUploadStatus() const;
        std::string     getCgipass() const;
        std::string     getCgiExtension() const;
};

std::ostream &operator<<(std::ostream &o, const Base &i);

#endif