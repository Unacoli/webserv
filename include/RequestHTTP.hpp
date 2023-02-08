#ifndef REQUESTHTTP_HPP
# define REQUESTHTTP_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <stdlib.h>
# include "utils.hpp"
# include "ResponseHTTP.hpp"

typedef std::basic_string<char> string;
class ResponseHTTP;

class   RequestHTTP{
	
	public :
		enum Method { GET, POST, DELETE, UNKNOWN };
		
		RequestHTTP();
		RequestHTTP(const RequestHTTP &src);
		RequestHTTP(const string& request);
		~RequestHTTP();

		RequestHTTP &operator=(const RequestHTTP &rhs);
			
		std::map<string, string> 	_headers;
		string 						_body;	
		string		getMethod() const;
		string 	getURI() const;
		string 	getHTTPVersion() const;
		string 	getHeaders() const;
		string		getHeader(const string& key) const;
		string 	getBody() const;
		string		getPath();
		string		getQuery();
		string		getCgi_info(string &extension);
		int             getClient_fd();
		string     getPort();
		size_t          getContentLength() const;
		bool            isComplete() const;
		bool			headers_received;

		void			appendBody(const string& body);
		void			reinit();
		void			parseRequest(const string& request);

	private :
		Method								_method;
		string							_uri;
		string 						_version;

		std::map<string, string>	_cgi_info;
		string							_path;
		int                                 _client_fd;
		string						    _full_request;

		void    		parseHeaders( std::vector<string> &headers );
};

std::ostream &operator<<(std::ostream &o, const RequestHTTP &i);

#endif
