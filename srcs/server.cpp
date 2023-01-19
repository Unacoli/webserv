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
		event.events = EPOLLIN;
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
	
	
	it = servers.begin();
	ite = servers.end();
	while (it != ite)
	{
		servers_list.insert(std::pair<int, t_server>(atoi((*it).listen.port.c_str()), *it));
		it++;
	}
	listen_sock_array = init_socket(servers_list);
	init_epoll(&epfd, listen_sock_array);
	reactor_loop(epfd, servers_list, listen_sock_array);
}


void	reactor_loop(int epfd,std::map<int, t_server>, std::vector<int> listen_socket)
{
	int conn_sock;
	int	flag = 0;
	int	ep_count = 0;
	std::vector<int>::iterator		it;
	std::vector<int>::iterator		ite ;
	struct epoll_event current_event[MAX_EVENTS];
	struct	sockaddr_in	cli_addr;
	socklen_t	cli_len = sizeof(cli_addr);
	/* accept incoming connection */
	std::cout << "\033[1m\033[33m Entering reactor loop \033[0m" << std::endl;
	while (1)
	{
		/* setting up poll using pollfds, requested events and timeout as unlimited */
		std::cout << "📡 Activating poll using epoll fd : " << epfd << " and EP count = " << ep_count << std::endl;
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);
		if (ep_count < 0)
			error_handler("\tEPOLL WAIT ERROR\t");
		for (int i = 0; i < ep_count; i++)
		{
			std::cout << "📫 Signal received on fd " << current_event[i].data.fd << " and EP count = " << ep_count << std::endl;
			/* Firstly check if there are new incoming connections. This will */
			/* show up from the listen sock */
			
			for (it = listen_socket.begin(), ite = listen_socket.end();it != ite; it++)
			{
				flag = 0;
				std::cout << "in loop and current fd is "<< current_event[i].data.fd << std::endl;
				if (current_event[i].data.fd == *it)
				{
					conn_sock = accept(*it, (struct sockaddr *)&cli_addr, &cli_len);
					if (conn_sock < 0)
						error_handler("\tSOCKET CONNECTION ERROR\t");
					std::cout << " 🔌 New incoming connection from " << inet_ntoa(cli_addr.sin_addr) << " on " << conn_sock << " on port " << ntohs(cli_addr.sin_port) << std::endl;
					make_socket_non_blocking(conn_sock);
					current_event->data.fd = conn_sock;
					current_event->events = EPOLLIN;
					epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, current_event);
					flag = 1 ;
					break;
				}
					
			}
			if (flag == 1)
			{
				std::cout << " in flag cond\n";
				continue ;
			}
			if (current_event[i].events & EPOLLIN)
			{
				std::cout << "\033[1m\033[35m \n Entering EPOLLIN and fd is "<< current_event[i].data.fd <<"\033[0m\n" << std::endl;
				char buffer[30000] = {0};
				long valread = recv( current_event[i].data.fd , buffer, 30000, 0);
				if (valread == 0)
				{
					std::cout << " ⛔️ Client fd " << current_event[i].data.fd << " has disconnected\n";
					close(current_event[i].data.fd);
				}
				if (valread < 0)
				{
					std::cout << "closing fd " << current_event[i].data.fd << std::endl;
					close(current_event[i].data.fd);
					error_handler("\tEPOLLIN READ ERROR\t");
				}
				//RequestHTTP request(buffer);
				//std::cout << "Request analyzed is :\n" << request << std::endl;
				const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
				write(current_event[i].data.fd , hello , strlen(hello));
				std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
			}
			else if (current_event[i].events & EPOLLRDHUP) {
				std::cout << "EPOLLRDHUP : client fd" << current_event[i].data.fd << " has disconnected\n";
				close(current_event[i].data.fd);
				epoll_ctl(epfd, EPOLL_CTL_DEL, current_event[i].data.fd, NULL);
				break ;
			}
			else {
				std::cout << "ELSE\n";
			}
		}	
			
			
	}
}

void make_socket_non_blocking(int socket_fd)
{
	int flags;

	flags = fcntl(socket_fd, F_GETFL, 0);
	if (flags == -1)
		error_handler("\tFCNTL ERROR\t");
	flags |= O_NONBLOCK;
	if (fcntl(socket_fd, F_SETFL, flags) == -1)
		error_handler("\tFCNTL ERROR\t");
}