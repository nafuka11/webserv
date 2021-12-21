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

const std::vector<ServerConfig> &Config::server() const
{
    return server_;
}

void Config::addServer(const ServerConfig &server_config)
{
    server_.push_back(server_config);
}
