# include "Client.hpp"

Client::Client() 
{
    _request = new RequestHTTP;
    _response = new ResponseHTTP;
    response_created = 0;
    resp_pos = 0;
}

Client::~Client()
{

}

void    Client::add_request(const std::string request)
{
    if (_request->headers_received == 0)
    {
        _request->parseRequest(request);
        _request->_client_fd = -1;
        _request->_cgi_info["PATH_INFO"] = "";
        std::cout << "parse request\n";
    }
    else
    {
        std::cout << "append body\n";
        _request->appendBody(request);
    }
}