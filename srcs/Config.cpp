#include "Config.hpp"
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
