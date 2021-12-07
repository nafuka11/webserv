#include <iostream>
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

typedef std::vector<ServerConfig>::const_iterator const_server_iter;
typedef std::map<std::string, LocationConfig>::const_iterator const_location_iter;

void put_listen(const_server_iter const_iter)
{
    int listen = const_iter->listen();
    std::cout << "              listen: " << listen << std::endl;
}

void put_server_name(const_server_iter const_iter)
{
    std::string server_name = const_iter->serverName();
    std::cout << "         server_name: " << server_name << std::endl;
}

void put_allow_methods(const_server_iter const_iter)
{
    std::vector<std::string> allow_methods = const_iter->allowMethods();
    std::vector<std::string>::const_iterator iter = allow_methods.begin();

    std::cout << "       allow_methods: ";
    if (iter == allow_methods.end())
    {
        std::cout << std::endl;
        return ;
    }
    while (iter != allow_methods.end())
    {
        if (iter != allow_methods.begin())
            std::cout << "                      ";
        std::cout << *iter << std::endl;
        ++iter;
    }
}

void put_cgi_extension(const_server_iter const_iter)
{
    std::string cgi_extension = const_iter->cgiExtension();
    std::cout << "       cgi_extension: " << cgi_extension <<  std::endl;
}

void put_client_max_body_size(const_server_iter const_iter)
{
    int client_max_body_size = const_iter->clientMaxBodySize();
    std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
}

void put_error_page(const_server_iter const_iter)
{
    std::map<int, std::string> error_page = const_iter->errorPage();
    std::map<int, std::string>::const_iterator iter = error_page.begin();

    std::cout << "          error_page: ";
    if (iter == error_page.end())
    {
        std::cout << std::endl;
        return ;
    }
    while (iter != error_page.end())
    {
        if (iter != error_page.begin())
            std::cout << "                      ";
        std::cout << iter->first << " "
                  << iter->second << std::endl;
        ++iter;
    }
}

void put_upload_path(const_server_iter const_iter)
{
    std::string upload_path = const_iter->uploadPath();
    std::cout << "         upload_path: " << upload_path << std::endl;
}

void put_alias(const_location_iter const_iter)
{
    std::cout << "                          alias: " << const_iter->second.alias() << std::endl;
}

void put_autoindex(const_location_iter const_iter)
{
    std::cout << "                      autoindex: " << const_iter->second.autoindex() << std::endl;
}

void put_index(const_location_iter const_iter)
{
    std::vector<std::string> index = const_iter->second.index();
    std::vector<std::string>::const_iterator const_index_iter = index.begin();

    std::cout << "                          index: ";
    if (const_index_iter == index.end())
    {
        std::cout << std::endl;
        return ;
    }
    while (const_index_iter != index.end())
    {
        if (const_index_iter != index.begin())
            std::cout << "                                 ";
        std::cout << *const_index_iter << std::endl;
        ++const_index_iter;
    }
}

void put_return(const_location_iter const_iter)
{
    std::map<int, std::string> return_redirect = const_iter->second.returnRedirect();
    std::map<int, std::string>::const_iterator const_return_iter = return_redirect.begin();

    std::cout << "                         return: ";
    if (const_return_iter == return_redirect.end())
    {
        std::cout << std::endl;
        return ;
    }
    while (const_return_iter != return_redirect.end())
    {
        if (const_return_iter != return_redirect.begin())
            std::cout << "                                 ";
        std::cout << const_return_iter->first << " "
                  << const_return_iter->second << std::endl;
        ++const_return_iter;
    }

}

void put_location_config(const_server_iter const_iter)
{
    std::map<std::string, LocationConfig> location = const_iter->location();

    for (const_location_iter const_iter = location.begin();
        const_iter != location.end();
        ++const_iter)
    {
        std::cout << "            location: " << const_iter->first<< std::endl;
        put_alias(const_iter);
        put_autoindex(const_iter);
        put_index(const_iter);
        put_return(const_iter);
    }
}

void put_server_config(Config &config)
{
    std::vector<ServerConfig> server = config.server();
    int count = 1;

    for (const_server_iter const_iter = server.begin();
        const_iter != server.end();
        ++const_iter)
    {
        std::cout << "-----[server_" << count << "]------------------------------"  << std::endl;
        put_listen(const_iter);
        put_server_name(const_iter);
        put_allow_methods(const_iter);
        put_cgi_extension(const_iter);
        put_client_max_body_size(const_iter);
        put_error_page(const_iter);
        put_upload_path(const_iter);
        put_location_config(const_iter);
        count++;
    }
}

int main(void)
{
    Config config = Config();

    config.readFile();
    put_server_config(config);
    return (0);
}
