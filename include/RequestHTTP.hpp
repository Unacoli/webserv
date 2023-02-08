#ifndef REQUESTHTTP_HPP
# define REQUESTHTTP_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <stdlib.h>
# include "utils.hpp"
# include "ResponseHTTP.hpp"

class ResponseHTTP;

class   RequestHTTP {
	
	public :
		enum Method { GET, POST, DELETE, HEAD, PUT, CONNECT, OPTIONS, TRACE, PATCH, UNKNOWN };

		RequestHTTP();
		RequestHTTP(const RequestHTTP &src);
		RequestHTTP(const std::string& request);
		~RequestHTTP();

		RequestHTTP &operator=(const RequestHTTP &rhs);
			
		std::map<std::string, std::string> 	_headers;
		std::string 						_body;	
		RequestHTTP::Method 				getMethod() const;
		std::string 						getURI() const;
		std::string 						getHTTPVersion() const;
		std::string 						getHeaders() const;
		std::string							getHeader(const std::string& key) const;
		std::string 						getBody() const;
		std::string							getPath();
		std::string							getQuery();
		std::string							getCgi_info(std::string &extension);
		int             					getClient_fd();	
		std::string     					getPort();
		size_t          					getContentLength() const;
		bool            					isComplete() const;
		bool								headers_received;
		std::string 						getMethodString() const;
		std::string 						getFullRequest() const;

		void								appendBody(const std::string& body);
		void								reinit();
		void								parseRequest(const std::string& request);

	private :
		Method							_method;
		std::string							_uri;
		std::string 							_version;

		std::map<std::string, std::string>		_cgi_info;
		std::string							_path;
		int                             _client_fd;
		std::string						    _full_request;

		void    		parseHeaders( std::vector<std::string> &headers );
};

std::ostream &operator<<(std::ostream &o, const RequestHTTP &i);

#endif
