# include "Client.hpp"

Client::Client() 
{
    std::cout << "in constructor \n";
    _request = new RequestHTTP;
    _response = new ResponseHTTP;
    resp_pos = 0;
}

Client::~Client()
{

}

void    Client::add_request(const std::string request)
{
    if (_request->headers_received == 0)
    {
        _request->_full_request = request;
        _request->parseRequest(request);
        _request->_client_fd = -1;
        _request->_cgi_info["PATH_INFO"] = "";
    }
    else
    {
        _request->appendBody(request);
        _request->_full_request += request;
    }
}