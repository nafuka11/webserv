#include "Config.hpp"
#include <iostream>
#include "ConfigParser.hpp"
#include "LocationConfig.hpp"

Config::Config()
{
}

Config::~Config()
{
}

void Config::readFile(const std::string &filepath)
{
    ConfigParser parser(*this);
    parser.readFile(filepath);
#ifdef DEBUG
    printServerConfig();
#endif
}

const std::map<int, std::vector<ServerConfig> > &Config::server() const
{
    return server_;
}

void Config::addServer(const ServerConfig &server_config)
{
    int listen = server_config.listen();
    std::map<int, std::vector<ServerConfig> >::iterator
        port_found = server_.find(listen);

    if (port_found != server_.end())
    {
        if (server_config.serverName().empty())
        {
            throw ConfigError(INVALID_VALUE, "server_name");
        }
        std::vector<ServerConfig> &servers = port_found->second;
        for (size_t i = 0; i < servers.size(); i++)
        {
            if (servers.at(i).serverName() == server_config.serverName())
            {
                throw ConfigError(DUPLICATE_VALUE, "server_name");
            }
        }
        servers.push_back(server_config);
    }
    else
    {
        std::vector<ServerConfig> new_servers(1, server_config);
        server_.insert(std::make_pair(listen, new_servers));
    }
}

#if DEBUG
void Config::printServerConfig() const
{
    int count = 1;
    std::map<int, std::vector<ServerConfig> >::const_iterator iter = server_.begin();

    for (; iter != server_.end(); ++iter)
    {
        std::cout << "port: " << iter->first << std::endl;
        for (std::vector<ServerConfig>::const_iterator server_config = iter->second.begin();
             server_config != iter->second.end();
             ++server_config)
        {
            std::cout << "\n------------------------------------------[ server No."
                      << count << " ]"  << std::endl;
            printServerName(server_config->serverName());
            printListen(server_config->listen());
            printAllowMethod(server_config->allowMethod(), "");
            printAutoindex(server_config->autoindex(), "");
            printCgiExtension(server_config->cgiExtension(), "");
            printClientMaxBodySize(server_config->clientMaxBodySize(), "");
            printErrorPage(server_config->errorPage(), "");
            printIndex(server_config->index(), "");
            printReturnRedirect(server_config->returnRedirect(), "");
            printUploadPath(server_config->uploadPath(), "");
            printLocation(server_config->location());
            count++;
        }
    }
}

void Config::printLocation(const std::map<std::string, LocationConfig> &location) const
{
    std::map<std::string, LocationConfig>::const_iterator iter = location.begin();
    for (; iter != location.end(); ++iter)
    {
        std::cout << "\nlocation " << iter->first << std::endl;
        printAlias(iter->second.alias(), "    ");
        printAllowMethod(iter->second.allowMethod(), "    ");
        printAutoindex(iter->second.autoindex(), "    ");
        printCgiExtension(iter->second.cgiExtension(), "    ");
        printErrorPage(iter->second.errorPage(), "    ");
        printIndex(iter->second.index(), "    ");
        printReturnRedirect(iter->second.returnRedirect(), "    ");
        printUploadPath(iter->second.uploadPath(), "    ");
    }
}

void Config::printServerName(const std::string &server_name) const
{
    std::cout << "server_name = [" << server_name << "]" << std::endl;
}

void Config::printListen(const int listen) const
{
    std::cout << "listen = [" << listen << "]"  << std::endl;
}

void Config::printAlias(const std::string &alias, const std::string &indent) const
{
    std::cout << indent << "alias = ";

    if (alias.size() == 0)
    {
        std::cout << std::endl;
        return;
    }
    std::cout << "[" << alias << "]" << std::endl;
}

void Config::printAllowMethod(const std::vector<std::string> &allow_method,
                              const std::string &indent) const
{
    std::cout << indent << "allow_method = ";

    std::vector<std::string>::const_iterator iter = allow_method.begin();
    for (; iter != allow_method.end(); ++iter)
    {
        std::cout << "[" << *iter << "]";
    }
    std::cout << std::endl;
}

void Config::printAutoindex(const std::string &autoindex, const std::string &indent) const
{
    std::cout << indent << "autoindex = [" << autoindex << "]" << std::endl;
}

void Config::printCgiExtension(const std::vector<std::string> &cgi_extension,
                               const std::string &indent) const
{
    std::cout << indent << "cgi_extension = ";

    std::vector<std::string>::const_iterator iter = cgi_extension.begin();
    for (; iter != cgi_extension.end(); ++iter)
    {
        std::cout << "[" << *iter << "]";
    }
    std::cout << std::endl;
}

void Config::printClientMaxBodySize(const int client_max_body_size,
                                    const std::string &indent) const
{
    std::cout << indent
              << "client_max_body_size = [" << client_max_body_size  << "]" << std::endl;
}

void Config::printErrorPage(const std::map<int, std::string> &error_page,
                            const std::string &indent) const
{
    std::cout << indent << "error_page = ";

    std::map<int, std::string>::const_iterator iter = error_page.begin();
    for (; iter != error_page.end(); ++iter)
    {
        std::cout << "[" << iter->first << " " << iter->second << "]";
    }
    std::cout << std::endl;
}

void Config::printIndex(const std::vector<std::string> &index,
                        const std::string &indent) const
{
    std::cout << indent << "index = ";

    std::vector<std::string>::const_iterator iter = index.begin();
    for (; iter != index.end(); ++iter)
    {
        std::cout << "[" << *iter << "]";
    }
    std::cout << std::endl;
}

void Config::printReturnRedirect(const std::map<int, std::string> &return_redirect,
                                 const std::string &indent) const
{
    std::cout << indent << "return =";

    std::map<int, std::string>::const_iterator iter = return_redirect.begin();
    if (return_redirect.empty())
    {
        std::cout << std::endl;
        return;
    }

    std::cout << "[" << iter->first << " " << iter->second << "]" << std::endl;
}

void Config::printUploadPath(const std::string &upload_path,
                             const std::string &indent) const
{
    std::cout << indent << "upload_path = ";

    if (upload_path.size() == 0)
    {
        std::cout << std::endl;
        return;
    }
    std::cout << upload_path << "]" << std::endl;
}

#endif
