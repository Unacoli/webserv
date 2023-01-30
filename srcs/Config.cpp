#include "Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Config::Config()
{
}

Config::Config( const Config & src )
{
    this->server = src.server;         //We'll see if we need to enter further in details for deep copy no idea for now
    *this = src;
}

/*
** ------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Config &				Config::operator=( Config const & rhs )
{
	if ( this != &rhs )
		this->server = rhs.server;
	return *this;
}

std::ostream &                operator<<( std::ostream & o, Config const & i )
{
    for ( std::vector<t_server>::const_iterator server = i.server.begin();
            server != i.server.end(); server++ )
    {
        o << "- - - - - - - SERVER CONFIGURATION " << server->server_names[0] << " - - - - - - -" << std::endl\
            << "- Port = " + server->listen.port << std::endl\
            << "- Host = " + server->listen.host << std::endl\
            << "- Server Name(s) = ";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            for ( std::vector<std::string>::const_iterator str = server->server_names.begin();
                    str != server->server_names.end(); str++ )
                o << *str << ' ';
            o << std::endl << "- Server root = " << server->default_serv.root << std::endl\
            << "- Error codes : " << std::endl;
            for (std::map<size_t, std::string>::const_iterator it = server->default_serv.errors.begin();
                    it != server->default_serv.errors.end(); it++)
                o << "Code = " << it->first << " > URI = " << it->second << std::endl;
            o << "- Server index = ";
            for ( std::vector<std::string>::const_iterator str = server->default_serv.index.begin();
                    str != server->default_serv.index.end(); str++ )
                o << *str << ' ';
            o << std::endl << "- Server HTTP Methods = " ;
            for ( std::vector<std::string>::const_iterator method = server->default_serv.methods.begin();
                    method != server->default_serv.methods.end(); method++ )
                o << *method << " ";
            o << std::endl << "- Server CGI Extension = ";
            for ( std::vector<std::string>::const_iterator ext = server->default_serv.cgi_extension.begin();
                    ext != server->default_serv.cgi_extension.end(); ext++ )
                o << *ext << " ";
            o << "\n- Server CGI Path = ";
            for ( std::vector<std::string>::const_iterator path = server->default_serv.cgi_path.begin();
                    path != server->default_serv.cgi_path.end(); path++ )
                o << *path << " ";
            o << "\n- Server Client_Body_Size = " << server->default_serv.client_body_size << std::endl\
            << "- Server Client_body_append = " << server->default_serv.client_body_append << std::endl\
            << "- Server Upload Path = " << server->default_serv.upload_path << std::endl\
            << "- Server Upload Status = " << server->default_serv.upload_status << std::endl\
            << "- Server autoindex = " << server->default_serv.autoindex << std::endl << std::endl;
            for ( std::vector<t_location>::const_iterator location = server->locations.begin();
                    location != server->locations.end(); location++ )
            {
                o << "- - - LOCATION " << location->path << " - - -" << std::endl\
                << "- Location Path = " << location->path << std::endl\
                << "- Location Root = " << location->root << std::endl\
                << "- Location index = ";
                for ( std::vector<std::string>::const_iterator index = location->index.begin();
                        index != location->index.end(); index++ )
                    o << *index << " ";
                o << std::endl << "- Location HTTP Methods = ";
                for ( std::vector<std::string>::const_iterator method = location->methods.begin();
                        method != location->methods.end(); method++ )
                    o << *method << " ";
                o << std::endl << "- Client Body Size = " << location->client_body_size << std::endl\
                << "- Client Body Append = " << location->client_body_append << std::endl\
                << "- Upload Path = " << location->upload_path << std::endl\
                << "- Upload Status = " << location->upload_status << std::endl\
                << "- Autodindex = " << location->autoindex << std::endl << "- Errors = " << std::endl;
                for (std::map<size_t, std::string>::const_iterator it = location->errors.begin();
                        it != location->errors.end(); it++)
                    o << "Code = " << it->first << " > URI = " << it->second << std::endl;
                o << "- CGI Extension = ";
                for ( std::vector<std::string>::const_iterator cgi = location->cgi_extension.begin();
                        cgi != location->cgi_extension.end(); cgi++ )
                    o << *cgi << " ";
                o << "- CGI Path = ";
                for ( std::vector<std::string>::const_iterator cgi = location->cgi_path.begin();
                        cgi != location->cgi_path.end(); cgi++ )
                    o << *cgi << " ";
                o << std::endl;
            }
            std::cout << " - - - - - - - - - - - - -- - - - - - - - " << std::endl;
    }
    return o;
}

/*
** --------------------------------- METHODS ---------------------------------
*/

