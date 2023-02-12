#include "main.hpp"

void     ft_launcher( std::string confpath, WebServer &WebServer )
{
    Config  config;
    
    try
    {
        config.FileOpenerChecker(confpath, &config);
    }
    catch(const std::exception& e)
    {
        std::cerr << "webserv: " << e.what() << '\n';
        return ;
    }
    try
    {
        WebServer.handle_servers(config.server);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
    return ;
}

int     main(int ac, char **av)
{
    std::string confpath;
    WebServer   WebServer;
    
    if (ac > 2)
    {
        std::cerr << "webserv: only one configuration file is allowed at once." << std::endl;
        return (ERROR);
    }
    if (ac == 1)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            confpath = cwd + std::string("/conf/simple.conf");
        else
            return (ERROR);
        std::cerr << "webserv: did not find any config file and choose a default one." << std::endl; 
    }
    else
    {
        if ( !std::strlen(av[1]) )
        {
            std::cerr << "webserv: config file's path can't be an empty string." << std::endl;
            return (ERROR);
        }
        confpath = av[1];
    }
    ft_launcher(confpath, WebServer);
    return (SUCCESS);
}