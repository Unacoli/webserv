#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <map>
# include <vector>
# include <fstream>
# include <iostream>
# include "configDataStruct.hpp"
# include "parsing.hpp"
# include "utils.hpp"

# define SERVER 1
# define LOCATION 2

typedef struct s_server t_server;

class Config
{
    public:
        std::fstream                *file;
        std::vector<t_server>       server;
        std::vector<t_server_block> server_blocks;
        
 
        Config();
        Config( Config const & src );
        ~Config();
        
        Config &    operator=( Config const & rhs );

        void        FileOpenerChecker(std::string confpath, Config *config);
        void        SyntaxChecker( Config *config );
        size_t      ServerHandler( std::string first, size_t line_nb, Config *config );
        size_t      LocationHandler( std::string first, size_t line_nb, t_server_block *serv, Config *config );
        void        MultiHandler( Config *config );
        void        CheckSemiColons( Config *config );
        void        RemoveSemiColons( Config *config );
        void        IsServerEnough( const Config &config );
};

std::ostream &  operator<<( std::ostream & o, Config const & i );

#endif