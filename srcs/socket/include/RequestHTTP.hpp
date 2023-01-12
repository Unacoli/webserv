/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHTTP.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: barodrig <barodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/12 14:30:37 by barodrig          #+#    #+#             */
/*   Updated: 2023/01/12 16:13:29 by barodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHTTP_HPP
# define REQUESTHTTP_HPP

# include "main.hpp"

class   RequestHTTP{
	
	public :
		enum Method { GET, POST, DELETE, UNKNOWN };
		
		RequestHTTP();
		RequestHTTP(const RequestHTTP &src);
		RequestHTTP(const std::string& request);
		~RequestHTTP();

		std::string		getMethod() const;
		std::string 	getURI() const;
		std::string 	getHTTPVersion() const;
		std::string 	getHeaders() const;
		std::string		getHeader(const std::string& key) const;
		std::string 	getBody() const;

	private :
		Method								_method;
		std::string							_uri;
		std::string 						_version;
		std::string 						_body;
		std::map<std::string, std::string> 	_headers;

		void	parseRequest(const std::string& request);
		void    parseHeaders( std::vector<std::string> &headers );
};

std::ostream &operator<<(std::ostream &o, const RequestHTTP &i);

#endif
