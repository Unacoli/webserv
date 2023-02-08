# include "WebServer.hpp"


int	WebServer::is_incoming_connection(std::vector<int> listen_socket, struct epoll_event *current_event, int *conn_sock, int epfd, int i)
{
	struct							epoll_event	event;
	std::vector<int>::iterator		it;
	std::vector<int>::iterator		ite;
	struct	sockaddr_in				cli_addr;
	socklen_t						cli_len = sizeof(cli_addr);

	for (it = listen_socket.begin(), ite = listen_socket.end();it != ite; it++)
	{
		if (current_event[i].data.fd == *it)
		{
			*conn_sock = accept(*it, (struct sockaddr *)&cli_addr, &cli_len);
			if (*conn_sock < 0)
				error_handler("\tSOCKET CONNECTION ERROR\t");
			std::cout << " 🔌 New incoming connection from " << inet_ntoa(cli_addr.sin_addr) << " on " << *conn_sock << " on port " << ntohs(cli_addr.sin_port) << std::endl;
			make_socket_non_blocking(*conn_sock);
			event.data.fd = *conn_sock;
			event.events = EPOLLIN;
			epoll_ctl(epfd, EPOLL_CTL_ADD, *conn_sock, &event);
			return *conn_sock;
		}
	}
	return 0;
}

void	WebServer::handle_client_request(int client_fd, struct epoll_event *current_event, int epfd, int i, std::map<int, \
std::map<std::string, t_server> > server_list, std::map<int, Client> &clients)
{
	(void)server_list;
	std::string buffer_string;
	long valread;
	//std::cout << "\033[1m\033[35m \n Entering EPOLLIN and fd is "<< current_event[i].data.fd <<"\033[0m\n" << std::endl;

	unsigned char buffer[BUFFER_SIZE] = {0};

	/* Read HTTP request recieved from client 						*/

	valread = recv(client_fd , buffer, sizeof(buffer), 0);
	std::cout << "\033[1m\033[37mBUFFER IS " << buffer << std::endl;
	std::cout << "\033[1m\033[35mREQuest from FD : " <<client_fd << " REQUEST is : " << *clients[client_fd]._request << "\033[0m\n" << std::endl;
	std::cout << "buffer len = " << strlen((char *)buffer) << std::endl;
	buffer_string = std::string((const char *)buffer, (size_t)valread);
	std::cout << buffer_string.size() << std::endl;
	std::cout << "ret is " << valread << std::endl << strerror(errno);
	if (valread < 0 )
	{
		std::cout << "READ ERROR\n";
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	else if (valread == 0)
	{
		std::cout << "recv disconnect client" << std::endl;
		client_disconnected(current_event, epfd, i, clients);
		return ;
	}
	//buffer_string()
	clients[client_fd].add_request(buffer_string);
	if (clients[client_fd]._request->isComplete() == true)
	{
		std::cout << "DONE REQUEST = " << *clients[client_fd]._request << std::endl;
		std::cout << "turning on epollout\n";
   		turn_on_epollout(current_event, epfd, i);
	}
		

	//std::cout << "Request is " << (*clients[current_event[i].data.fd]._request) << std::endl;
	
}

void	WebServer::send_client_response(int client_fd, struct epoll_event *current_event, int epfd, int i, std::map<int, \
std::map<std::string, t_server> > server_list, std::map<int, Client> &clients)
{
	long ret_send = 0;

	std::cout << client_fd << " EPOLLOUT signal\n";
	if (clients[current_event[i].data.fd]._request->isComplete() == false)
	{
		std::cout << "string is empty\n";
		return ;
	}
	RequestHTTP *request = clients[current_event[i].data.fd]._request;
	t_server server = find_server(server_list, request->_headers["Host"], current_event[i].data.fd);
	//std::cout << "Request == " << *request << std::endl;
	if (checkMaxBodySize(request->getBody().size(), server, *request) == 1)
	{
		ResponseHTTP response;
		response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
		std::cout << "RESPONSE IS " << response.getResponse() << std::endl;
		ret_send = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
		if (ret_send < 0)
		{
			client_disconnected(current_event, epfd, i, clients);
			read_error_handler("\033[1m\033[35mSend error\033[0m\n");
		}
	}
	else if (request->isComplete() == true)
	{
		clients[current_event[i].data.fd]._response = new ResponseHTTP(*request, server);
		turn_on_epollout(current_event, epfd, i);
        send_response(current_event, clients, i, epfd);

	}

}

void    WebServer::send_response(struct epoll_event *current_event, std::map<int, Client> &clients, int i, int epfd)
{
    long            ret_send;
    (void)epfd;
    unsigned int    pos = clients[current_event[i].data.fd].resp_pos * SEND_BUFFER;
    int             resp_len = clients[current_event[i].data.fd]._response->getResponse().size();
    int             max_size = resp_len > SEND_BUFFER ? SEND_BUFFER : resp_len;

   // std::cout << "RESPONSE IS " << clients[current_event[i].data.fd]._response->getResponse().c_str() + pos << std::endl;

    ret_send = send(current_event[i].data.fd , clients[current_event[i].data.fd]._response->getResponse().c_str() + pos, max_size, 0);
    if (ret_send < 0)
    {
        std::cout << "send error  = -1\n" << strerror(errno) << std::endl;
        return ;
    }
    std::cout << "Ret send = " << ret_send << "\nresponse size = " << resp_len << std::endl;
	std::cout << " POs = " << pos << std::endl;
    if ((int)pos >= resp_len || ret_send < SEND_BUFFER)
    {
		std::cout << "Response compelte ! \n";
		turn_on_epollin(current_event, epfd, i);
        clients[current_event[i].data.fd]._response->reinit();
        clients[current_event[i].data.fd]._request->reinit();
		clients[current_event[i].data.fd].resp_pos = 0;
		std::cout << "Reset pos to : " << clients[current_event[i].data.fd].resp_pos << std::endl;
    }
    else
        clients[current_event[i].data.fd].resp_pos++;

}