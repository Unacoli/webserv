#ifndef RESPONSEHTTP_HPP
# define RESPONSEHTTP_HPP

# include <string>
# include <map>
# include <ostream>
# include "parsing.hpp"
# include "server.hpp"
# include "configDataStruct.hpp"

class RequestHTTP;

class ResponseHTTP{
    public:
        enum StatusCode { OK, CREATED, NO_CONTENT, \
                            MULTIPLE_CHOICES, MOVED_PERMANENTLY, FOUND, SEE_OTHER, NOT_MODIFIED, \
                            BAD_REQUEST, UNAUTHORIZED, FORBIDDEN, \
                            NOT_FOUND, METHOD_NOT_ALLOWED, CONFLICT, GONE, REQUEST_ENTITY_TOO_LARGE, \
                            INTERNAL_SERVER_ERROR, NOT_IMPLEMENTED, SERVICE_UNAVAILABLE, GATEWAY_TIMEOUT };
        
        ResponseHTTP();
        ResponseHTTP(const ResponseHTTP &src);
        ResponseHTTP(const RequestHTTP& request, const t_server server);
        ~ResponseHTTP();
        
        StatusCode      getStatusCode() const;
        int             getStatusCodeInt() const;
        std::string     getStatusPhrase() const;
        std::string     getHeaders() const;
        std::string     getHeader(const std::string& name) const;
        std::string     getBody() const;
        std::string     getResponse() const;
        size_t          getContentLength() const;
        
        void            sendError(StatusCode statusCode);
        void            appendBody(const std::string& body);
        ResponseHTTP    &operator=(const ResponseHTTP &rhs);

    private:
        t_location                          _location;
        t_location                          _default_serv;
        StatusCode                          _statusCode;
        std::string                         _statusPhrase;
        std::map<std::string, std::string>  _headers;
        std::string                         _content_type;
        std::string                         _body;
        std::string                         _path;
        std::string                         _response;
        
        void            methodDispatch(const RequestHTTP request);
        void            getMethodCheck(const RequestHTTP request);
        void            postMethodCheck(const RequestHTTP request);
        void            deleteMethodCheck(const RequestHTTP request);
        void            defineLocation(RequestHTTP request, t_server server);   

        std::string     generateStatusLine(StatusCode code);
        void            generateAutoIndexResponse(RequestHTTP request, t_location location);
        void            generateResponse(const RequestHTTP &request, t_server server);
        void            buildResponse( const ResponseHTTP::StatusCode &code, const std::string &statusLine, const RequestHTTP &request);
        std::string     generateDate( void );
        std::string     defineContentType( const RequestHTTP &request);
        std::string     defineContentLength( void );
        std::string     generateBody( void );
        std::string     generateErrorBody( void );
        std::string     generateFileBody( void );
        std::string     generateAutoIndexBody( void );
        void            responseMaker( void );
        std::string     defineConnection(const RequestHTTP &request);
        int             createPostPath(std::string path) const;
        std::string     handlingContentDisposition(std::string const &body, RequestHTTP request) const;
};

std::ostream    &operator<<(std::ostream &o, const ResponseHTTP &i);

#endif