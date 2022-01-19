#include <iostream>
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

enum g_status{
    SERVER,
    LOCATION
};

typedef std::vector<ServerConfig>::const_iterator const_server_iter;
typedef std::map<std::string, LocationConfig>::const_iterator const_location_iter;

int g_status;

void put_listen(const int listen, const std::string &space)
{
    std::cout << space << "listen: " << listen << std::endl;
}

void put_server_name(const std::string &server_name, const std::string &space)
{
    std::cout << space << "server_name: " << server_name << std::endl;
}

void put_allow_methods(const std::vector<std::string> &allow_method, const std::string &space)
{
    std::string second_space;
    if (g_status == LOCATION)
        second_space = "                                    ";
    else
        second_space = "                      ";

    std::cout << space << "allow_method: ";

    for (std::vector<std::string>::const_iterator iter = allow_method.begin();
        iter != allow_method.end();
        iter++)
    {
        if (iter != allow_method.begin())
            std::cout << second_space;
        std::cout << *iter << std::endl;
    }
}

void put_cgi_extension(const std::vector<std::string> &cgi_extension, const std::string &space)
{
    std::string second_space;
    if (g_status == LOCATION)
        second_space = "                                    ";
    else
        second_space = "                      ";

    std::cout << space << "cgi_extension: ";

    for (std::vector<std::string>::const_iterator iter = cgi_extension.begin();
        iter != cgi_extension.end();
        iter++)
    {
        if (iter != cgi_extension.begin())
            std::cout << second_space;
        std::cout << *iter << std::endl;
    }
}

void put_client_max_body_size(const int client_max_body_size)
{
    std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
}

void put_error_page(const std::map<int, std::string> &error_page, const std::string &space)
{
    std::string second_space;
    if (g_status == LOCATION)
        second_space = "                                  ";
    else
        second_space = "                      ";

    std::cout << space <<"error_page: ";

    if (error_page.size() == 0)
    {
        std::cout << std::endl;
        return;
    }

    for (std::map<int, std::string>::const_iterator iter = error_page.begin();
         iter != error_page.end();
         ++iter)
    {
        if (iter != error_page.begin())
            std::cout << second_space;
        std::cout << iter->first << " "
                  << iter->second << std::endl;
    }
}

void put_upload_path(const std::string &upload_path, const std::string &space)
{
    std::cout << space << "upload_path: " << upload_path << std::endl;
}

void put_alias(const std::string &alias, const std::string &space)
{
    std::cout << space << "alias: " << alias << std::endl;
}

void put_autoindex(const std::string &autoindex, const std::string &space)
{
    std::cout << space << "autoindex: " << autoindex << std::endl;
}

void put_index(const std::vector<std::string> &index, const std::string &space)
{
    std::string second_space;
    if (g_status == LOCATION)
        second_space = "                             ";
    else
        second_space = "                      ";

    std::cout << space << "index: ";

    for (std::vector<std::string>::const_iterator const_iter = index.begin();
         const_iter!= index.end();
         ++const_iter)
    {
        if (const_iter != index.begin())
            std::cout << second_space;
        std::cout << *const_iter << std::endl;
    }
}

void put_return(const std::map<int, std::string> &return_redirect, const std::string &space)
{
    std::string second_space;
    if (g_status == LOCATION)
        second_space = "                              ";
    else
        second_space = "                      ";

    std::cout << space << "return: ";
    if (return_redirect.size() == 0)
    {
        std::cout << std::endl;
        return;
    }
    for (std::map<int, std::string>::const_iterator iter = return_redirect.begin();
         iter != return_redirect.end();
         ++iter)
    {
        if (iter != return_redirect.begin())
            std::cout << second_space;
        std::cout << iter->first << " "
                  << iter->second << std::endl;
    }

}

void put_location_config(const std::map<std::string, LocationConfig> &location, const std::string &space)
{
    g_status = LOCATION;

    for (const_location_iter const_iter = location.begin();
        const_iter != location.end();
        ++const_iter)
    {
        std::cout << space << "location: " << const_iter->first<< std::endl;
        put_alias(const_iter->second.alias(), "                      ");
        put_allow_methods(const_iter->second.allowMethod(), "                      ");
        put_autoindex(const_iter->second.autoindex(),  "                      ");
        put_cgi_extension(const_iter->second.cgiExtension(), "                      ");
        put_error_page(const_iter->second.errorPage(), "                      ");
        put_index(const_iter->second.index(), "                      ");
        put_return(const_iter->second.returnRedirect(), "                      ");
        put_upload_path(const_iter->second.uploadPath(), "                      ");
    }

    g_status = SERVER;
}

void put_server_config(Config &config)
{
    std::vector<ServerConfig> server = config.server();
    int count = 1;
    g_status = SERVER;

    for (const_server_iter const_iter = server.begin();
        const_iter != server.end();
        ++const_iter)
    {
        std::cout << "\n-----[server_" << count << "]------------------------------"  << std::endl;
        put_allow_methods(const_iter->allowMethod(), "        ");
        put_autoindex(const_iter->autoindex(), "           ");
        put_cgi_extension(const_iter->cgiExtension(), "      ");
        put_client_max_body_size(const_iter->clientMaxBodySize());
        put_error_page(const_iter->errorPage(), "          ");
        put_index(const_iter->index(), "               ");
        put_listen(const_iter->listen(), "              ");
        put_return(const_iter->returnRedirect(),  "              ");
        put_server_name(const_iter->serverName(), "         ");
        put_upload_path(const_iter->uploadPath(), "         ");
        put_location_config(const_iter->location(), "            ");
        count++;
    }
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Failed arguments. Please enter the following... " << std::endl
                  << "$ ./file [CONFIG_FILE_PATH]" << std::endl;
        return (0);
    }

    Config config = Config();
    try
    {
        config.readFile(argv[1]);
        put_server_config(config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
