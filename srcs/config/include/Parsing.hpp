/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 19:35:03 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/11 11:30:13 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP

# include "Config.hpp"
# include "ConfigDataStruct.hpp"

typedef struct s_line
{
    std::vector<std::string> words;
    int         token;
    int         line_number;
}               t_line;

typedef struct s_location_block
{
    std::vector<t_line> location_lines;
    bool    is_ended;
}            t_location_block;


typedef struct s_server_block
{
    std::vector<t_line> server_lines;
    std::vector<t_location_block> location_blocks;
    bool    is_ended;
}            t_server_block;
#endif