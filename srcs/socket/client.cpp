/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 11:47:15 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/16 16:00:49 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

int    createClient(int clientID)
{
    int                 sock = 0;
    long                valread;
    struct sockaddr_in  serv_addr;
    std::string         hello = "Hello from client";
    char                buffer[1024] = {0};
    
    if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("Socket Creation Error in client n°" + IntToStr(clientID));
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if ( inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0 )
        throw std::runtime_error("Invalid address or Address not supported in client n°" + IntToStr(clientID));

    if ( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 )
        throw std::runtime_error("Connection Failed for client n°" + IntToStr(clientID));
    send(sock, hello.c_str(), strlen(hello.c_str()),0);
    std::cout << "Hello message sent from client n°" << IntToStr(clientID) << std::endl;
    valread = read(sock, buffer, 1024);
    std::cout << "Client n°" << clientID << " received : " << buffer << std::endl;
    return (0);
}