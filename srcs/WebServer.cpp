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
		event.events = EPOLLIN | EPOLLRDHUP;
		/*  epoll_ctl. This is the function that allows you to add, modify and delete file */
		/*descriptors from the list that a particular epoll file descriptor is watching. */
		if (epoll_ctl(*epfd, EPOLL_CTL_ADD, *it, &event) == -1)
			error_handler("\tEPOLL CTL ERROR\t");	
		
	}
}

void WebServer::add_fd_to_poll(int fd, fd_set *fds)
{
	FD_SET(fd, fds);
	if (this->max_fd < fd)
		this->max_fd = fd;
}

void WebServer::run_select_poll(fd_set *reads, fd_set *writes)
{
	int ret = 0;

	if ((ret = select(this->max_fd + 1, reads, writes, 0, 0)) < 0)
		exit(1);
	else if (ret == 0)
		std::cout << "[ERROR] select() timeout" << std::endl;
	this->reads = *reads;
	this->writes = *writes;
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
		//std::cout << "Sock created : " << listen_sock << std::endl;
		if (listen_sock < 0)
			error_handler("Socket Creation Error\n");
		ret = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (ret < 0)
			error_handler("set sock opt error\n");
		make_socket_non_blocking(listen_sock);
		
		memset(&client_addr, 0, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(it->first);
		client_addr.sin_addr.s_addr = INADDR_ANY; // should I initialize host here ?

		//std::cout << "binding socket to port " << it->first << std::endl;
		if (bind(listen_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
				error_handler("\tBIND ERROR\t\n");
		getsockname(listen_sock, (struct sockaddr *) &client_addr, &client_len);

		// print the port number
		//std::cout <<  "Socket binded to port no : " << ntohs(client_addr.sin_port) << " at server : " << client_addr.sin_addr.s_addr <<  "listen sock is " << listen_sock << std::endl;

		if ((listen(listen_sock, MAX_CONNECTIONS)) < 0)
			error_handler("\tLISTEN ERROR\t\n");
		
		listen_sock_array.push_back(listen_sock);
	}
	return listen_sock_array;
	
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
	int	flag = 0;
	int	ep_count = 0;

	struct epoll_event current_event[MAX_EVENTS];
	
	/* accept incoming connection */
	//std::cout << "\033[1m\033[33m Entering reactor loop \033[0m" << std::endl;
	while (1)
	{
		/* setting up poll using pollfds, requested events and timeout as unlimited */
		//std::cout << "📡 Activating poll using epoll fd : " << epfd << " and EP count = " << ep_count << std::endl;
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);
<<<<<<< HEAD
		if (ep_count < 0)
			error_handler("\tEPOLL WAIT ERROR\t\n");
=======
		if (ep_count < 0)	// std::cout << "AFTER recv and ret is " << valread << std::endl;
	// std::cout << strerror(errno) << std::endl;
	// std::cout << "Buffer is " << buffer << std::endl;
			error_handler("\tEPOLL WAIT ERROR\t");
>>>>>>> clmurphy

		/* Epoll wait has stopped waiting which means it has recieved a signal 		*/
		/* There we are going to loop through the fds it's watching and see which	*/
		/* one received a signal and thn handle it 									*/


		for (int i = 0; i < ep_count; i++)
		{
			//std::cout << "📫 Signal received on fd " << current_event[i].data.fd << " and EP count = " << ep_count << std::endl;

			/* check if its a new connection and then accept it and add it the epoll list */
			flag = is_incoming_connection(listen_socket, current_event, &conn_sock, epfd, i);			
			if (flag == 1)	
				continue ;

			/* check if there was a disconnection or problem on fd						*/

			if (current_event[i].events & EPOLLRDHUP)
			{
				std::cout << "EPOLLRDHUP\n";
				client_disconnected(current_event, epfd, i);	
			}
			else if (current_event[i].events & EPOLLIN)
			{
				std::cout << "EPOLLIN \n";
				handle_client_request(current_event, epfd, i, server_list);
			}
			else if (current_event[i].events & EPOLLOUT)
				std::cout << "\n\nEPOLLOUT\n\n";
		}	
	}
}

void	WebServer::handle_client_request(struct epoll_event *current_event, int epfd, int i, std::map<int, std::map<std::string, t_server> > server_list)
{
	int			ret = 0;
	int			ret_send;
	std::vector<std::string> string_vec;
	long valread;
	//std::cout << "\033[1m\033[35m \n Entering EPOLLIN and fd is "<< current_event[i].data.fd <<"\033[0m\n" << std::endl;

	char buffer[30000] = {0};
	char *save = NULL;

	/* Read HTTP request recieved from client 						*/

		valread = recv( current_event[i].data.fd , buffer, 30000, 0);
		usleep(1);
		if (valread < 0 )
		{
			std::cout << strerror(errno) << std::endl;
		}
		else if (valread == 0)
		{
			std::cout << "recv disconnect client" << std::endl;
			client_disconnected(current_event, epfd, i);
			return ;
		}
		save = (char *)malloc(sizeof(buffer));
		strcpy(save, buffer);
		string_vec.push_back(std::string(save));
		std::cout << "AFTER recv and ret is " << valread << std::endl;
		std::cout << "\033[1m\033[33mBuffer is \n" << buffer << std::endl;
		std::cout << "\nEND OF BUFFER \033[0m\n" << std::endl;

	

	/* handle HTTP request	*/

	//std::cout << "\033[1m\033[31mBuffer sent to request is [ \n" << result << "\n ] \033[0m\n" << std::endl;
	RequestHTTP request(buffer);
	// std::cout << "\033[1m\033[32mREQUEST  is " << request << "\033[0m\n" << std::endl;

	t_server server = find_server(server_list, request._headers["Host"], current_event[i].data.fd);
	if (checkMaxBodySize(valread, server, request) == 1)
	{
		std::cerr << "MAX BODY SIZE REACHED" << std::endl;
		ResponseHTTP response;
		response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
		std::cout << "RESPONSE IS " << response.getResponse() << std::endl;
		ret_send = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
		if (ret_send < 0)
		{
			client_disconnected(current_event, epfd, i);;
			read_error_handler("Send error\n");
		}
	}
	else
	{
		std::cout << "ENTERING LOOP AFTER SEND\n";
		while (valread > 0 && request.isComplete() == false)
		{
			std::cout << "In loop !\n";
			sleep(1);
			std::cerr << "BODY IS BEFORE BZERO =\n" << request << std::endl;
			bzero(buffer, 30000);
			valread = recv( current_event[i].data.fd , buffer, 30000, 0);
			std::cout << "\033[1m\033[35mAFTER recv 2 and ret is " << valread << std::endl;
			std::cout << strerror(errno) << std::endl;
			// std::cout << "\033[1m\033[35mBuffer is \n" << buffer << std::endl;
			// std::cout << "\nEND OF BUFFER \033[0m\n" << std::endl;
			// if(valread < 0)
			// {
			// 	//client_disconnected(current_event, epfd, i);
			// 	return ;
			// }
			if (valread == 0)
			{
				client_disconnected(current_event, epfd, i);
				free(dynamic_buffer);
				return ;
			}
			request.appendBody(buffer);
			std::cerr << "BODY IS AFTER BZERO:\n" << request << "\033[0m" << std::endl;
			if (checkMaxBodySize(valread, server, request) == 1)
			{
				std::cout << " In chck max body size\n";
				ResponseHTTP response;
				response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
				ret_send = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
				if (ret_send < 0)
				{
					client_disconnected(current_event, epfd, i);
					free(dynamic_buffer);
					read_error_handler("Send error\n");
				}
				//std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
				free(dynamic_buffer);
				return ;
			}
			if (tmprequest.isComplete() == true)
				break ;
			return ;
			valread = recv( current_event[i].data.fd , buffer, 30000, 0);
		}
	}
	RequestHTTP request(dynamic_buffer);
	/* generate response to HTTP request 	*/	
	//std::cerr << "REQUEST IS =\n" << request << std::endl;
	ResponseHTTP response(request, server);
	//std::cerr << "RESPONSE IS =\n" << response << std::endl;
	/* Send HTTP response to server						*/
	/* Loop is needed here to ensure that the entirety 	*/
	/* of a large file will be sent to the client 		*/
	std::cout << "RESPONSE : " << response.getResponse() << std::endl;
	std::cout << "Here\n" << std::endl;
	int error_ret = 0;
	if (ret != (int)response.getResponse().length())
	{
		while (ret < (int)response.getResponse().length())
		{
			std::cout << "sending response ! \n";
			error_ret = send(current_event[i].data.fd , response.getResponse().c_str() + ret , response.getResponse().length() - ret, 0);
			if (error_ret < 0)
			{
				client_disconnected(current_event, epfd, i);
				free(dynamic_buffer);
				read_error_handler("Send error\n");
			}
			ret +=  error_ret;
		}
	 }
	//std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
}

void	WebServer::client_disconnected(struct epoll_event *current_event, int epfd, int i)
{
	std::cout << " ⛔️ Client fd " << current_event[i].data.fd << " has disconnected\n";
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
			std::cout << " 🔌 New incoming connection from " << inet_ntoa(cli_addr.sin_addr) << " on " << *conn_sock << " on port " << ntohs(cli_addr.sin_port) << std::endl;
			make_socket_non_blocking(*conn_sock);
			current_event->data.fd = *conn_sock;
			current_event->events = EPOLLIN | EPOLLET ;
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
	if(pos >= 0)
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

void WebServer::make_socket_non_blocking(int socket_fd)
{
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
		error_handler("\tFCNTL ERROR\t");
}

bool	WebServer::is_request_complete(std::string request)
{
	//Check if the HTTP request is complete
	if (request.find("\r") != std::string::npos)
		return true;
	return false;
}
