# include "WebServer.hpp"

WebServer::WebServer() : server_config(), listen_sock(), client_addr()
{
 
}

WebServer::~WebServer()
{
 
}

void    WebServer::init(t_server *_server_config)
{
    server_config = _server_config;

    memset(&client_addr, 0, sizeof(client_addr));
	this->client_addr.sin_family = AF_INET;
	this->client_addr.sin_port = htons(atoi(server_config->listen.port.c_str()));
	this->client_addr.sin_addr.s_addr = INADDR_ANY; // should I initialize host here ?

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Sock created : " << listen_sock << std::endl;
	// handle socket() error
	if (listen_sock < 0)
	{
		std::cout << strerror(errno) << std::endl;
		exit(1);
	}
	std::cout << "WebServer  initialized !\n";
}