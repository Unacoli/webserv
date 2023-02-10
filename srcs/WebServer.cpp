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
	std::map<int, Client> clients;
	int	client_fd = 0;
	int	ep_count = 0;

	struct epoll_event current_event[MAX_EVENTS];
	
	/* accept incoming connection */
	//std::cout << "\033[1m\033[33m Entering reactor loop \033[0m" << std::endl;
	while (1)
	{
		// std::cout << "IN EPOLL WAIT\n";
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);
		if (ep_count < 0)	
			error_handler("\tEPOLL WAIT ERROR\t");

		for (int i = 0; i < ep_count; i++)
		{
			// std::cout << "epoll count is " << ep_count 	<< std::endl;

			// std::cout << "📫 Signal received on fd " << current_event[i].data.fd << " and EP count = " << ep_count << std::endl;
			
			client_fd = is_incoming_connection(listen_socket, current_event, &conn_sock, epfd, i);			
			if (client_fd > 0)	
			{
				std::cout << "client fd is " << client_fd << std::endl;
				clients.insert(std::pair<int, Client>(client_fd, Client()));
				break ;
			}

			/* check if there was a disconnection or problem on fd						*/

			if (current_event[i].events & EPOLLRDHUP)
			{
				std::cout << "EPOLLRDHUP\n";
				client_disconnected(current_event, epfd, i, clients);	
			}
			else if (current_event[i].events & EPOLLOUT)
			{
				std::cout << "\n\nEPOLLOUT\n\n";
				send_client_response(current_event[i].data.fd, current_event, epfd, i, server_list, clients);
			}
			else if (current_event[i].events & EPOLLIN)
			{
				std::cout << "EPOLLIN \n";
				handle_client_request(current_event[i].data.fd, current_event, epfd, i, server_list, clients);
			}
		}	
	}
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



bool	WebServer::is_request_complete(std::string request)
{
	//Check if the HTTP request is complete
	if (request.find("\r") != std::string::npos)
		return true;
	return false;
}
