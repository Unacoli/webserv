/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 15:49:19 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 12:06:33 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "main.hpp"

std::string const               IntToStr( int nbr );
std::string const               SizeToStr( size_t nbr );
std::vector<std::string>        LineToWords ( std::string str );
size_t                          StrToSize( std::string const &str );

#endif