/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 15:49:19 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 19:50:11 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "main.hpp"

std::string const               SizeToStr( size_t nbr );
std::vector<std::string>        LineToWords ( std::string str );
size_t                          StrToSize( std::string const &str );

#endif