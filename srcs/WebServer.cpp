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

std::vector<int> WebServer::init_socket(std::map<int, t_server> server_list)
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

void	WebServer::handle_servers(std::vector<t_server> servers)
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
	init_poll(&epfd, listen_sock_array);
	reactor_loop(epfd, servers_list, listen_sock_array);
}


void	WebServer::reactor_loop(int epfd,std::map<int, t_server> server_list, std::vector<int> listen_socket)
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
		}	
	}
}

void	WebServer::handle_client_request(struct epoll_event *current_event, int epfd, int i, std::map<int, t_server> server_list)
{
	size_t			ret = 0;
	std::cout << "\033[1m\033[35m \n Entering EPOLLIN and fd is "<< current_event[i].data.fd <<"\033[0m\n" << std::endl;
	char buffer[30000] = {0};

	/* Read HTTP request recieved from client 						*/

	long valread = recv( current_event[i].data.fd , buffer, 30000, 0);
	if (valread < 0 )
		read_error_handler("Recv error\n");
	if (valread == 0)
	{
		client_disconnected(current_event, epfd, i);
		return ;
	}
	/* handle HTTP request	*/
	RequestHTTP request(buffer);
	t_server server = find_server(server_list, current_event[i].data.fd);
	if (checkMaxBodySize(valread, server, request) == 1)
	{
		ResponseHTTP response;
		response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
		ret = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
		if (ret < 0)
		{
			client_disconnected(current_event, epfd, i);
			read_error_handler("Send error\n");
		}
		std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
	}
	else
	{
		while (valread > 0 && request.isComplete() == false)
		{
			valread = recv( current_event[i].data.fd , buffer, 30000, 0);
			if(valread < 0)
				read_error_handler("Recv error\n");
			if (valread == 0)
			{
				client_disconnected(current_event, epfd, i);
				return ;
			}
			request.appendBody(buffer);
			if (checkMaxBodySize(valread, server, request) == 1)
			{
				ResponseHTTP response;
				response.sendError(ResponseHTTP::REQUEST_ENTITY_TOO_LARGE);
				ret = send(current_event[i].data.fd , response.getResponse().c_str() , response.getResponse().length(), 0);
				if (ret < 0)
				{
					client_disconnected(current_event, epfd, i);
					read_error_handler("Send error\n");
				}
				std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
				return ;
			}
		}
	}
	/* generate response to HTTP request 	*/	
	ResponseHTTP response(request, server);
	/* Send HTTP response to server						*/
	/* Loop is needed here to ensure that the entirety 	*/
	/* of a large file will be sent to the client 		*/
<<<<<<< HEAD
	//std::cout << "RESPONSE : " << response.getResponse() << std::endl;
	int error_ret = 0;
=======
	//std::cerr << "RESPONSE IS =\n" << response.getResponse() << "\n";
>>>>>>> 9cde7b0abd0b4631d83634447495dfe3eb5d4014
	if (ret != response.getResponse().length())
	{
		while (ret < response.getResponse().length())
		{
			error_ret = send(current_event[i].data.fd , response.getResponse().c_str() + ret , response.getResponse().length() - ret, 0);
			if (error_ret < 0)
			{
				client_disconnected(current_event, epfd, i);
				read_error_handler("Send error\n");
			}
			ret +=  error_ret;
		}
	}
	std::cout << "\033[1m\033[33m 📨 Server sent message to client on fd" << current_event[i].data.fd << " \033[0m" << std::endl;
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
			current_event->events = EPOLLIN;
			epoll_ctl(epfd, EPOLL_CTL_ADD, *conn_sock, current_event);
			return 1;
		}
	}
	return 0;
}

