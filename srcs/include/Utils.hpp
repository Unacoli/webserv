/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 15:49:19 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/16 17:14:16 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "main.hpp"

/*
** File System Operation
*/
bool                            isDirectory(std::string path);
bool                            fileExists(std::string path);
bool                            isReadable(std::string path);
std::string                     readFile(std::string path);                    

/*
** Variable Operation
*/
std::string const               IntToStr( int nbr );
std::string const               SizeToStr( size_t nbr );
std::vector<std::string>        LineToWords ( std::string str );
size_t                          StrToSize( std::string const &str );
void                            split(const std::string& s, char delim, std::vector<std::string>& parts);
std::string                     trim(const std::string &str);

#endif