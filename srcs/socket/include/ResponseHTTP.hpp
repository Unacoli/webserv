/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHTTP.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 15:30:14 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/17 13:39:56 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHTTP_HPP
# define RESPONSEHTTP_HPP

# include "main.hpp"

class ResponseHTTP{
    public:
        enum StatusCode { OK, CREATED, NO_CONTENT, \
                            MULTIPLE_CHOICES, MOVED_PERMANENTLY, FOUND, SEE_OTHER, NOT_MODIFIED, \
                            BAD_REQUEST, UNAUTHORIZED, FORBIDDEN, \
                            NOT_FOUND, METHOD_NOT_ALLOWED, CONFLICT, GONE, \
                            INTERNAL_SERVER_ERROR, NOT_IMPLEMENTED, SERVICE_UNAVAILABLE, GATEWAY_TIMEOUT };
        
        ResponseHTTP();
        ResponseHTTP(const ResponseHTTP &src);
        ResponseHTTP(const RequestHTTP& request, t_server server);
        ~ResponseHTTP();

        std::string     generateResponse(const RequestHTTP& request, StatusCode code, const std::string& body, t_server server);
        
        StatusCode      getStatusCode() const;
        std::string     getStatusPhrase() const;
        std::string     getHeaders() const;
        std::string     getHeader(const std::string& name) const;
        std::string     getBody() const;

        ResponseHTTP    &operator=(const ResponseHTTP &rhs);

    private:
        t_location                          _location;
        StatusCode                          _statusCode;
        std::string                         _statusPhrase;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        
        void        methodDispatch(const RequestHTTP request, const t_server server);
        void        getMethodCheck(const RequestHTTP request, const t_server server);
        void        postMethodCheck(const RequestHTTP request, const t_server server);
        void        deleteMethodCheck(const RequestHTTP request, const t_server server);
        void        defineLocation(RequestHTTP request, t_server server);
        
        std::string generateStatusLine(StatusCode code);
        std::string generateAutoindex(RequestHTTP request, t_location location);
};

std::ostream    &operator<<(std::ostream &o, const ResponseHTTP &i);

#endif