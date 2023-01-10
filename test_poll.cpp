#include <poll.h>
#include <iostream>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
 #include <arpa/inet.h>
# include <cerrno>
# include <string.h>

# define PORT 8080
# define MAX_CONNECTIONS 1
# define TIMEOUT 5

int main()
{
	std::string html = "<html><body>Hello, World!</body></html>";
	int listen_sock, conn_sock, ret, i;
	struct sockaddr_in serv_addr;
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
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Bind socket to the port */
	if (bind(listen_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
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

// 	if (connect(listen_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
//     // Error connecting to server
//   }
	/* accept incoming connection */
	while (1)
	{
		/* setting up poll using pollfds, requested events and timeout as unlimited */
		ret = poll(fds, 1, -1);
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
			}
			std::cout << "New incoming connection from " << conn_sock << std::endl;
			char buffer[30000] = {0};
			long valread = read( conn_sock , buffer, 30000);
			printf("%s\n",buffer );
			const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
			write(conn_sock , hello , strlen(hello));
			printf("------------------Hello message sent-------------------");
			close(conn_sock);
		}
	

	}
	std::cout << "connection accepted and conn sock is " << conn_sock << std::endl;
	
	send(conn_sock, html.c_str(), html.length(), 0);

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
	 return 0;
}

