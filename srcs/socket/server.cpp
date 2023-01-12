/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clmurphy <clmurphy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 10:24:43 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 11:10:01 by clmurphy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

void	init_client_addr(struct sockaddr_in *client_addr)
{
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr->sin_family = AF_INET;
	client_addr->sin_port = htons(PORT);
	client_addr->sin_addr.s_addr = INADDR_ANY;
	std::cout << "Server address initialized !\n";
}

int	server_start()
{
	std::string html = "<html><body>Hello, World!</body></html>";
	int listen_sock, conn_sock, ret, i;
	struct sockaddr_in client_addr;
	int		nfds = 1;
	struct pollfd fds[nfds];
	const char	*ip_address = "127.0.0.1";
	char buffer[1024];

	/* create a listening socket */

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Sock created : " << listen_sock << std::endl;
	// handle socket() error
	if (listen_sock < 0)
	{
		std::cout << strerror(errno) << std::endl;
		return 0;
	}
	/* define server adress */	
	init_client_addr(&client_addr);

	/* Bind socket to the port */
	std::cout << "binding socket to port\n";
	if (bind(listen_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
	{
		std::cout << "bind error\n";
		std::cout << strerror(errno) << std::endl;
		return 0;
	}

	/* start listening for connections. */
	ret = listen(listen_sock, MAX_CONNECTIONS);
	std::cout << "Started to listen and ret is " << ret << std::endl;

	/* set up the poll fds which will be used to listen on multiple fds for client connections*/
	memset(fds, 0, sizeof(fds));
	fds[0].fd = listen_sock;
	fds[0].events = POLLIN;
	std::cout << "pollfd set to listen sock\n";
// 	if (connect(listen_sock, (struct sockaddr*) &client_addr, sizeof(client_addr)) < 0) {
//     // Error connecting to server
//   }
	/* accept incoming connection */
	while (1)
	{
		/* setting up poll using pollfds, requested events and timeout as unlimited */
		std::cout << "Entered while loop before poll\n";
		ret = poll(fds, 1, -1);
		std::cout << "Poll has returned a value so has either had a response or failed\n";
		/* chekc to see if poll failed */
		if (ret < 0)
		{
			std::cout << "poll error\n" << std::endl;
			std::cout << strerror(errno) << std::endl;
			return 0;
		}
		/* check to see if poll timed out */
		if (ret == 0)
		{
			std::cout << "poll timed out\n" << std::endl;
		}
		/* see if descriptors equlas pollin */
		std::cout << "checking fds for poll revents\n";
		for (i = 0; i < 1; i++)
		{
			if (fds[i].revents == 0)
			{
				std::cout << "fd[i] revents is 0"  << std::endl;
				continue;
			}
			if (fds[i].fd == listen_sock)
			{
				std::cout << "listening socket is readable\n";
					conn_sock = accept(listen_sock, (struct sockaddr *) NULL, NULL);
				if (conn_sock < 0)
				{
					std::cout << strerror(errno) << std::endl;
					return 0;
				}
				std::cout << "Connection accpeted from" << inet_ntoa(client_addr.sin_addr) << " : " << ntohs(client_addr.sin_port) << std::endl;
			}
			std::cout << "New incoming connection from " << conn_sock << std::endl;
			char buffer[30000] = {0};
			long valread = read( conn_sock , buffer, 30000);
			(void) valread;
			//HERE VOIDING
			printf("%s\n",buffer );
			const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
			write(conn_sock , hello , strlen(hello));
			printf("------------------Hello message sent-------------------\n");
			close(conn_sock);
		}
		

	}
	std::cout << "connection accepted and conn sock is " << conn_sock << std::endl;

		close(conn_sock);


	// fds[0].fd = 0;
	// fds[0].events = POLLIN;

	
	// std::cout << POLLIN << " revents " << fds[0].revents << std::endl;
	// int poll_res = poll(fds, 1, timeout);

	// std::cout << POLLIN << " revents " << fds[0].revents << std::endl;
	// if (poll_res == 0) {
	// 	std::cout << "Poll timed out" << std::endl;
	// } else if (poll_res == -1) {
	// 	std::cout<< "Error while trying to poll" << std::endl;
	// } else {
	// 	if (fds[0].revents & POLLIN) {
	// 		std::cout << "Data available at stdin " << std::endl;
	// 	}
	// }

	(void) ip_address;
	(void) buffer;
	
	 return 0;
}

