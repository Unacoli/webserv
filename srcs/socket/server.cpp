/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2023/01/16 15:57:13 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "server.hpp"
# include "main.hpp"
#include "WebServer.hpp"

void	error_handler(std::string error)
{
	throw std::runtime_error(error + strerror(errno));
}

void	handle_servers(std::vector<t_server> servers)
{
	std::vector<t_server>::iterator it;
	std::vector<t_server>::iterator ite;
	
	it = servers.begin();
	ite = servers.end();
	while (it != ite)
	{
		server_start(&(*(it)));
		it++;
	}
}

void	server_start(t_server *server_config)
{
	WebServer *_webserv = new WebServer();
	_webserv->init(server_config);

	/* Bind socket to the port */
	
	std::cout << "binding socket to port\n";
	if (bind(_webserv->listen_sock, (struct sockaddr *)&_webserv->client_addr, sizeof(_webserv->client_addr)) < 0)
			error_handler("\tBIND ERROR\t");

	/* start listening for connections. */
	if ((listen(_webserv->listen_sock, MAX_CONNECTIONS)) < 0)
		error_handler("\tLISTEN ERROR\t");
	
	make_socket_non_blocking(_webserv->listen_sock);
	std::cout << "Webserv socket " << _webserv->listen_sock << " listening "<< std::endl;
	/* set up the poll fds which will be used to listen on multiple fds for client connections*/
	run_server(_webserv);
}

void	run_server(WebServer *_webserv)
{
	int	epfd = 0;
	
	init_poll(&epfd, _webserv->listen_sock);	
	reactor_loop(epfd, _webserv);
}

void	init_poll(int *epfd, int listen_sock)
{
	/* Set the epoll struct using the intial listening socket*/
	*epfd = epoll_create1(0);
	if (*epfd == -1)
		error_handler("\tPOLL CREATE ERROR\t");
	add_epoll_handler(epfd, listen_sock);	
}

void	add_epoll_handler(int *epfd, int listen_sock)
{
	struct	epoll_event	event;

	event.data.fd = listen_sock;
	event.events = EPOLLIN;
	/*  epoll_ctl. This is the function that allows you to add, modify and delete file */
	/*descriptors from the list that a particular epoll file descriptor is watching. */
	if (epoll_ctl(*epfd, EPOLL_CTL_ADD, listen_sock, &event) == -1)
		error_handler("\tEPOLL CTL ERROR\t");	
}

void	reactor_loop(int epfd, WebServer *_webserv)
{
	int conn_sock;
	int	ep_count;
	struct epoll_event current_event[MAX_EVENTS];
	struct	sockaddr_in	cli_addr;
	socklen_t	cli_len = sizeof(cli_addr);
	/* accept incoming connection */
	while (1)
	{
		/* setting up poll using pollfds, requested events and timeout as unlimited */
		std::cout << "Activating poll to listen to fds, epoll fd is " << epfd << std::endl;
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);
		if (ep_count < 0)
			error_handler("\tEPOLL WAIT ERROR\t");
		for (int i = 0; i < ep_count; i++)
		{
			/* Firstly check if there are new incoming connections. This will */
			/* show up from the listen sock */
			if (current_event[i].data.fd == _webserv->listen_sock)
			{
				conn_sock = accept(_webserv->listen_sock, (struct sockaddr *)&cli_addr, &cli_len);
				if (conn_sock < 0)
					error_handler("\tSOCKET CONNECTION ERROR\t");
				std::cout << "New incoming connection from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
				make_socket_non_blocking(conn_sock);
				current_event->data.fd = conn_sock;
				current_event->events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, current_event);
			}
			else if (current_event[i].events & EPOLLIN)
			{
				std::cout << "Entering EPOLLIN" << std::endl;
				char buffer[30000] = {0};
				long valread = recv( conn_sock , buffer, 30000, 0);
				if (valread == 0)
				{
					std::cout << "client fd " << current_event[i].data.fd << "has disconnected\n";
					close(current_event[i].data.fd);
				}
				if (valread < 0)
				{
					close(current_event[i].data.fd);
					error_handler("\tREAD ERROR\t");
				}
				std::cout << "Request received from cleint, sending message . . .\n";
				const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
				write(conn_sock , hello , strlen(hello));
				printf("------------------Hello message sent-------------------\n");
			}
			else if (current_event[i].events & EPOLLRDHUP) {
				std::cout << "client fd" << current_event[i].data.fd << "has disconnected\n";
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