<<<<<<< HEAD:srcs/socket/include/ResponseHTTP.hpp
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHTTP.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 15:30:14 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/17 18:02:22 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

=======
>>>>>>> nargouse:include/ResponseHTTP.hpp
#ifndef RESPONSEHTTP_HPP
# define RESPONSEHTTP_HPP

# include <string>
# include <map>
# include <ostream>
# include "Parsing.hpp"
# include "server.hpp"

class RequestHTTP;

class ResponseHTTP{
    public:
        enum StatusCode { OK, CREATED, NO_CONTENT, \
                            MULTIPLE_CHOICES, MOVED_PERMANENTLY, FOUND, SEE_OTHER, NOT_MODIFIED, \
                            BAD_REQUEST, UNAUTHORIZED, FORBIDDEN, \
                            NOT_FOUND, METHOD_NOT_ALLOWED, CONFLICT, GONE, \
                            INTERNAL_SERVER_ERROR, NOT_IMPLEMENTED, SERVICE_UNAVAILABLE, GATEWAY_TIMEOUT };
        
        ResponseHTTP();
        ResponseHTTP(const ResponseHTTP &src);
        ResponseHTTP(const RequestHTTP& request, const t_server server);
        ~ResponseHTTP();
        
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
        std::string                         _content_type;
        std::string                         _body;
        std::string                         _path;
        std::string                         _response;
        
        void        methodDispatch(const RequestHTTP request, const t_server server);
        void        getMethodCheck(const RequestHTTP request, const t_server server);
        void        postMethodCheck(const RequestHTTP request, const t_server server);
        void        deleteMethodCheck(const RequestHTTP request, const t_server server);
        void        defineLocation(RequestHTTP request, t_server server);
        
        std::string generateStatusLine(StatusCode code);
        void        generateAutoIndexResponse(RequestHTTP request, t_location location);
        void        generateResponse(const RequestHTTP &request, t_server server);
        void        buildResponse( const ResponseHTTP::StatusCode &code, const std::string &statusLine, const RequestHTTP &request);
        std::string generateDate( void );
        std::string defineContentType( const RequestHTTP &request);
        std::string defineContentLength( void );
        std::string generateBody( void );
        std::string generateErrorBody( void );
        std::string generateFileBody( void );
        std::string generateAutoIndexBody( void );
        void        responseMaker( void );
};

std::ostream    &operator<<(std::ostream &o, const ResponseHTTP &i);

#endif