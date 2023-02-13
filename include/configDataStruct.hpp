#ifndef CONFIGDATASTRUCT_HPP
# define CONFIGDATASTRUCT_HPP

# include "Config.hpp"

typedef struct s_listen {
		std::string    port;
		std::string    ip;
}               t_listen;

typedef struct s_location{
		std::string                         	path;               // Location path. All the requests that match this path will be redirected to this location.
		std::string                         	root;               // Define HTTP redirection path for this root.
		std::vector<std::string>            	index;              // List of file to be checked for index. They should be read in the specified order as Nginx does if the request is a directory.
		std::vector<std::string>            	methods;            // HTTP methods allowed in the server (GET, POST, ...)
		std::vector<std::string>    			redirects;          // Map redirection codes with the uri of its page.

		long		                        	client_body_size;   // Default is 1M in Nginx. It defines the files upload size limit for a client body request.
		int										client_body_append; // Default is false in Nginx. It defines if the client body should be appended to the file or not.

		std::string                         	upload_path;        // Path to upload files if upload_status is on and the HTTP method post is enabled.
		bool                                	upload_status;      // Does this server location enabled upload (ex : "upload_status on").

		int	                                	autoindex;          // Enable or disable the directory listing output.
		
		std::map<size_t, std::string>       	errors;             // Map error codes with the uri of its page.
		
		std::map<std::string, std::string>  	cgi;				// Map extensions with the path of the cgi program.
}               t_location;

typedef struct s_server{
		std::vector<t_listen>               	listen;             // Ports and hosts of the server
		std::vector<std::string>            	server_names;       // Define virtual server names, the first one is the primary server name as in Nginx.
		
		t_location                          	default_serv;       // These are all the default information about the server that can be overwritten by the location directives.
		std::vector<t_location>             	locations;          // List of all the location directives. 
		
}               t_server;

#endif