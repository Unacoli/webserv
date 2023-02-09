#include "WebServer.hpp"

WebServer::WebServer()
{

}

WebServer::~WebServer()
{

}

void	WebServer::error_handler(std::string error)
{
	throw std::runtime_error(error + strerror(errno));
}

void	WebServer::read_error_handler(std::string error)
{
	throw std::runtime_error(error);
}

void	WebServer::handle_servers(std::vector<t_server> servers)
{
	std::map<int, std::map<std::string, t_server> >		servers_list;
	std::map<int, std::map<std::string, t_server> >::iterator server_list_it;
	int							epfd = 0;
	std::vector<int>			listen_sock_array;
	std::vector<t_server>::iterator it;
	std::vector<t_server>::iterator ite;
	std::vector<t_listen>::iterator listen_it;
	std::vector<t_listen>::iterator listen_ite;

	
	/* create a map of servers paired with their port number to be */
	/* be able to send the correct sever when an event is triggered */
	/* on its corresponding port 									*/

	it = servers.begin();
	ite = servers.end();
	while (it != ite)
	{
		// We insert the server in the map with the port number as key
		for (listen_it = it->listen.begin(), listen_ite = it->listen.end(); listen_it != listen_ite; listen_it++)
		{
			server_list_it = servers_list.find(atoi(listen_it->port.c_str()));
			if (server_list_it != servers_list.end())
			{
				server_list_it->second.insert(std::pair<std::string, t_server>(listen_it->ip, *it));
			}
			else
			{
				std::map<std::string, t_server> server_map;
				server_map.insert(std::pair<std::string, t_server>(listen_it->ip, *it));
				servers_list.insert(std::pair<int, std::map<std::string, t_server> >(atoi(listen_it->port.c_str()),  server_map));
			}

		}
		it++;
	}

	/* create an array of sockets to listen to several ports */
	/* simultaneously										*/
	listen_sock_array = init_socket(servers_list);

	/* add these sockets to the epoll structure to wait for events */
	init_poll(&epfd, listen_sock_array);
	reactor_loop(epfd, servers_list, listen_sock_array);
}


void	WebServer::reactor_loop(int epfd, std::map<int, std::map<std::string, t_server> > server_list, std::vector<int> listen_socket)
{
	int conn_sock;
	std::map<int, Client> clients;
	int	client_fd = 0;
	int	ep_count = 0;

	struct epoll_event current_event[MAX_EVENTS];
	
	/* accept incoming connection */
	//std::cout << "\033[1m\033[33m Entering reactor loop \033[0m" << std::endl;
	while (1)
	{
		std::cout << "IN EPOLL WAIT\n";
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);

		if (ep_count < 0)
			error_handler("\tEPOLL WAIT ERROR\t\n");


		/* Epoll wait has stopped waiting which means it has recieved a signal 		*/
		/* There we are going to loop through the fds it's watching and see which	*/
		/* one received a signal and thn handle it 									*/


		for (int i = 0; i < ep_count; i++)
		{
			std::cout << "epoll count is " << ep_count 	<< std::endl;

			std::cout << "📫 Signal received on fd " << current_event[i].data.fd << " and EP count = " << ep_count << std::endl;
			
			client_fd = is_incoming_connection(listen_socket, current_event, &conn_sock, epfd, i);			
			if (client_fd > 0)	
			{
				std::cout << "client fd is " << client_fd << std::endl;
				clients.insert(std::pair<int, Client>(client_fd, Client()));
				break ;
			}

			/* check if there was a disconnection or problem on fd						*/

			if (current_event[i].events & EPOLLRDHUP)
			{
				std::cout << "EPOLLRDHUP\n";
				client_disconnected(current_event, epfd, i, clients);	
			}
			else if (current_event[i].events & EPOLLOUT)
			{
				std::cout << "\n\nEPOLLOUT\n\n";
				send_client_response(current_event[i].data.fd, current_event, epfd, i, server_list, clients);
			}
			else if (current_event[i].events & EPOLLIN)
			{
				std::cout << "EPOLLIN \n";
				handle_client_request(current_event[i].data.fd, current_event, epfd, i, server_list, clients);
			}
		}	
	}
}

