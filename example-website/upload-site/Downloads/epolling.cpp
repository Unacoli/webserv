/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epolling.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoncalv <mgoncalv@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 12:42:44 by gefaivre          #+#    #+#             */
/*   Updated: 2023/02/02 16:53:49 by mgoncalv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <sys/epoll.h>
#include "Server.hpp"
#include "Client.hpp"

#define MAX_EVENTS 5
#define READ_SIZE 10

void epolling(std::vector<Server *> servers)
{ 
	int running = 1, event_count, i;
	struct epoll_event events[MAX_EVENTS];



	
	std::cout << "SALUT AFTER" <<servers.size() <<std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		struct epoll_event event;
		
		int epoll_fd = epoll_create1(0);
		if (epoll_fd == -1)
		{
			fprintf(stderr, "Failed to create epoll file descriptor\n");
			return;
		}
	
		servers[i]->setEpollFd(epoll_fd);
		servers[i]->setSocket();
	
		event.events = EPOLLIN;
		event.data.fd = servers[i]->getServerFd();

		
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i]->getServerFd(), &event))
		{
			fprintf(stderr, "Failed to add file descriptor to epoll\n");
			std::cout << "errno = " << errno << std::endl;
			close(epoll_fd);
			return;
		}
	}

	while (running)
	{
		for (size_t s = 0; s < servers.size(); s++)
		{
			event_count = epoll_wait(servers[s]->getEpollFd(), events, MAX_EVENTS, 0);
			for (i = 0; i < event_count; i++)
			{
				if (events[i].data.fd == servers[s]->getServerFd())
				{
					std::cout << "----------SERVER EVENT" << std::endl;
					servers[s]->newclient(servers[s]->getEpollFd());
				}
				else if (events[i].events & (EPOLLHUP | EPOLLRDHUP))
				{
					std::cout << RED << "EPOLLHUP" << reset << std::endl;
					servers[s]->deleteClient(events[i].data.fd);
				}
				else if (events[i].events & EPOLLIN)
				{
					std::cout << "----------EPOLLIN EVENT" << std::endl;
					servers[s]->clients[events[i].data.fd]->readRequest1();
				}
				else if (events[i].events & EPOLLOUT)
				{
					std::cout << "----------EPOLLOUT EVENT" << std::endl;
						if (servers[s]->clients[events[i].data.fd]->CreateAndSendResponse() == DELETE_CLIENT)
							servers[s]->deleteClient(events[i].data.fd);
				}		
			}
			if (s == servers.size())
				s = 0;
			bzero(events, MAX_EVENTS);
		}
	}
	for (size_t s = 0; s < servers.size(); i++)
	{
		if (close(servers[s]->getServerFd()))
		{
			fprintf(stderr, "Failed to close epoll file descriptor\n");
			return;
		}
	}
}