t_server	WebServer::find_server(std::map<int, t_server> server_list, int fd)
{
	struct sockaddr_in addr;
	socklen_t		addr_len = sizeof(addr);

	getsockname(fd, (struct sockaddr *)&addr, &addr_len);
	std::cout << "IN FIND SERVER AND PORT IS : "<< htons(addr.sin_port) << std::endl;
	return server_list[htons(addr.sin_port)];
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

void                WebServer::Cgi_GET_resp(ResponseHTTP &resp, std::string &cgi_ret)
{
	std::stringstream ss(cgi_ret);
	size_t tmpi;
	std::string tmp;
	std::string body;

//	resp.appendHeader("Server", /*insert server name*/);
	while(getline(ss, tmp, '\n'))
	{
		if (tmp.length() == 1 && tmp[0] == '\r')
			break ;
		size_t mid = tmp.find(":");
		size_t end = tmp.find("\n");
		if (tmp[end] == '\r')
		{
			tmp.erase(tmp.length() - 1, 1);
			end -= 1;
		}
		if ((tmpi = tmp.find(";")) != std::string::npos)
			tmp = tmp.substr(0, tmpi);
		std::string key = tmp.substr(0, mid);
		std::string value = tmp.substr(mid + 1, end);
		resp.appendHeader(key, value);
	}
	while (getline(ss, tmp, '\n'))
	{
		body += tmp;
		body += "\n";
	}
	resp.appendBody(body);
	resp.appendHeader("Content-Length", IntToStr(resp.getBody().size()));
}

void                WebServer::Cgi_POST_resp(ResponseHTTP &resp, std::string &cgi_ret, RequestHTTP &req)
{
	std::stringstream ss(cgi_ret);
	size_t tmpi;
	std::string tmp;
	std::string body;

//	resp.appendHeader("Server", /*insert server name*/);
	while (getline(ss, tmp, '\n'))
	{
		if (tmp.length() == 1 && tmp[0] == '\r')
			break ;
		size_t mid = tmp.find(":");
		size_t end = tmp.find("\n");
		if (tmp[end] == '\r')
		{
			tmp.erase(tmp.length() - 1, 1);
			end -= 1;
		}
		if ((tmpi = tmp.find(";")) != std::string::npos)
			tmp = tmp.substr(0, tmpi);
		std::string key = tmp.substr(0, mid);
		std::string value = tmp.substr(mid + 1, end);
		resp.appendHeader(key, value);
	}
	while (getline(ss, tmp, '\n'))
	{
		body += tmp;
		body += "\n";
	}

	std::string full_path = req.getPath();
	size_t index = full_path.find_last_of("/");
	if (index == std::string::npos)
	{
		resp.sendError(ResponseHTTP::INTERNAL_SERVER_ERROR);
		return;
	}

	std::string file_name = full_path.substr(index + 1);
	std::string folder_path = full_path.substr(0, index);

	std::string command = "mkdir -p " + folder_path;
	system(command.c_str());
	int write_fd = open(full_path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (write_fd < 0)
	{
		resp.sendError(ResponseHTTP::INTERNAL_SERVER_ERROR);
		return;
	}

	this->add_fd_to_poll(write_fd, &(this->writes));
	this->run_select_poll(&(this->reads), &(this->writes));
	if (FD_ISSET(write_fd, &(this->writes)) == 0)
	{
		resp.sendError(ResponseHTTP::INTERNAL_SERVER_ERROR);
		close(write_fd);
		return;
	}

	int r = write(write_fd, body.c_str(), body.size());
	if (r < 0)
	{
		resp.sendError(ResponseHTTP::INTERNAL_SERVER_ERROR);
		close(write_fd);
		return;
	}
	else if (r == 0)
	{
		resp.sendError(ResponseHTTP::INTERNAL_SERVER_ERROR);
		close(write_fd);
		return;
	}
	close(write_fd);

	resp.appendHeader("Content-Length", IntToStr(resp.getBody().size()));
}

static void kill_child_process(int sig)
{
    (void) sig;
    kill(-1, SIGKILL);
}

int                 WebServer::send_Cgi_resp(Cgi &cgi, RequestHTTP &req)
{
	this->add_fd_to_poll(cgi.getPipe_write(), &(this->writes));
	this->run_select_poll(&(this->reads), &(this->writes));
	if (FD_ISSET(cgi.getPipe_write(), &(this->writes)) == 0)
	{
		std::cerr << "[ERROR] writing to cgi failed" << std::endl;
		signal(SIGALRM, kill_child_process);
		alarm(30);
		signal(SIGALRM, SIG_DFL);
		close(cgi.getPipe_read());
		close(cgi.getPipe_write());
		return 500;
	}
	cgi.write_Cgi();
	FD_ZERO(&this->writes);
	this->add_fd_to_poll(cgi.getPipe_read(), &(this->reads));
	this->run_select_poll(&(this->reads), &(this->writes));
	if (FD_ISSET(cgi.getPipe_read(), &(this->reads)) == 0)
	{
		std::cerr << "[ERROR] reading from cgi failed" << std::endl;
		signal(SIGALRM, kill_child_process);
		alarm(30);
		signal(SIGALRM, SIG_DFL);
		close(cgi.getPipe_read());
		close(cgi.getPipe_write());
		return 500;
	}
	std::string cgi_ret = cgi.read_Cgi();
	if (cgi_ret.empty())
		return 500;
	close(cgi.getPipe_read());
	close(cgi.getPipe_write());
	std::cout << "cgi successfully read" << std::endl;
	if (cgi_ret.compare("cgi: failed") == 0)
		return 400;
	else
	{
		if (req.getMethod() == "GET")
		{
			std::string code = getStatus_Cgi(cgi_ret);
			if (code.empty())
				return 502;
			ResponseHTTP res; //need a constructor that take the status code in parameter
			Cgi_GET_resp(res, cgi_ret);
			res.responseMaker();
			int send_ret = send(req.getClient_fd(), res.getResponse().c_str(), res.getResponse().size(), 0);
			if (send_ret < 0)
				return 500;
			else if (send_ret == 0)
				return 400;
			else
				std::cout << "cgi responded" << std::endl;
		}
		if (req.getMethod() == "POST")
		{
			std::string code = getStatus_Cgi(cgi_ret);
			if (code.empty())
				return 502;
			ResponseHTTP res;
			Cgi_POST_resp(res, cgi_ret, req);
			res.responseMaker();
			int send_ret = send(req.getClient_fd(), res.getResponse().c_str(), res.getResponse().size(), 0);
			if (send_ret < 0)
				return 500;
			else if (send_ret == 0)
				return 400;
			else
				std::cout << "cgi responded" << std::endl;
			std::cout << cgi_ret << std::endl;			
		}
	}
	return 0;
}

std::string			WebServer::getStatus_Cgi(std::string &cgi_ret)
{
	std::string status_line;
	std::stringstream ss(cgi_ret);

	std::string line;
	while (getline(ss, line, '\n'))
	{
		if (line.substr(0, 6) == "Status")
			status_line = line;
	}
	if (status_line.empty())
		return status_line;
	cgi_ret.erase(0, status_line.length() + 1);
	status_line.erase(0, 8);
	status_line.erase(status_line.length() - 1, 1);
	return status_line;
}