/*
**  FileOpenerChecker() function will first redirect the path of the conf file to a FileChecker() function
**  who's purpose is to check if the file is valid and accessible or not. If it is valid, FileOpenerChecker()
**  will itself open the file stream.
**  The file will be opened using <cstdio> library.
**  The next step will be to redirect the opened file to the SyntaxChecker() function that will check the conformity
**  of the config file syntax with our rules.
**  Finally, it will redirect the opened file to the MultiHandler() function that will call all the handlers necessary
**  to populate or tserver and t_location structs with the information we need by parsing the file.
**  If any of the steps fail, an exception will be thrown.
*/
void    Config::FileOpenerChecker( std::string confpath, Config *config )
{
    //Check if the file is valid and accessible.
    //Open the file using only functions from the fstream library.
    std::fstream    file(confpath.c_str(), std::ios::in);
    if (file.fail() == true)
        throw std::runtime_error("File error.");
    config->file = &file;
    //Check the syntax of the file and we make sure to close the file in case of an error.
    SyntaxChecker(config);
    CheckSemiColons(config);
    RemoveSemiColons(config);
    //Call the MultiHandler() function that will call all the handlers necessary to populate the structs.
    MultiHandler(config);
    IsServerEnough(*config);
    return ;
}

void    Config::SyntaxChecker( Config *config )
{
    std::string     line;
    size_t          line_nb = 0;
    size_t          new_line = 0;

    while (std::getline(*(config->file), line))
    {
        line_nb++;
        if (line.empty())
            continue ;
        if (line[0] == '#')
            continue ;
        if (line.find("server") != std::string::npos)
        {
            new_line = ServerHandler(line, line_nb, config);
            line_nb = 0;
            config->file->seekg(0);
            while (line_nb < new_line)
            {
                std::getline(*(config->file), line);
                line_nb++;
            }
            continue ;
        }
    }
    return ;
}



