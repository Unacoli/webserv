/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 10:23:42 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 11:59:59 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

int	    server_start();
void	init_server_addr(struct sockaddr_in *serv_addr);

//Client tester
int    createClient(int clientID);

#endif