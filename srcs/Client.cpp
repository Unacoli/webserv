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
        _request->parseRequest(request);
        std::cout << "parse request\n";
    }
    else
    {
        std::cout << "append body\n";
        _request->appendBody(request);
    }
}