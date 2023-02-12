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
	char buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	/* Read HTTP request recieved from client */
	valread = recv(client_fd , buffer, sizeof(buffer), 0);
	buffer_string = std::string(buffer, (size_t)valread);
	if (clients[client_fd].request_created == 0)
	{
		clients[client_fd]._request = new RequestHTTP;
		clients[client_fd].request_created = 1;
	}
	if (clients[client_fd]._request->headers_received == 1)
		clients[client_fd]._request->bytes_read +=  buffer_string.size() ;
	if (valread < 0 )
	{
		client_disconnected(epfd, client_fd, clients);
		return ;
	}
	else if (valread == 0)
	{
		client_disconnected(epfd, client_fd, clients);
		return ;
	}

	clients[client_fd].add_request(buffer_string);
	if (clients[client_fd]._request->isComplete() == true)
	{
   		turn_on_epollout(current_event, epfd, i);
	}
}

void	WebServer::send_client_response(int client_fd, struct epoll_event *current_event, int epfd, int i, std::map<int, \
std::map<std::string, t_server> > server_list, std::map<int, Client> &clients)
{
	bool		flag = 0;

	if (clients[client_fd]._request->is_complete == false)
	{
		turn_on_epollin(current_event, epfd, i);	
		return ;
	}
	else
		flag = true;
	t_server server = find_server(server_list, clients[client_fd]._request->_headers["Host"], client_fd);
	if (flag == true)
	{
		if (clients[client_fd].response_created == 0)
		{
			clients[client_fd]._response = new ResponseHTTP(*clients[client_fd]._request, server);
			clients[client_fd].response_created = 1;
		}
        send_response(client_fd, current_event, clients, i, epfd);
	}
}

void    WebServer::send_response(int client_fd, struct epoll_event *current_event, std::map<int, Client> &clients, int i, int epfd)
{
    long               ret_send = 0;
    unsigned int       pos = clients[client_fd].resp_pos * SEND_BUFFER;
    size_t             resp_len = clients[client_fd]._response->getResponse().size();
    size_t             max_size = resp_len > SEND_BUFFER ? SEND_BUFFER : resp_len;

	if (clients[client_fd]._response->getResponse().size() == 0)
	    return ;
    ret_send = send(client_fd , clients[client_fd]._response->getResponse().c_str() + pos, max_size, 0);
    if (ret_send < 0)
    {
        std::cerr << "send error  = -1\n" << strerror(errno) << std::endl;
        return ;
    }
    if (pos >= resp_len || ret_send < SEND_BUFFER)
    {
		clients[client_fd].response_created = 0;
    	clients[client_fd].request_created = 0;
		delete clients[client_fd]._request;
		delete clients[client_fd]._response;
		clients[client_fd].resp_pos = 0;
		turn_on_epollin(current_event, epfd, i);
    }
    else
        clients[client_fd].resp_pos++;

}