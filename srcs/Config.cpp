#include "Config.hpp"

Config::Config()
{
}

Config::~Config()
{
}

void Config::readFile()
{
    ServerConfig server_config = ServerConfig();

    // 後でreadした結果をセットするに変更
    server_config.setListen(80);
    server_config.setServerName("webserv");
    server_config.addAllowMethods("GET");
    server_config.setcgiExtension("php");
    server_config.setClientMaxBodySize(1000);
    server_config.addErrorPage(404, "./404.html");
    server_config.setUploadPath("/load");

    // LocationConfig location_config  = LocationConfig();
    // server_config.addLocation("/", location_config);

    addServer(server_config);
}

const std::vector<ServerConfig> Config::server() const
{
    return server_;
}

void Config::addServer(const ServerConfig &server_config)
{
    server_.push_back(server_config);
}