size_t   Config::ServerHandler( std::string first, size_t line_nb, Config *config )
{
    t_server_block      serv;
    t_line              line;
    std::string         word;
    std::string         tmp;
    size_t              new_line;
    int                 braces = 0;
    line.words = LineToWords(first);
    line.line_number = line_nb;
    serv.server_lines.push_back(line);
    line.words.clear();
    if (first.find('{') != std::string::npos)
    {
        braces++;
        if (first.find_first_not_of(" \tserver{") != std::string::npos)
            throw std::runtime_error("Syntax error on line " + SizeToStr(line_nb));
    }
    else
    {
        if (std::getline(*(config->file), tmp))
        {
            line_nb++;
            line.words = LineToWords(tmp);
            line.line_number = line_nb;
            serv.server_lines.push_back(line);
            line.words.clear();
            if (tmp.find('{') != std::string::npos)
            {
                braces++;
                if (tmp.find_first_not_of(" \t{") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line_nb));
            }
            else
                throw std::runtime_error("Syntax error on line " + SizeToStr(line_nb));
        }
    }
    //Now we are gonna parse each line of the server block until we find the matching closing brace.
    //Each line will be split in words that will be stored in a vector.
    //Each word stored in the std::string word then stored in the vector line.words.
    //Each line will be stored in the vector serv.server_lines.
    while (std::getline(*(config->file), tmp))
    {
        line_nb++;
        if (tmp.empty() || (tmp[0] == '#'))
            continue ;
        if (tmp.find_first_not_of(" \t") != std::string::npos)
        {
            if (tmp.find("location") != std::string::npos)
            {
                new_line = LocationHandler(tmp, line_nb, &serv, config);
                line_nb = 0;
                config->file->seekg(0);
                while (line_nb < new_line)
                {
                    std::getline(*(config->file), tmp);
                    line_nb++;
                }
                continue ;
            }
            if (tmp.find("}") != std::string::npos)
            {
                braces++;
                if (braces == 2)
                {
                    if (tmp.find_first_not_of(" \t}") != std::string::npos)
                        throw std::runtime_error("Syntax error 1 on line " + SizeToStr(line_nb));
                    line.words = LineToWords(tmp);
                    line.line_number = line_nb;
                    serv.server_lines.push_back(line);
                    line.words.clear();
                    config->server_blocks.push_back(serv);
                    return (line_nb);
                }
                else
                    throw std::runtime_error("bSyntax error 2 on line " + SizeToStr(line_nb));
            }
            if (tmp.find("{") != std::string::npos)
                    throw std::runtime_error("Syntax error 3 on line " + SizeToStr(line_nb));
            if (!tmp.empty() || (tmp[0] != '#'))
            {
                line.words = LineToWords(tmp);
                line.line_number = line_nb;
                serv.server_lines.push_back(line);
                line.words.clear();
            }
        }
    }
    throw std::runtime_error("Syntax error 4 on line " + SizeToStr(line_nb) + ". Missing closing brace.");
}

size_t   Config::LocationHandler( std::string first, size_t line_nb, t_server_block *serv, Config *config )
{
    t_location_block    loc;
    t_line              line;
    std::string         word;
    std::string         tmp;
    int                 braces = 0;
    if (first.find('{') != std::string::npos)
    {
        braces++;
        line.words = LineToWords(first);
        if (line.words.size() != 3)
            throw std::runtime_error("Syntax error 1 in location block on line " + SizeToStr(line_nb));
        line.line_number = line_nb;
        loc.location_lines.push_back(line);
        line.words.clear();
    }
    else
    {
        if (std::getline(*(config->file), tmp))
        {
            line_nb++;
            if (tmp.find('{') != std::string::npos && tmp.find_first_not_of(" \t{") == std::string::npos)
            {
                braces++;
                line.words = LineToWords(tmp);
                if (line.words.size() != 2)
                    throw std::runtime_error("Syntax error 2 in location block on line " + SizeToStr(line_nb - 1));
                line.line_number = line_nb;
                loc.location_lines.push_back(line);
                line.words.clear();
            }
            else
                throw std::runtime_error("Syntax error 3 in location block on line " + SizeToStr(line_nb));
        }
    }
    //Now we are gonna parse each line of the server block until we find the matching closing brace.
    //Each line will be split in words that will be stored in a vector.
    //Each word stored in the std::string word then stored in the vector line.words.
    //Each line will be stored in the vector serv.server_lines.
    while (std::getline(*(config->file), tmp))
    {
        line_nb++;
        if (tmp.empty() || (tmp[0] == '#'))
            continue ;
        if (tmp.find_first_not_of(" \t") != std::string::npos)
        {
            if (tmp.find("}") != std::string::npos)
            {
                braces++;
                if (braces == 2)
                {
                    line.words = LineToWords(tmp);
                    line.line_number = line_nb;
                    loc.location_lines.push_back(line);
                    line.words.clear();
                    serv->location_blocks.push_back(loc);
                    return (line_nb);
                }
                else
                    throw std::runtime_error("Syntax error 4 on line in location block " + SizeToStr(line_nb));
            }
            if (tmp.find("{") != std::string::npos)
                    throw std::runtime_error("Syntax error 5 on line in location block " + SizeToStr(line_nb));
            line.words = LineToWords(tmp);
            line.line_number = line_nb;
            loc.location_lines.push_back(line);
            line.words.clear();
        }
    }
    throw std::runtime_error("Syntax error 6 on line " + SizeToStr(line_nb) + ". Missing closing brace.");
}

