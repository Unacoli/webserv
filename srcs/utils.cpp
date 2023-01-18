#include "utils.hpp"

int     checkPath(std::string path)
{
    struct stat buf;
    if (stat(path.c_str(), &buf) == 0)
    {
        //path exists is a file and readble
        if (S_ISREG(buf.st_mode) && (buf.st_mode & S_IRUSR))
            return (1);
        //path exists is a directory and readble
        else if (S_ISDIR(buf.st_mode) && (buf.st_mode & S_IRUSR))
            return (2);
        //path exists but is not a file or a directory
        else
            return (3);
    }
    return (0);
}

std::string readFile(std::string path)
{
    std::string ret;
    std::ifstream file(path.c_str());
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
            ret += line;
        file.close();
    }
    return ret;
}

bool    isReadable(std::string path)
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return (buf.st_mode & S_IRUSR);
}

bool    isDirectory(std::string path)
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return S_ISDIR(buf.st_mode);
}

bool    fileExists(std::string path)
{
    struct stat buf;
    return (stat(path.c_str(), &buf) == 0);
}

std::string trim(const std::string &str) 
{
    std::string whitespaces(" \t\r\n");
    int start = str.find_first_not_of(whitespaces);
    int end = str.find_last_not_of(whitespaces);
    if(start == -1 || end == -1)
        return "";
    else
        return str.substr(start, end-start+1);
}


void    split(const std::string& s, char delim, std::vector<std::string>& parts) 
{
    size_t start = 0, end = 0;
    while ((end = s.find(delim, start)) != std::string::npos) 
    {
        parts.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    parts.push_back(s.substr(start));
}

std::string const                  IntToStr( int nbr )
{
    std::stringstream ss;
    ss << nbr;
    return ss.str();
}

std::string const                   SizeToStr( size_t nbr )
{
    std::ostringstream oss;
    oss << nbr;
    return oss.str();
}

size_t                              StrToSize( std::string const &str )
{
    size_t nbr = static_cast<size_t>(std::atol(str.c_str()));
    return (nbr);
}

std::vector<std::string>            LineToWords ( std::string str )
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find_first_of(" \t");
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty())
            tokens.push_back(token);
        start = str.find_first_not_of(" \t", end);
        end = str.find_first_of(" \t", start);
    }
    std::string token = str.substr(start, end);
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}