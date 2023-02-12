#include "WebServer.hpp"

WebServer::WebServer()
{

}

WebServer::~WebServer()
{
	return ;
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

static volatile sig_atomic_t got_signal = 0;

void    ctrlc_normal(int num)
{
    (void)num;
    printf("Close Webserv\n");
    got_signal = 1;
}

static void sig_err(void)
{
    exit(0);
}

void    signal_handler(void)
{
    if (signal(SIGINT, ctrlc_normal) == SIG_ERR)
        sig_err();
}

void	WebServer::reactor_loop(int epfd, std::map<int, std::map<std::string, t_server> > server_list, std::vector<int> listen_socket)
{
	int conn_sock;
	std::map<int, Client> clients;
	int	client_fd = 0;
	int	ep_count = 0;

	struct epoll_event current_event[MAX_EVENTS];
	
	/* accept incoming connection */
	while (got_signal == 0)
	{
		signal_handler();
		ep_count = epoll_wait(epfd, current_event, MAX_EVENTS, -1);
		if (ep_count < 0)	
			error_handler("\tEPOLL WAIT ERROR\t");
		for (int i = 0; i < ep_count; i++)
		{
			/* check if there was a disconnection or problem on fd						*/
			if (current_event[i].events & EPOLLRDHUP)
			{
				client_disconnected(epfd, current_event[i].data.fd, clients);	
			}
			else if (current_event[i].events & EPOLLIN)
			{
				client_fd = is_incoming_connection(listen_socket, current_event, &conn_sock, epfd, i);			
				if (client_fd > 0)	
				{
					clients.insert(std::pair<int, Client>(client_fd, Client()));
					break ;
				}
				handle_client_request(current_event[i].data.fd, current_event, epfd, i, server_list, clients);
			}
			else if (current_event[i].events & EPOLLOUT)
			{
				send_client_response(current_event[i].data.fd, current_event, epfd, i, server_list, clients);
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

	host = host.substr(0, pos);
	getsockname(fd, (struct sockaddr *)&addr, &addr_len);
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
	return (server.begin())->second;
}



bool	WebServer::is_request_complete(std::string request)
{
	//Check if the HTTP request is complete
	if (request.find("\r") != std::string::npos)
		return true;
	return false;
}
