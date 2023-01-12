/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 15:50:23 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 12:06:16 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

std::string const                  IntToStr( int nbr )
{
    std::stringstream ss;
    ss << nbr;
    return ss.str();
}

std::string const                   SizeToStr( size_t nbr )
{
    std::ostringstream oss;
    oss << nbr;
    return oss.str();
}

size_t                              StrToSize( std::string const &str )
{
    size_t nbr = static_cast<size_t>(std::atol(str.c_str()));
    return (nbr);
}

std::vector<std::string>            LineToWords ( std::string str )
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find_first_of(" \t");
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty())
            tokens.push_back(token);
        start = str.find_first_not_of(" \t", end);
        end = str.find_first_of(" \t", start);
    }
    std::string token = str.substr(start, end);
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}