void	WebServer::handle_client_request(int client_fd, struct epoll_event *current_event, int epfd, int i, std::map<int, std::map<std::string, t_server> > server_list, std::map<int, Client> &clients)
{
	int			ret = 0;
	int			ret_send;
	//std::cout << "\033[1m\033[35m \n Entering EPOLLIN and fd is "<< current_event[i].data.fd <<"\033[0m\n" << std::endl;
	char buffer[30000] = {0};
//	char *save = NULL;

	/* Read HTTP request recieved from client 						*/

	long valread = recv( current_event[i].data.fd , buffer, 30000, 0);
	if (valread < 0 )
		return ;
	if (valread == 0)
	{
		client_disconnected(current_event, epfd, i, clients);
		return ;
	}
	/* handle HTTP request	*/
	RequestHTTP request(buffer);
	t_server server = find_server(server_list, request._headers["Host"], current_event[i].data.fd);
	if (checkMaxBodySize(valread, server, request) == 1)
	{
		ResponseHTTP response;
		response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
		ret_send = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
		if (ret_send < 0)
		{
			client_disconnected(current_event, epfd, i, clients);
			read_error_handler("Send error\n");
		}
		else
			ret = ret_send;
		//std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
	}
	else
	{
		while (valread > 0 && request.isComplete() == false)
		{

			valread = recv( current_event[i].data.fd , buffer, 30000, 0);
			if(valread < 0)
				return;
			if (valread == 0)
			{
				//std::cerr << "Client disconnected" << std::endl;
				client_disconnected(current_event, epfd, i, clients);
				return ;
			}
			request.appendBody(buffer);
			//std::cerr << " BODY NOW IS : " << request._body << std::endl;
			if (checkMaxBodySize(valread, server, request) == 1)
			{
				ResponseHTTP response;
				response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
				ret_send = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
				if (ret_send < 0)
				{
					client_disconnected(current_event, epfd, i, clients);
					read_error_handler("Send error\n");
				}
				else
					ret += ret_send;
				//std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
				return ;
			}
		}
	}
	/* generate response to HTTP request 	*/	
	//std::cerr << "REQUEST IS =\n" << request << std::endl;
	ResponseHTTP response(request, server);
	//std::cerr << "RESPONSE IS =\n" << response << std::endl;
	/* Send HTTP response to server						*/
	/* Loop is needed here to ensure that the entirety 	*/
	/* of a large file will be sent to the client 		*/
	//std::cout << "RESPONSE : " << response.getResponse() << std::endl;
	int error_ret = 0;
	if (ret != (int)response.getResponse().length())
	{
		while (ret < (int)response.getResponse().length())
		{
			error_ret = send(current_event[i].data.fd , response.getResponse().c_str() + ret , response.getResponse().length() - ret, 0);
			if (error_ret < 0)
			{
				client_disconnected(current_event, epfd, i, clients);
				read_error_handler("Send error\n");
			}
			ret +=  error_ret;
		}
	}
	//std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
}

void	WebServer::client_disconnected(struct epoll_event *current_event, int epfd, int i, std::map<int, Client> clients)
{
	//std::cout << " ⛔️ Client fd " << current_event[i].data.fd << " has disconnected\n";
	close(current_event[i].data.fd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, current_event[i].data.fd, NULL);
}

int	WebServer::is_incoming_connection(std::vector<int> listen_socket, struct epoll_event *current_event, int *conn_sock, int epfd, int i)
{
	std::vector<int>::iterator		it;
	std::vector<int>::iterator		ite;
	struct	sockaddr_in	cli_addr;
	socklen_t	cli_len = sizeof(cli_addr);

	for (it = listen_socket.begin(), ite = listen_socket.end();it != ite; it++)
	{
		if (current_event[i].data.fd == *it)
		{
			*conn_sock = accept(*it, (struct sockaddr *)&cli_addr, &cli_len);
			if (*conn_sock < 0)
				error_handler("\tSOCKET CONNECTION ERROR\t");
			//std::cout << " 🔌 New incoming connection from " << inet_ntoa(cli_addr.sin_addr) << " on " << *conn_sock << " on port " << ntohs(cli_addr.sin_port) << std::endl;
			make_socket_non_blocking(*conn_sock);
			current_event->data.fd = *conn_sock;
			current_event->events = EPOLLIN;
			epoll_ctl(epfd, EPOLL_CTL_ADD, *conn_sock, current_event);
			return 1;
		}
	}
	return 0;
}

t_server	WebServer::find_server(std::map<int, std::map<std::string, t_server> > server_list, std::string host, int fd)
{
	struct sockaddr_in addr;
	std::map<std::string, t_server>::iterator it;
	std::map<std::string, t_server>::iterator ite;
	socklen_t		addr_len = sizeof(addr);
	size_t pos = host.find(':');
	
	if(pos == 0)
		host = host.substr(0, pos);
	getsockname(fd, (struct sockaddr *)&addr, &addr_len);
	//std::cout << " HOST = " << host << " PORT : " << htons(addr.sin_port) << std::endl;
	std::map<std::string, t_server>	server(server_list[htons(addr.sin_port)]);
	it = server.begin();
	ite = server.end();
	for (; it !=ite; it++)
	{
		if (it->first == host)
		{
			return it->second;
		}
	}
	//std::cout << "IN FIND SERVER AND PORT IS : "<< htons(addr.sin_port) << " HOST IS " << host << std::endl;
	return (server.begin())->second;
}



bool	WebServer::is_request_complete(std::string request)
{
	//Check if the HTTP request is complete
	if (request.find("\r") != std::string::npos)
		return true;
	return false;
}
