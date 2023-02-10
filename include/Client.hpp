#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "RequestHTTP.hpp"
# include "ResponseHTTP.hpp"

class ResponseHTTP;
typedef std::basic_string<char> string;

class Client {
    public:
        Client();
        ~Client();
        Client &Cleint(const Client &copy);
        Client &operator=(const Client &rhs);
        void    add_request(const string request);
    public:
        ResponseHTTP    *_response;
        RequestHTTP     *_request;
        unsigned int    resp_pos;
        bool            response_sent;
        bool            response_created;

};

#endif