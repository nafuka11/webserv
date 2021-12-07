#include <iostream>
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

typedef std::vector<ServerConfig>::const_iterator const_server_iter;
typedef std::map<std::string, LocationConfig>::const_iterator const_location_iter;

void put_listen(const_server_iter const_iter)
{
    const int listen = const_iter->listen();
    std::cout << "              listen: " << listen << std::endl;
}

void put_server_name(const_server_iter const_iter)
{
    const std::string server_name = const_iter->serverName();
    std::cout << "         server_name: " << server_name << std::endl;
}

void put_allow_methods(const_server_iter const_iter)
{
    const std::vector<std::string> allow_methods = const_iter->allowMethods();

    std::cout << "       allow_methods: ";
    for (std::vector<std::string>::const_iterator iter = allow_methods.begin();
         iter != allow_methods.end();
         ++iter)
    {
        if (iter != allow_methods.begin())
            std::cout << "                      ";
        std::cout << *iter << std::endl;
    }
}

void put_cgi_extension(const_server_iter const_iter)
{
    const std::string cgi_extension = const_iter->cgiExtension();
    std::cout << "       cgi_extension: " << cgi_extension <<  std::endl;
}

void put_client_max_body_size(const_server_iter const_iter)
{
    const int client_max_body_size = const_iter->clientMaxBodySize();
    std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
}

void put_error_page(const_server_iter const_iter)
{
    const std::map<int, std::string> error_page = const_iter->errorPage();

    std::cout << "          error_page: ";
    for (std::map<int, std::string>::const_iterator iter = error_page.begin();
         iter != error_page.end();
         ++iter)
    {
        if (iter != error_page.begin())
            std::cout << "                      ";
        std::cout << iter->first << " "
                  << iter->second << std::endl;
    }
}

void put_upload_path(const_server_iter const_iter)
{
    const std::string upload_path = const_iter->uploadPath();
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
    const std::vector<std::string> index = const_iter->second.index();

    std::cout << "                          index: ";
    for (std::vector<std::string>::const_iterator const_index_iter = index.begin();
         const_index_iter != index.end();
         ++const_index_iter)
    {
        if (const_index_iter != index.begin())
            std::cout << "                                 ";
        std::cout << *const_index_iter << std::endl;
    }
}

void put_return(const_location_iter const_iter)
{
    const std::map<int, std::string> return_redirect = const_iter->second.returnRedirect();

    std::cout << "                         return: ";
    for (std::map<int, std::string>::const_iterator const_return_iter = return_redirect.begin();
         const_return_iter != return_redirect.end();
         ++const_return_iter)
    {
        if (const_return_iter != return_redirect.begin())
            std::cout << "                                 ";
        std::cout << const_return_iter->first << " "
                  << const_return_iter->second << std::endl;
    }

}

void put_server_config(Config &config)
{
    const std::vector<ServerConfig> server = config.server();

    for (const_server_iter const_iter = server.begin();
        const_iter != server.end();
        ++const_iter)
    {
        put_listen(const_iter);
        put_server_name(const_iter);
        put_allow_methods(const_iter);
        put_cgi_extension(const_iter);
        put_client_max_body_size(const_iter);
        put_error_page(const_iter);
        put_upload_path(const_iter);
    }
}

void put_location_config(Config &config)
{
    const std::vector<ServerConfig> server = config.server();
    const std::map<std::string, LocationConfig> location = server[0].location();

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
    std::cout << "\n" << std::endl;
}

int main(void)
{
    Config config = Config();

    config.readFile();
    put_server_config(config);
    put_location_config(config);
    return (0);
}
