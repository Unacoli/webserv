#include "server.hpp"

void	error_handler(std::string error)
{
	throw std::runtime_error(error + strerror(errno));
}


void	init_epoll(int *epfd, std::vector<int> listen_sock)
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
		event.events = EPOLLIN | EPOLLRDHUP;
		/*  epoll_ctl. This is the function that allows you to add, modify and delete file */
		/*descriptors from the list that a particular epoll file descriptor is watching. */
		if (epoll_ctl(*epfd, EPOLL_CTL_ADD, *it, &event) == -1)
			error_handler("\tEPOLL CTL ERROR\t");	
		
	}
}

std::vector<int> init_socket(std::map<int, t_server> server_list)
{
	std::map<int, t_server>::iterator 	it;
	std::map<int, t_server>::iterator 	ite;
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
	for (int i = 0; it != ite; it++, i++)
	{
		listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		std::cout << "Sock created : " << listen_sock << std::endl;
		if (listen_sock < 0)
			error_handler("Socket Creation Error");
		ret = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (ret < 0)
			error_handler("set sock opt error\n");
		make_socket_non_blocking(listen_sock);
		
		memset(&client_addr, 0, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(it->first);
		client_addr.sin_addr.s_addr = INADDR_ANY; // should I initialize host here ?

		std::cout << "binding socket to port " << it->first << std::endl;
		if (bind(listen_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
				error_handler("\tBIND ERROR\t");
		getsockname(listen_sock, (struct sockaddr *) &client_addr, &client_len);

		// print the port number
		std::cout <<  "Socket binded to port no : " << ntohs(client_addr.sin_port) << " at server : " << client_addr.sin_addr.s_addr <<  "listen sock is " << listen_sock << std::endl;

		if ((listen(listen_sock, MAX_CONNECTIONS)) < 0)
			error_handler("\tLISTEN E RROR\t");
		
		listen_sock_array.push_back(listen_sock);
	}
	return listen_sock_array;
	
}

void	handle_servers(std::vector<t_server> servers)
{
	std::map<int, t_server>		servers_list;
	int							epfd = 0;
	std::vector<int>			listen_sock_array;
	std::vector<t_server>::iterator it;
	std::vector<t_server>::iterator ite;
	
	/* create a map of servers paired with their port number to be */
	/* be able to send the correct sever when an event is triggered */
	/* on its corresponding port 									*/

	it = servers.begin();
	ite = servers.end();
	while (it != ite)
	{
		servers_list.insert(std::pair<int, t_server>(atoi((*it).listen.port.c_str()), *it));
		it++;
	}

	/* create an array of sockets to listen to several ports */
	/* simultaneously										*/
	listen_sock_array = init_socket(servers_list);

	/* add these sockets to the epoll structure to wait for events */
	init_epoll(&epfd, listen_sock_array);
	reactor_loop(epfd, servers_list, listen_sock_array);
}


void	reactor_loop(int epfd,std::map<int, t_server> server_list, std::vector<int> listen_socket)
{
	int conn_sock;
	int	flag = 0;
	int	ep_count = 0;

	struct epoll_event current_event[MAX_EVENTS];
	
	/* accept incoming connection */
	std::cout << "\033[1m\033[33m Entering reactor loop \033[0m" << std::endl;
	while (1)
	{
		/* setting up poll using pollfds, requested events and timeout as unlimited */
		std::cout << "📡 Activating poll using epoll fd : " << epfd << " and EP count = " << ep_count << std::endl;
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);
		if (ep_count < 0)
			error_handler("\tEPOLL WAIT ERROR\t");

		/* Epoll wait has stopped waiting which means it has recieved a signal 		*/
		/* There we are going to loop through the fds it's watching and see which	*/
		/* one received a signal and thn handle it 									*/


		for (int i = 0; i < ep_count; i++)
		{
			std::cout << "📫 Signal received on fd " << current_event[i].data.fd << " and EP count = " << ep_count << std::endl;

			/* check if its a new connection and then accept it and add it the epoll list */
			flag = is_incoming_connection(listen_socket, current_event, &conn_sock, epfd, i);			
			if (flag == 1)	
				continue ;

			/* check if there was a disconnection or problem on fd						*/

			if (current_event[i].events & EPOLLRDHUP)
				client_disconnected(current_event, epfd, i);	
			else if (current_event[i].events & EPOLLIN)
				handle_client_request(current_event, epfd, i, server_list);
			else {
				std::cout << "ELSE\n";
			}
		}	
			
			
	}
}

void	handle_client_request(struct epoll_event *current_event, int epfd, int i, std::map<int, t_server> server_list)
{
	size_t			ret;
	std::cout << "\033[1m\033[35m \n Entering EPOLLIN and fd is "<< current_event[i].data.fd <<"\033[0m\n" << std::endl;
	char buffer[30000] = {0};

	/* Read HTTP request recieved from client 						*/

	long valread = recv( current_event[i].data.fd , buffer, 30000, 0);

	// Check read errors 

	if (valread == 0)
	{
		client_disconnected(current_event, epfd, i);
		return ;
	}
	if (valread < 0)
	{
		close(current_event[i].data.fd);
		return;
	}

	/* handle HTTP request		*/
	RequestHTTP request(buffer);
	/* generate response to HTTP request 	*/	
	ResponseHTTP response(request, find_server(server_list, current_event[i].data.fd));
	
	/* Send HTTP response to server						*/
	/* Loop is needed here to ensure that the entirety 	*/
	/* of a large file will be sent to the client 		*/

	ret = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
	if (ret != response.getResponse().length())
	{
		while (ret < response.getResponse().length())
			ret += send(current_event[i].data.fd , response.getResponse().c_str() + ret , response.getResponse().length() - ret, 0);
	}
	std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
}

void	client_disconnected(struct epoll_event *current_event, int epfd, int i)
{
	std::cout << " ⛔️ Client fd " << current_event[i].data.fd << " has disconnected\n";
	close(current_event[i].data.fd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, current_event[i].data.fd, NULL);
}

int	is_incoming_connection(std::vector<int> listen_socket, struct epoll_event *current_event, int *conn_sock, int epfd, int i)
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
			std::cout << " 🔌 New incoming connection from " << inet_ntoa(cli_addr.sin_addr) << " on " << *conn_sock << " on port " << ntohs(cli_addr.sin_port) << std::endl;
			make_socket_non_blocking(*conn_sock);
			current_event->data.fd = *conn_sock;
			current_event->events = EPOLLIN;
			epoll_ctl(epfd, EPOLL_CTL_ADD, *conn_sock, current_event);
			return 1;
		}
	}
	return 0;
}

t_server	find_server(std::map<int, t_server> server_list, int fd)
{
	struct sockaddr_in addr;
	socklen_t		addr_len = sizeof(addr);

	getsockname(fd, (struct sockaddr *)&addr, &addr_len);
	std::cout << "IN FIND SERVER AND PORT IS : "<< htons(addr.sin_port) << std::endl;
	return server_list[htons(addr.sin_port)];
}

void make_socket_non_blocking(int socket_fd)
{
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
		error_handler("\tFCNTL ERROR\t");
}

bool	is_request_complete(std::string request)
{
	//Check if the HTTP request is complete
	if (request.find("\r") != std::string::npos)
		return true;
	return false;
}