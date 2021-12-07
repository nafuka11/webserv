#include "Config.hpp"
#include "LocationConfig.hpp"

Config::Config()
{
}

Config::~Config()
{
}

void Config::readFile()
{
    ServerConfig server_config = ServerConfig();

    /* 後でreadした結果をセットするに変更 */
    server_config.setListen(80);
    server_config.setServerName("webserv");

    server_config.addAllowMethods("GET");
    server_config.addAllowMethods("POST");

    server_config.setcgiExtension("php");
    server_config.setClientMaxBodySize(1000);

    server_config.addErrorPage(404, "/html/404.html");
    server_config.addErrorPage(501, "/html/50x.html");
    server_config.setUploadPath("/load");

    LocationConfig location_config = LocationConfig();
    location_config.setAlias("/test");
    location_config.setAutoIndex("off");

    location_config.addIndex("index.html");
    location_config.addIndex("index.php");

    location_config.addReturnRedirect(301, "http://$host/index.html");
    location_config.addReturnRedirect(302, "http://$host/index.html");

    server_config.addLocation("/", location_config);

    location_config.clear();


    location_config.setAlias("/test2");
    location_config.setAutoIndex("on");

    location_config.addIndex("index2.html");
    location_config.addIndex("index2.php");

    location_config.addReturnRedirect(301, "http://$host/index2.html");
    location_config.addReturnRedirect(302, "http://$host/index2.html");

    server_config.addLocation("/2/", location_config);
    /***************************************/

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
