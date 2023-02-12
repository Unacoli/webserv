# include "WebServer.hpp"

void	WebServer::init_poll(int *epfd, std::vector<int> listen_sock)
{
	std::vector<int>::iterator	it = listen_sock.begin();
	std::vector<int>::iterator	ite = listen_sock.end();

	*epfd = epoll_create1(0);
	if (*epfd == -1)
		error_handler("\tPOLL CREATE ERROR\t");
		
	for (; it != ite; it++)
	{
		struct	epoll_event	event;

		event.data.fd = *it;
		event.events = EPOLLIN | EPOLLRDHUP ;
		/*  epoll_ctl. This is the function that allows you to add, modify and delete file */
		/*descriptors from the list that a particular epoll file descriptor is watching. */
		if (epoll_ctl(*epfd, EPOLL_CTL_ADD, *it, &event) == -1)
			error_handler("\tEPOLL CTL ERROR\t");	
		
	}
}



void	WebServer::client_disconnected( int epfd, int client_fd, std::map<int, Client> &clients)
{
	clients.erase(client_fd);
	close(client_fd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
}

std::vector<int> WebServer::init_socket(std::map<int, std::map<std::string, t_server> > server_list)
{
	std::map<int, std::map<std::string, t_server> >::iterator 	it;
	std::map<int, std::map<std::string, t_server> >::iterator 	ite;
	int									listen_sock;
	std::vector<int>					listen_sock_array;
	int 								ret = 0;
	int									on = 1;
	struct sockaddr_in					client_addr;
	socklen_t							client_len = sizeof(client_addr);
	
	it = server_list.begin();
	ite = server_list.end();

	/* initializing servers to listen to x number of ports depending on the numver*/
	/* of servers present in config file */
	for (; it != ite; it++)
	{
		listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (listen_sock < 0)
			error_handler("Socket Creation Error");
		ret = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (ret < 0)
			error_handler("set sock opt error\n");
		make_socket_non_blocking(listen_sock);	
		memset(&client_addr, 0, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(it->first);
		client_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(listen_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
				error_handler("\tBIND ERROR\t");
		getsockname(listen_sock, (struct sockaddr *) &client_addr, &client_len);

		if ((listen(listen_sock, MAX_CONNECTIONS)) < 0)
			error_handler("\tLISTEN E RROR\t");
		
		listen_sock_array.push_back(listen_sock);
	}
	return listen_sock_array;
	
}



void    WebServer::turn_on_epollout(struct epoll_event *current_event, int epfd, int i)
{
    struct	epoll_event	event;

	event.events = EPOLLOUT | EPOLLRDHUP;
	event.data.fd = current_event[i].data.fd;
	epoll_ctl(epfd, EPOLL_CTL_MOD, current_event[i].data.fd, &event);   

}

void    WebServer::turn_on_epollin(struct epoll_event *current_event, int epfd, int i)
{
    struct	epoll_event	event;

	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = current_event[i].data.fd;
	epoll_ctl(epfd, EPOLL_CTL_MOD, current_event[i].data.fd, &event);   

}

void WebServer::make_socket_non_blocking(int socket_fd)
{
	int flags;
	flags = fcntl (socket_fd, F_GETFL, 0);
  	if (flags == -1)
      perror ("fcntl");

  	flags |= O_NONBLOCK;
	if(fcntl (socket_fd, F_SETFL, flags) == -1)
      perror ("fcntl");
}