void    Config::CheckSemiColons( Config *config )
{
    for ( std::vector<t_server_block>::const_iterator server = config->server_blocks.begin();
            server != config->server_blocks.end(); server++ )
    {
        for ( std::vector<t_line>::const_iterator line = server->server_lines.begin();
                line != server->server_lines.end(); line++ )
        {
            if (line->words[line->words.size() - 1].find(';') == std::string::npos \
                    && line->words[line->words.size() - 1].find_first_of("{}") == std::string::npos)
                throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) +  " : missing ';' at the end of the line.");
            if (line->words[line->words.size() - 1].find_first_of("{}") != std::string::npos \
                    && line->words[line->words.size() - 1].find(";") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : extra ';' at the end of the line.");
        }
        for ( std::vector<t_location_block>::const_iterator location = server->location_blocks.begin();
                location != server->location_blocks.end(); location++ )
        {
            for ( std::vector<t_line>::const_iterator line = location->location_lines.begin();
                    line != location->location_lines.end(); line++ )
            {
                if (line->words[line->words.size() - 1].find(';') == std::string::npos \
                        && line->words[line->words.size() - 1].find_first_of("{}") == std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number)+ " : missing ';' at the end of the line.");
                if (line->words[line->words.size() - 1].find_first_of("{}") != std::string::npos \
                        && line->words[line->words.size() - 1].find(";") != std::string::npos)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : extra ';' at the end of the line.");
            }
        }
    }
}

void    Config::RemoveSemiColons( Config *config )
{
    std::vector<t_server_block> serv;
    t_line new_line;
    for ( std::vector<t_server_block>::const_iterator it = config->server_blocks.begin();
            it != config->server_blocks.end(); it++ )
    {
        t_server_block server;
        for ( std::vector<t_line>::const_iterator line = it->server_lines.begin();
                line != it->server_lines.end(); line++ )
        {
            new_line.line_number = line->line_number;
            for ( std::vector<std::string>::const_iterator word = line->words.begin();
                    word != line->words.end(); word++ )
            {
                if (word->find(';') != std::string::npos)
                    new_line.words.push_back(word->substr(0, word->find(';')));
                else
                    new_line.words.push_back(*word);
            }
            server.server_lines.push_back(new_line);
            new_line.words.clear();
        }
        for ( std::vector<t_location_block>::const_iterator loc = it->location_blocks.begin();
                loc != it->location_blocks.end(); loc++ )
        {
            t_location_block location;
            for ( std::vector<t_line>::const_iterator line = loc->location_lines.begin();
                    line != loc->location_lines.end(); line++ )
            {
                new_line.line_number = line->line_number;
                for ( std::vector<std::string>::const_iterator word = line->words.begin();
                        word != line->words.end(); word++ )
                {
                    if (word->find(';') != std::string::npos)
                        new_line.words.push_back(word->substr(0, word->find(';')));
                    else
                        new_line.words.push_back(*word);
                }
                location.location_lines.push_back(new_line);
                new_line.words.clear();
            }
            server.location_blocks.push_back(location);
        }
        serv.push_back(server);
    }
    config->server_blocks.erase(config->server_blocks.begin(), config->server_blocks.end());
    config->server_blocks = serv;
}

