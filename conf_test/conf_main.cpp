#include <iostream>
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

int main(void)
{
    Config config = Config();

    config.readFile();

    std::vector<ServerConfig> server = config.server();
    int listen = server[0].listen();
    std::string server_name = server[0].serverName();
    std::vector<std::string> allow_methods = server[0].allowMethods();
    std::string cgi_extension = server[0].cgiExtension();
    int client_max_body_size = server[0].clientMaxBodySize();
    std::map<int, std::string> error_page = server[0].errorPage();
    std::string upload_path = server[0].uploadPath();
    std::map<std::string, LocationConfig> location = server[0].location();

    std::cout << "              listen: " << listen << std::endl;

    std::cout << "         server_name: " << server_name << std::endl;

    std::cout << "       allow_methods: ";
    for (std::vector<std::string>::const_iterator allow_methods_iter = allow_methods.begin();
         allow_methods_iter != allow_methods.end();
         ++allow_methods_iter)
    {
        if (allow_methods_iter != allow_methods.begin())
            std::cout << "                      ";
        std::cout << *allow_methods_iter << std::endl;
    }

    std::cout << "       cgi_extension: " << cgi_extension <<  std::endl;

    std::cout << "client_max_body_size: " << client_max_body_size << std::endl;

    for (std::map<int, std::string>::const_iterator error_page_iter  = error_page.begin();
         error_page_iter != error_page.end();
         ++error_page_iter)
    {
        std::cout << "          error_page: " << error_page_iter->first << " " << error_page_iter->second << std::endl;
    }

    std::cout << "         upload_path: " << upload_path << std::endl;

    for (std::map<std::string, LocationConfig>::const_iterator location_iter = location.begin();
        location_iter != location.end();
        ++location_iter)
    {
        std::vector<std::string> index = location_iter->second.index();
        std::map<int, std::string> return_redirect = location_iter->second.returnRedirect();
        std::map<int, std::string>::const_iterator return_iter = return_redirect.begin();

        std::cout << "            location: " << location_iter->first<< std::endl
                  << "                          alias: " << location_iter->second.alias() << std::endl
                  << "                      autoindex: " << location_iter->second.autoindex() << std::endl
                  << "                          index: " << index[0] << std::endl
                  << "                         return: " << return_iter->first << " " << return_iter->second << std::endl;
    }
    return (0);
}