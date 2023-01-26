#include "main.hpp"

void     ft_launcher( std::string confpath, WebServer &WebServer )
{
    Config  config;
    // int      server_status = 0;
    // pid_t    pid_client = 0;
    // pid_t    pid_server = 0;
    
    try
    {
        config.FileOpenerChecker(confpath, &config);
        //std::cout << config << std::endl;
        // pid_server = fork();
        // if (pid_server == 0)
        // {
            WebServer.handle_servers(config.server);
        //     exit(0);
        // }
        // else
        // {
        //     sleep(1);
        //     for (int i = 0; i < 0; i++)
        //     {
        //         pid_client = fork();
        //         if (pid_client == 0)
        //         {
        //             createClient(i);
        //             exit(0);
        //         }
        //     }
        // }
        // waitpid(pid_server, &server_status, 0);
    }
    catch(const std::exception& e)
    {
        std::cerr << "webserv: " << e.what() << '\n';
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
        confpath = DEFAULT_PATH;
        std::cerr << "webserv: did not find any config file and chose a default one." << std::endl; 
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