void    Config::MultiHandler( Config *config )
{
    //Now we are gonna parse every line of our server_blocks and location_blocks.
    //We are gonna check the format of each line at the same time and fill the t_server structure and its vector of t_location with the information we'll gather.
    //If we encounter an unvalid directive, format, etc... We'll throw an exception.

    for ( std::vector<t_server_block>::const_iterator server = config->server_blocks.begin();
            server != config->server_blocks.end(); server++ )
    {
        t_server   serv;
        t_location default_serv;
        default_serv.autoindex = false;
        default_serv.upload_status = false;
        default_serv.client_body_size = 1048576;
        for ( std::vector<t_line>::const_iterator line = server->server_lines.begin();
                line != server->server_lines.end(); line++ )
        {
            if (line->words[0] == "listen")
            {
                t_listen listen;
                if (line->words.size() == 2)
                {
                    listen.port = line->words[1];
                    listen.host = "*";
                    serv.listen = listen;
                }
                else if (line->words.size() == 3)
                {
                    listen.port = line->words[1];
                    listen.host = line->words[2];
                    serv.listen = listen;
                }
                else
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for listen directive.");
            }
            else if (line->words[0] == "server_name")
            {
                for (size_t i = 1; i < line->words.size(); i++)
                    serv.server_names.push_back(line->words[i]);
            }
            else if (line->words[0] == "error")
            {
                if (line->words.size() != 3)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for error directive.");
                default_serv.errors.insert(std::pair<size_t, std::string>(StrToSize(line->words[1]), line->words[2]));
            }
            else if (line->words[0] == "root")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for root directive.");
                default_serv.root = line->words[1];
            }
            else if (line->words[0] == "index")
            {
                for (size_t i = 1; i < line->words.size(); i++)
                    default_serv.index.push_back(line->words[i]);
            }
            else if (line->words[0] == "method")
            {
                if (line->words.size() < 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for method directive.");
                for (size_t i = 1; i < line->words.size(); i++)
                    default_serv.methods.push_back(line->words[i]);
            }
            else if (line->words[0] == "client_max_body_size")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for client_body_size directive.");
                default_serv.client_body_size = StrToSize(line->words[1]);
            }
            else if (line->words[0] == "client_body_append")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for client_body_append directive.");
                if (line->words[1] != "true" && line->words[1] != "false"\
                    && line->words[1] != "true;" && line->words[1] != "false;")
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for upload_status directive.");
                if (line->words[1] == "true" || line->words[1] == "true;")
                    default_serv.client_body_append = true;
                else
                    default_serv.client_body_append = false;   
            }
            else if (line->words[0] == "autoindex")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for autoindex directive.");
                if (line->words[1] != "on" && line->words[1] != "off"\
                        && line->words[1] != "on;" && line->words[1] != "off;")
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for autoindex directive.");
                if (line->words[1] == "on" || line->words[1] == "on;")
                    default_serv.autoindex = true;
                else
                    default_serv.autoindex = false;
            }
            else if (line->words[0] == "upload_status")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for upload_status directive.");
                if (line->words[1] != "on" && line->words[1] != "off"\
                        && line->words[1] != "on;" && line->words[1] != "off;")
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for upload_status directive.");
                if (line->words[1] == "on" || line->words[1] == "on;")
                    default_serv.upload_status = true;
                else
                    default_serv.upload_status = false;
            }
            else if (line->words[0] == "upload_path")
            {
                if (line->words.size() != 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for upload_path directive.");
                default_serv.upload_path = line->words[1];
            }
            else if (line->words[0] == "cgi_path")
            {
                if (line->words.size() < 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgi_path directive.");
                for (size_t i = 1; i < line->words.size(); i++)
                    default_serv.cgi_path.push_back(line->words[i]);
            }
            else if (line->words[0] == "cgi_extension")
            {
                if (line->words.size() < 2)
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgipass directive.");
                for (size_t i = 1; i < line->words.size(); i++)
                    default_serv.cgi_extension.push_back(line->words[i]);
            }
            else if (line->words[0] == "server" || line->words[0] == "}")
                continue ;
            else
                throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid directive.");
            if (default_serv.autoindex != false)
                default_serv.autoindex = true;
            serv.default_serv = default_serv;
        }
        for ( std::vector<t_location_block>::const_iterator location = server->location_blocks.begin();
                location != server->location_blocks.end(); location++ )
        {
            t_location loc;
            loc.upload_status = false;
            loc.autoindex = false;
            loc.client_body_size = -1;
            loc.client_body_append = -1;
            for ( std::vector<t_line>::const_iterator line = location->location_lines.begin();
                    line != location->location_lines.end(); line++ )
            {
                if (line->words[0] == "location")
                {
                    if (line->words.size() != 3)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for location directive.");
                    if ( line->words[1][0] != '/' && line->words[1] != "=" && line->words[1] != "~" && line->words[1] != "~*")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid operator for location directive.");
                    loc.path = line->words[1];
                }
                else if (line->words[0] == "root")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for root directive.");
                    loc.root = line->words[1];
                }
                else if (line->words[0] == "index")
                {
                    for (size_t i = 1; i < line->words.size(); i++)
                        loc.index.push_back(line->words[i]);
                }
                else if (line->words[0] == "method")
                {
                    if (line->words.size() < 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for method directive.");
                    for (size_t i = 1; i < line->words.size(); i++)
                        loc.methods.push_back(line->words[i]);
                }
                else if (line->words[0] == "client_body_size")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for client_body_size directive.");
                    loc.client_body_size = StrToSize(line->words[1]);
                }
                else if (line->words[0] == "client_body_append")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for client_body_append directive.");
                    if (line->words[1] != "true" && line->words[1] != "false"\
                        && line->words[1] != "true;" && line->words[1] != "false;")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for upload_status directive.");
                    if (line->words[1] == "true" || line->words[1] == "true;")
                        loc.client_body_append = true;
                    else
                        loc.client_body_append = false;   
                }
                else if (line->words[0] == "upload_status")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for upload_status directive.");
                    if (line->words[1] != "on" && line->words[1] != "off"\
                            && line->words[1] != "on;" && line->words[1] != "off;")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for upload_status directive.");
                    if (line->words[1] == "on" || line->words[1] == "on;")
                        loc.upload_status = true;
                    else
                        loc.upload_status = false;
                }
                else if (line->words[0] == "upload_path")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for upload_path directive.");
                    loc.upload_path = line->words[1];
                }
                else if (line->words[0] == "autoindex")
                {
                    if (line->words.size() != 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for autoindex directive.");
                    if (line->words[1] != "on" && line->words[1] != "off"\
                            && line->words[1] != "on;" && line->words[1] != "off;")
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid value for autoindex directive.");
                    if (line->words[1] == "on" || line->words[1] == "on;")
                        loc.autoindex = true;
                    else
                        loc.autoindex = false;
                }
                else if (line->words[0] == "cgi_path")
                {
                    if (line->words.size() < 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgiparam directive.");
                    for (size_t i = 1; i < line->words.size(); i++)
                        loc.cgi_path.push_back(line->words[i]);
                }
                else if (line->words[0] == "cgi_extension")
                {
                    if (line->words.size() < 2)
                        throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid number of arguments for cgipass directive.");
                    for (size_t i = 1; i < line->words.size(); i++)
                        loc.cgi_extension.push_back(line->words[i]);
                }
                else if (line->words[0] == "}")
                    continue ;
                else
                    throw std::runtime_error("Syntax error on line " + SizeToStr(line->line_number) + " : invalid directive.");
            }
            serv.locations.push_back(loc);
        }
        config->server.push_back(serv);
    }
    
    return ;
}

void    Config::IsServerEnough( const Config &config )
{
    //Here we are gonna check that each t_server has at least a listen directive and a server_name directive.
    //Those two directives are mandatory in any server created in Nginx.
    for ( std::vector<t_server>::const_iterator server = config.server.begin();
            server != config.server.end(); server++ )
    {
        if ( server->listen.port.empty() || server->listen.host.empty() || server->server_names.empty() )
            throw std::runtime_error("Syntax error : each server block must have at least a listen directive and a server_name directive.");
    }
}
