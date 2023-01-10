/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 19:35:03 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/10 20:07:31 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP

#include "UtilsLibraries.hpp"

typedef struct s_parsing
{
    std::vector<t_server_block> server_blocks;
}            t_parsing;

typedef struct s_server_block
{
    std::vector<t_server_parse> server_lines;
    std::vector<t_location_block> location_blocks;
    bool    is_ended;
}            t_server_block;

typedef struct s_location_block
{
    std::vector<t_location_parse> location_lines;
    bool    is_ended;
}            t_location_block;

typedef struct s_server_parse
{
    // *line contain a line split into words.
    std::vector<std::string> words;
    int         token;
    int         line_number;
}               t_server_parse;

typedef struct s_location_parse
{
    // *line contain a line split into words.
    std::vector<std::string> words;
    int         token;
    int         line_number;
}               t_location_parse;

#endif